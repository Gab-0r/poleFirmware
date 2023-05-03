
#Create library as INTERFACE
add_library(supplyManager INTERFACE)

target_sources(supplyManager
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/supplyManager.cpp
)

target_include_directories(supplyManager
        INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}
)

#Inlcuding libraries
target_link_libraries(supplyManager
        INTERFACE
            pico_stdlib
            hardware_gpio
            pico_time
)