project(si5351)

add_library(si5351 STATIC
    si5351.cpp
)

target_link_libraries(si5351 pico_stdlib hardware_i2c)


add_library(si5351_EXT_CLK STATIC
    si5351.cpp
)

target_compile_definitions(si5351_EXT_CLK PRIVATE
    EXT_CLK=1
)

target_link_libraries(si5351_EXT_CLK pico_stdlib hardware_i2c)

add_library(si5351_fpga_dac STATIC
    si5351.cpp
)

target_compile_definitions(si5351_fpga_dac PRIVATE
    DAC_fpga=1
)

target_link_libraries(si5351_fpga_dac pico_stdlib hardware_i2c)

add_library(si5351_fpga_dac_ext_clk STATIC
    si5351.cpp
)

target_compile_definitions(si5351_fpga_dac_ext_clk PRIVATE
    DAC_fpga_ext_clk=1
)

target_link_libraries(si5351_fpga_dac_ext_clk pico_stdlib hardware_i2c)