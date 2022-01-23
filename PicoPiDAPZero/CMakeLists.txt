project(PicoPiDAPZero)

set_source_files_properties(PicoPiDAPZero.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(PicoPiDAPZero_core1.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(PicoPiDAPZero_wav.cpp PROPERTIES COMPILE_FLAGS -fexceptions)
set_source_files_properties(bmp_shinonome16.cpp PROPERTIES COMPILE_FLAGS -fexceptions)

add_subdirectory(Si5351)

#-- Pico Pi DAP Zero general I2S DAC --
add_executable(PicoPiDAPZero
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        )

target_compile_definitions(PicoPiDAPZero PRIVATE

)

target_link_libraries(PicoPiDAPZero pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico pico_tag tft_espi TJpg_Decoder hardware_pwm si5351)

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
        cs4398.cpp
        )

target_compile_definitions(PicoPiDAPZero_CS4398 PRIVATE
    DAC_CS4398=1
    NO_SOFT_VOL=1
)

target_link_libraries(PicoPiDAPZero_CS4398 pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351)

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
        DacPlusPro.cpp
        )

target_compile_definitions(PicoPiDAPZero_DacPlusPro PRIVATE
    DAC_DacPlusPro=1
    NO_SOFT_VOL=1
)
        
target_link_libraries(PicoPiDAPZero_DacPlusPro pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351)

pico_enable_stdio_usb(PicoPiDAPZero_DacPlusPro 1)

pico_add_extra_outputs(PicoPiDAPZero_DacPlusPro)
#------------------------------------------


#-- Pico Pi DAP Zero + Zero_HAT_DAC_CS4398 --
add_executable(PicoPiDAPZero_Zero_HAT_DAC_CS4398
        PicoPiDAPZero.cpp
        PicoPiDAPZero_core1.cpp
        PicoPiDAPZero_wav.cpp
        getDirectoryList.cpp
        split.cpp
        bmp_shinonome16.cpp
        cs4398.cpp
        )

target_compile_definitions(PicoPiDAPZero_Zero_HAT_DAC_CS4398 PRIVATE
    DAC_Zero_HAT_DAC_CS4398=1
    NO_SOFT_VOL=1
)
        
target_link_libraries(PicoPiDAPZero_Zero_HAT_DAC_CS4398 pico_stdlib hardware_i2c hardware_adc ESP8266AudioPico_NO_SOFT_VOL pico_tag tft_espi TJpg_Decoder hardware_pwm si5351_Zero_HAT_DAC_CS4398)

pico_enable_stdio_usb(PicoPiDAPZero_Zero_HAT_DAC_CS4398 1)

pico_add_extra_outputs(PicoPiDAPZero_Zero_HAT_DAC_CS4398)
#--------------------------------------------
