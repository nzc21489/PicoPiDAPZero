project(PicoPiDAPZero)

set_source_files_properties(PicoPiDAPZero.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(PicoPiDAPZero_core1.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(PicoPiDAPZero_wav.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(bmp_shinonome16.cpp PROPERTIES COMPILE_FLAGS -fexceptions)

add_subdirectory(Si5351)
add_subdirectory(uac2)

#-- Pico Pi DAP Zero general I2S DAC --
add_executable(PicoPiDAPZero
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        )

target_compile_definitions(PicoPiDAPZero PRIVATE

)

target_link_libraries(PicoPiDAPZero pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2)

pico_enable_stdio_usb(PicoPiDAPZero 1)

pico_add_extra_outputs(PicoPiDAPZero)
#-------------------------------------


#-- Pico Pi DAP + Pi DAC Zero CS4398 --
add_executable(PicoPiDAPZero_CS4398
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        cs4398.cpp
        )

target_compile_definitions(PicoPiDAPZero_CS4398 PRIVATE
    DAC_CS4398=1
    NO_SOFT_VOL=1
)

target_link_libraries(PicoPiDAPZero_CS4398 pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2_CS4398)

pico_enable_stdio_usb(PicoPiDAPZero_CS4398 1)

pico_add_extra_outputs(PicoPiDAPZero_CS4398)
#--------------------------------------


#-- Pico Pi DAP Zero + HiFiBerry DAC+ Pro --
add_executable(PicoPiDAPZero_DacPlusPro
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        DacPlusPro.cpp
        )

target_compile_definitions(PicoPiDAPZero_DacPlusPro PRIVATE
    DAC_DacPlusPro=1
    NO_SOFT_VOL=1
)
        
target_link_libraries(PicoPiDAPZero_DacPlusPro pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2_DacPlusPro)

pico_enable_stdio_usb(PicoPiDAPZero_DacPlusPro 1)

pico_add_extra_outputs(PicoPiDAPZero_DacPlusPro)
#------------------------------------------


#-- Pico Pi DAP Zero + HiFiBerry DAC+ Pro SoftVol--
add_executable(PicoPiDAPZero_DacPlusPro_SoftVol
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        DacPlusPro.cpp
        )

target_compile_definitions(PicoPiDAPZero_DacPlusPro_SoftVol PRIVATE
    DAC_DacPlusPro=1
)
        
target_link_libraries(PicoPiDAPZero_DacPlusPro_SoftVol pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2_DacPlusPro)

pico_enable_stdio_usb(PicoPiDAPZero_DacPlusPro_SoftVol 1)

pico_add_extra_outputs(PicoPiDAPZero_DacPlusPro_SoftVol)
#------------------------------------------

#-- Pico Pi DAP Zero + Zero_HAT_DAC_CS4398 --
add_executable(PicoPiDAPZero_Zero_HAT_DAC_CS4398
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        cs4398.cpp
        )

target_compile_definitions(PicoPiDAPZero_Zero_HAT_DAC_CS4398 PRIVATE
    DAC_Zero_HAT_DAC_CS4398=1
    NO_SOFT_VOL=1
    EXT_CLK=1
)
        
target_link_libraries(PicoPiDAPZero_Zero_HAT_DAC_CS4398 pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351_EXT_CLK tinyusb_device tinyusb_board pico_uac2_Zero_HAT_DAC_CS4398)

pico_enable_stdio_usb(PicoPiDAPZero_Zero_HAT_DAC_CS4398 1)

pico_add_extra_outputs(PicoPiDAPZero_Zero_HAT_DAC_CS4398)
#--------------------------------------------

#-- Pico Pi DAP Zero + FPGA_DeltaSigma --
add_executable(PicoPiDAPZero_FPGA_DeltaSigma
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        )

target_compile_definitions(PicoPiDAPZero_FPGA_DeltaSigma PRIVATE
    DAC_FPGA_DeltaSigma=1
)

target_link_libraries(PicoPiDAPZero_FPGA_DeltaSigma pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351_fpga_dac tinyusb_device tinyusb_board pico_uac2)

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
        )

target_compile_definitions(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK PRIVATE
    DAC_FPGA_DeltaSigma_ExtClk=1
)

target_link_libraries(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351_fpga_dac_ext_clk tinyusb_device tinyusb_board pico_uac2)

pico_enable_stdio_usb(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK 1)

pico_add_extra_outputs(PicoPiDAPZero_FPGA_DeltaSigma_EXT_CLK)
#-------------------------------------

#-- Pico Pi DAP Zero + PCM1795 --
add_executable(PicoPiDAPZero_PCM1795
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        pcm1795.cpp
        )

target_compile_definitions(PicoPiDAPZero_PCM1795 PRIVATE
    DAC_PCM1795=1
    NO_SOFT_VOL=1
)
        
target_link_libraries(PicoPiDAPZero_PCM1795 pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351 tinyusb_device tinyusb_board pico_uac2)

pico_enable_stdio_usb(PicoPiDAPZero_PCM1795 1)

pico_add_extra_outputs(PicoPiDAPZero_PCM1795)
#------------------------------------------

#-- Pico Pi DAP Zero + AK449X --
add_executable(PicoPiDAPZero_AK449X
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        usb_serial.c
        ak449x.cpp
        )

target_compile_definitions(PicoPiDAPZero_AK449X PRIVATE
    DAC_AK449X=1
    NO_SOFT_VOL=1
    EXT_CLK=1
)
        
target_link_libraries(PicoPiDAPZero_AK449X pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351_EXT_CLK tinyusb_device tinyusb_board pico_uac2)

pico_enable_stdio_usb(PicoPiDAPZero_AK449X 1)

pico_add_extra_outputs(PicoPiDAPZero_AK449X)
#------------------------------------------