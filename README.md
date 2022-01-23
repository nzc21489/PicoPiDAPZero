# Pico Pi DAP Zero


## Building
```sh
cd PicoPiDAPZero
git clone https://github.com/nzc21489/pico_tag.git
git clone https://github.com/nzc21489/TFT_eSPI.git
git clone https://github.com/nzc21489/pico_i2s.git
git clone https://github.com/nzc21489/TJpg_Decoder.git
git clone https://github.com/nzc21489/fatfs.git
git clone https://github.com/nzc21489/ESP8266Audio.git
git clone https://github.com/nzc21489/sd_pico.git
git clone https://github.com/nzc21489/pico_sdio.git
git clone https://github.com/nzc21489/SPI_pico.git
git clone https://github.com/nemtrif/utfcpp.git
export PICO_SDK_PATH=path_to_pico-sdk
mkdir build
cd build
cmake ..
make
```