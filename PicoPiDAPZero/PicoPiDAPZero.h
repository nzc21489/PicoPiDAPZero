/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 nzc21489
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef PICOPIDAPZERO_H
#define PICOPIDAPZERO_H

#include <stdio.h>
#include <string>
#include <string.h>
#include "pico/stdlib.h"

#include <vector>

#include <array>
#include <vector>

#include "utf8.h"

#include "SPI.h"
#include "TFT_eSPI.h"

#include "bmp_shinonome16.h"

#include "getDirectoryList.h"

#include "split.h"

#include <TJpg_Decoder.h>

#include <sstream>
#include <ios>
#include <iomanip>

#include "NotoSans15.h"
#include "icons.h"
#define num_font NotoSans15
#define icon_font icons15

#include "pico_tag.h"

#include "ff.h"

#include "si5351.h"

#include "tusb.h"

#include "usb_serial.h"
#include "bsp/board.h"

#include "uac2_main.h"
#include "pico_uac2_program.h"

#include "pico_i2c.h"

#include "general_i2s.h"
#include "cs4398.h"
#include "pcm512x.h"
#include "pcm1795.h"
#include "ak449x.h"
#include "bd34352.h"
#include "dac.h"

#include "version_picopidap_zero.h"

extern FATFS FatFs;
extern FIL Fil;

extern vector<string> files;
extern vector<string> dirs;

extern uint32_t *stack_core1;
#define stack_core1_size 1024 * 10 / 4 // 10kB

extern getDirectoryList *get_directory_list;

extern volatile bool music_playing;
extern volatile bool music_playing_pre;

extern unsigned int duration;
extern volatile int play_start;

extern volatile uint8_t volume;

extern volatile bool get_dir_name_list;
extern volatile bool get_is_dir;
extern volatile bool file_update;

extern volatile bool pico_tag_wait;

extern volatile bool digital_filter_write;
extern volatile bool digital_filter_read;

extern string music_path;
extern vector<string> musics;
extern uint16_t music_select;

extern volatile bool player_screen_update;

extern volatile bool pause;
extern volatile uint32_t pause_time;

extern volatile bool let_music_stop;

extern uint8_t buttons_pin[10];

extern FIL fil;
extern DIR dir;
extern FRESULT fr;
extern string pmp_file;
extern string pmp_file2;
extern string pmp_path;
extern string pmp_vol;
extern string pmp_playing_mode;

extern volatile bool file_exist;
extern volatile bool volume_write;
extern volatile bool album_art_write;
extern volatile bool playing_mode_write;

extern volatile uint16_t rotate_count;
extern volatile uint8_t player_screen_rotate_num;

extern pico_tag *pico_tag1;

extern volatile uint32_t audio_start_seek;

extern volatile bool seeking;
extern volatile bool seek;
extern volatile bool audio_decoder;
extern volatile bool audio_decoder_pre;

extern uint8_t bit_pre;
extern uint32_t sampling_rate_pre;

#define sd_init_skip_button1 6
#define sd_init_skip_button2 3

#define usb_dac_button1 5
#define usb_dac_button2 9

extern volatile bool sd_status;

enum file_type
{
    wav_file,
    flac_file,
    mp3_file,
    aac_file,
    opus_file,
    file_start,
};
extern volatile file_type audio_type_pre;

extern volatile bool usb_dac_mode;

extern int digital_filter;

extern int playing_mode;

enum playing_mode_type
{
    playing_mode_normal,
    playing_mode_repeat_1,
    playing_mode_repeat_directory,
    playing_mode_repeat_all,
};

#define playing_mode_num 4

extern volatile bool repeat_next;
extern string jpeg_file_name;

extern dac *dac1;

extern uint32_t mclk_44_1k;
extern uint32_t mclk_48k;

extern volatile bool wait_core1_setup;

#ifdef i2c_sda_pin
#define sda_pin i2c_sda_pin
#else
#define sda_pin 2
#endif

#ifdef i2c_scl_pin
#define scl_pin i2c_scl_pin
#else
#define scl_pin 3
#endif

#ifdef ModelB
#ifdef i2c_sda_pin2
#define sda_pin2 i2c_sda_pin2
#else
#define sda_pin2 24
#endif

#ifdef i2c_scl_pin2
#define scl_pin2 i2c_scl_pin2
#else
#define scl_pin2 25
#endif

#ifdef SW1Pin
#define sw1_pin SW1Pin
#else
#define sw1_pin 23

extern bool softvol;
#endif

#endif // ModelB

void status_bar_left_update(uint8_t play_mode);
void status_bar_right_update();

#ifdef NO_SOFT_VOL
extern void change_volume(uint8_t vol);
#endif
extern void cs4398_setup();
extern void init_key();
extern void core1();
extern void draw_album_art();

extern void wav_start(uint8_t bit_num);
extern void wav_open(string wav_file_path);
extern bool wav_loop();
extern void wav_stop();
extern void wav_seek(uint32_t seek_point);
extern void wav_end();
extern void wav_set_gain(float gain);
extern void wav_set_end_byte(uint32_t end_byte);

#endif // PICOPIDAPZERO_H