project(si5351)

add_library(si5351 STATIC
    si5351.cpp
)

target_link_libraries(si5351 pico_stdlib hardware_i2c)


add_library(si5351_Zero_HAT_DAC_CS4398 STATIC
    si5351.cpp
)

target_compile_definitions(si5351_Zero_HAT_DAC_CS4398 PRIVATE
    DAC_Zero_HAT_DAC_CS4398=1
)

target_link_libraries(si5351_Zero_HAT_DAC_CS4398 pico_stdlib hardware_i2c)
