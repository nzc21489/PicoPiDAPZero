cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(PicoPiDAP)

pico_sdk_init()

set(PICO_GCC_TRIPLE "")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -Wl,-s -ffunction-sections -fdata-sections -ffat-lto-objects")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -Wl,-s -ffunction-sections -fdata-sections -fno-threadsafe-statics -ffat-lto-objects")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections --use-linker-plugin")

set(INCLUDE_DIR
  ${PROJECT_SOURCE_DIR}/PicoPiDAPZero
  ${PROJECT_SOURCE_DIR}/PicoPiDAPZero/Si5351
  ${PROJECT_SOURCE_DIR}/pico_tag
  ${PROJECT_SOURCE_DIR}/TJpg_Decoder/src
  ${PROJECT_SOURCE_DIR}/ESP8266Audio/src
  ${PROJECT_SOURCE_DIR}/fatfs
  ${PROJECT_SOURCE_DIR}/sd_pico
  ${PROJECT_SOURCE_DIR}/utfcpp/source
  ${PROJECT_SOURCE_DIR}/utfcpp/source/utf8
  ${PROJECT_SOURCE_DIR}/pico_i2s
  ${PROJECT_SOURCE_DIR}/pico_sdio
  ${PROJECT_SOURCE_DIR}/pico_sdio/include
  ${PROJECT_SOURCE_DIR}/TFT_eSPI
  ${PROJECT_SOURCE_DIR}/SPI_pico
  ${PICO_SDK_PATH}
)

add_compile_definitions(
        PICO_AUDIO_I2S_CLOCK_PIN_BASE=19
        PICO_AUDIO_I2S_DATA_PIN=18
        PICO_SD_CLK_PIN=10
        PICO_SD_CMD_PIN=11
        PICO_SD_DAT0_PIN=12
        ARDUINO_ARCH_RP2040
        sdio_4pin=1
        )

include_directories("${INCLUDE_DIR}")

add_subdirectory(PicoPiDAPZero)
add_subdirectory(ESP8266Audio)
add_subdirectory(pico_i2s)
add_subdirectory(fatfs)
add_subdirectory(sd_pico)
add_subdirectory(pico_sdio)
add_subdirectory(TFT_eSPI)
add_subdirectory(SPI_pico)
add_subdirectory(pico_tag)
add_subdirectory(TJpg_Decoder)
