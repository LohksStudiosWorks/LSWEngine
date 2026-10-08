#include <LSWE/lswe.hpp>
#include <iostream>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <cstdint>
#include <chrono>
#include <thread>
#include <regex>
#include <memory>
#include <format>

using namespace LSWE;

struct playing {
    double
        time_curr;
    std::string
        path_now,
        name_curr,
        time_curr_str,
        time_total_str;
};

struct playing_instance {
    Utility::File file;
    Audio::Sample sample;
    Audio::SampleInstance instance;

    playing_instance(const char* buf, const size_t len)
        : file(Utility::File::open_mem((void*)buf, len, "r")),
          sample(Audio::Sample::load(file, ".ogg")),
          instance(Audio::SampleInstance::create(sample))
    {}
};

std::string sec_to_str(uint64_t sec);
std::string sanitize_url(std::string url);
playing calculate_now(nlohmann::ordered_json selected);
std::string sanitize_js_to_json(const std::string& input);
bool download_from(const char* path, std::string& data);

const std::string original_path = "https://lohkat.github.io/lofi/";
constexpr char playlist_source_json[] = "https://lohkat.github.io/lofi/js/times.js";

int main(int argc, char **argv) {
    std::cout << "Tunning in..." << std::endl;

    auto voice = Audio::Voice::create();
    auto mixer = Audio::Mixer::create();

    voice << mixer;

    /* Get listing here and parse JS -> JSON -> C++ */
    std::string buf;
    if (!download_from(playlist_source_json, buf)) return 1;

    buf = buf.substr(buf.find('['));
    while(buf.back() != ']') buf.pop_back();
    buf = sanitize_js_to_json(buf);

    auto plist = nlohmann::ordered_json::parse(buf);
    auto chosen = plist[0];

    std::cout << "Got links, thinking..." << std::endl;

    using player = std::pair<std::string, std::unique_ptr<playing_instance>>;

    player plays;
    
    while(1) {
        playing play = calculate_now(chosen);

        if (play.path_now != plays.first) {
            plays.first = play.path_now;
            if (!download_from(sanitize_url(plays.first).c_str(), buf)) {
                std::cout << "\nFail loading from '" << plays.first << "'. Trying again..." << std::endl;
                plays.first.clear();
            }
            else {
                if (plays.second) {
                    plays.second->instance.stop();
                }
                plays.second = std::make_unique<playing_instance>(buf.data(), buf.size());
                mixer << plays.second->instance;
                plays.second->instance.set_position(plays.second->sample.get_frequency() * play.time_curr);
                plays.second->instance.play();
            }
        }

        std::cout << "\r" << play.time_total_str << " :: " << play.name_curr << " :: " << play.time_curr_str;
        std::cout << std::flush;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}

std::string sec_to_str(uint64_t orig) {
    const uint64_t secs = orig % 60;
    orig /= 60;
    const uint64_t mins = orig % 60;
    orig /= 60;
    const uint64_t hour = orig;

    return std::format("{:02}:{:02}:{:02}", hour, mins, secs);
}

std::string sanitize_url(std::string url) {
    size_t pos = 0;
    while ((pos = url.find(" ", pos)) != std::string::npos) {
        url.replace(pos, 1, "%20");
        pos += 3; // Move past the newly inserted "%20"
    }
    return url;
}

playing calculate_now(nlohmann::ordered_json selected) {
    // 1. Get precise floating-point seconds (matches JS Number(new Date()) * 0.001)
    double time_now = std::chrono::duration<double>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    const int64_t factor = 11987;

    // 2. Divide by a float (1000.0) to prevent integer truncation
    double total_time_sec = selected["total_time"].get<int64_t>() / 1000.0;

    // 3. Use std::fmod for floating-point modulo instead of %
    double time_full = std::fmod(time_now, total_time_sec);
    
    double show_time = 0;
    int64_t idx = 0;

    const auto tracks = selected["tracks"];

    for(int64_t i = 0; i < tracks.size(); ++i) {
        idx = (factor + idx) % tracks.size();

        // The rest of your logic remains exactly the same
        double track_time = static_cast<double>(tracks[idx][1].get<int64_t>()) * 0.001;
        if (time_full > track_time) {
            time_full -= track_time;
            show_time += track_time;
        }
        else break;
    }

    const std::string raw_name = tracks[idx][0].get<std::string>();

    constexpr char expected_beg_str[] = "tracks/lofi/";
    constexpr size_t expected_off = std::strlen(expected_beg_str);

    std::string name = raw_name.substr(expected_off);
    name = name.size() > 4 ? name.substr(0, name.size() - 4) : "ERR";


    return playing {
        .time_curr = time_full,
        .path_now = original_path + raw_name,
        .name_curr = name,
        .time_curr_str = sec_to_str(time_full),
        .time_total_str = sec_to_str(show_time + time_full)
    };
}

std::string sanitize_js_to_json(const std::string& input) {
    // 1. Wrap unquoted keys in double quotes.
    // Matches '{' or ',' followed by whitespace, an identifier, optional space, and ':'
    std::regex key_regex(R"(([{},]\s*)([a-zA-Z_]\w*)\s*:)");
    std::string cur_str = std::regex_replace(input, key_regex, "$1\"$2\":");

    // 2. Remove trailing commas. 
    // Your example has them (e.g., after the last array item), but strict JSON 
    // and nlohmann::ordered_json will throw a parse error if they are left in.
    std::regex trailing_comma(R"(,(\s*[\]}]))");
    cur_str = std::regex_replace(cur_str, trailing_comma, "$1");

    return cur_str;
}

static size_t __WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    auto* buffer = static_cast<std::string*>(userp);
    
    auto* data_ptr = static_cast<uint8_t*>(contents);
    buffer->insert(buffer->end(), data_ptr, data_ptr + total_size);
    
    return total_size;
}

bool download_from(const char* path, std::string& data) {
    CURL* curl = curl_easy_init();
    if (!curl) { return false; }

    data.clear();

    curl_easy_setopt(curl, CURLOPT_URL, path);
    
    // Follow HTTP redirects (301, 302, etc.)
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    // Hook up the callback and pass the vector's address as user data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "LSWE-Example/1.0");

    CURLcode res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}