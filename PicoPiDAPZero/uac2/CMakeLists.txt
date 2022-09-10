project(pico_uac2)

set_source_files_properties(pico_uac2_program.cpp PROPERTIES COMPILE_FLAGS -fexceptions)

#-- Pico Pi DAP Zero --
add_library(pico_uac2 STATIC
        uac2_main.cpp
        usb_descriptors.cpp
        pico_uac2_program.cpp
        )

target_link_libraries(pico_uac2 pico_stdlib tinyusb_device
  tinyusb_board pico_audio_i2s si5351 hardware_i2c)
#-------------------------------------

#-- Pico Pi DAP Zero Model B --
add_library(pico_uac2_ModelB STATIC
        uac2_main.cpp
        usb_descriptors.cpp
        pico_uac2_program.cpp
        )

target_compile_definitions(pico_uac2_ModelB PRIVATE
    ModelB=1
)

target_link_libraries(pico_uac2_ModelB pico_stdlib tinyusb_device
  tinyusb_board pico_audio_i2s si5351 hardware_i2c)
#-------------------------------------