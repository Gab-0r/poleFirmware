#TODO: comment this file

add_library(lightManager INTERFACE)

target_sources(lightManager
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/lightManager.cpp
)

target_include_directories(lightManager
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(lightManager
        INTERFACE
            pico_stdlib
)