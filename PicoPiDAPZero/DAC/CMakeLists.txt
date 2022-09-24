project(DAC)

add_library(dac STATIC
    cs4398.cpp
    pcm512x.cpp
    pcm1795.cpp
    ak449x.cpp
    bd34352.cpp
    general_i2s.cpp
)

target_link_libraries(dac pico_stdlib hardware_i2c)