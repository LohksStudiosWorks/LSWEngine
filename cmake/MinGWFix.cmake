if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE 
        -static-libgcc 
        -static-libstdc++ 
        -static
    )
endif()