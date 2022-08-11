project(pico_uac2)

#-- Pico Pi DAP Zero general I2S DAC --
add_library(pico_uac2 STATIC
        uac2_main.cpp
        usb_descriptors.cpp
        pico_uac2_program.cpp
        )

target_link_libraries(pico_uac2 pico_stdlib tinyusb_device
  tinyusb_board pico_audio_i2s si5351 hardware_i2c)
#-------------------------------------

#-- Pico Pi DAP + Pi DAC Zero CS4398 --
add_library(pico_uac2_CS4398 STATIC
        uac2_main.cpp
        usb_descriptors.cpp
        pico_uac2_program.cpp
        ../cs4398.cpp
        )

target_compile_definitions(pico_uac2_CS4398 PRIVATE
    DAC_CS4398=1
    NO_SOFT_VOL=1
)

target_link_libraries(pico_uac2_CS4398 pico_stdlib tinyusb_device
  tinyusb_board pico_audio_i2s si5351 hardware_i2c)
#--------------------------------------

#-- Pico Pi DAP Zero + HiFiBerry DAC+ Pro --
add_library(pico_uac2_DacPlusPro STATIC
        uac2_main.cpp
        usb_descriptors.cpp
        pico_uac2_program.cpp
        ../DacPlusPro.cpp
        )

target_compile_definitions(pico_uac2_DacPlusPro PRIVATE
    DAC_DacPlusPro=1
    NO_SOFT_VOL=1
)

target_link_libraries(pico_uac2_DacPlusPro pico_stdlib tinyusb_device
  tinyusb_board pico_audio_i2s hardware_i2c)
#------------------------------------------

#-- Pico Pi DAP Zero + Zero_HAT_DAC_CS4398 --
add_library(pico_uac2_Zero_HAT_DAC_CS4398 STATIC
        uac2_main.cpp
        usb_descriptors.cpp
        pico_uac2_program.cpp
        ../cs4398.cpp
        )

target_compile_definitions(pico_uac2_Zero_HAT_DAC_CS4398 PRIVATE
    DAC_CS4398=1
    NO_SOFT_VOL=1
)

target_link_libraries(pico_uac2_Zero_HAT_DAC_CS4398 pico_stdlib tinyusb_device
  tinyusb_board pico_audio_i2s si5351_EXT_CLK hardware_i2c)
#--------------------------------------------
