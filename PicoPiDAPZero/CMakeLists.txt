project(PicoPiDAPZero)

set_source_files_properties(PicoPiDAPZero.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(PicoPiDAPZero_core1.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(PicoPiDAPZero_wav.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(bmp_shinonome16.cpp PROPERTIES COMPILE_FLAGS -fexceptions)

add_subdirectory(Si5351)
add_subdirectory(uac2)
add_subdirectory(DAC)

#-- Pico Pi DAP Zero --
add_executable(PicoPiDAPZero
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        pico_i2c.cpp
        i2c_devices.cpp
        )

target_compile_definitions(PicoPiDAPZero PRIVATE

)

target_link_libraries(PicoPiDAPZero pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2 dac)

pico_enable_stdio_usb(PicoPiDAPZero 1)

pico_add_extra_outputs(PicoPiDAPZero)
#-------------------------------------

#-- Pico Pi DAP Zero Model B --
add_executable(PicoPiDAPZero_ModelB
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        pico_i2c.cpp
        i2c_devices.cpp
        )

target_compile_definitions(PicoPiDAPZero_ModelB PRIVATE
        ModelB=1
)

target_link_libraries(PicoPiDAPZero_ModelB pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2_ModelB dac)

pico_enable_stdio_usb(PicoPiDAPZero_ModelB 1)

pico_add_extra_outputs(PicoPiDAPZero_ModelB)
#-------------------------------------

#-- Pico Pi DAP Zero External clock --
add_executable(PicoPiDAPZero_EXT_CLK
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        pico_i2c.cpp
        i2c_devices.cpp
        )

target_compile_definitions(PicoPiDAPZero_EXT_CLK PRIVATE
        EXT_CLK=1
)

target_link_libraries(PicoPiDAPZero_EXT_CLK pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2 dac)

pico_enable_stdio_usb(PicoPiDAPZero_EXT_CLK 1)

pico_add_extra_outputs(PicoPiDAPZero_EXT_CLK)
#-------------------------------------

#-- Pico Pi DAP Zero + FPGA_DeltaSigma --
add_executable(PicoPiDAPZero_FPGA_DeltaSigma
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        pico_i2c.cpp
        i2c_devices.cpp
        )

target_compile_definitions(PicoPiDAPZero_FPGA_DeltaSigma PRIVATE
    DAC_FPGA_DeltaSigma=1
)

target_link_libraries(PicoPiDAPZero_FPGA_DeltaSigma pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2 dac)

pico_enable_stdio_usb(PicoPiDAPZero_FPGA_DeltaSigma 1)

pico_add_extra_outputs(PicoPiDAPZero_FPGA_DeltaSigma)
#-------------------------------------

#-- Pico Pi DAP Zero + FPGA_DeltaSigma_EXT_CLK --
add_executable(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        pico_i2c.cpp
        i2c_devices.cpp
        )

target_compile_definitions(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK PRIVATE
    DAC_FPGA_DeltaSigma=1
    EXT_CLK=1
)

target_link_libraries(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2 dac)

pico_enable_stdio_usb(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK 1)

pico_add_extra_outputs(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK)
#-------------------------------------