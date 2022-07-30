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

#include "PicoPiDAPZero.h"

FATFS FatFs;
FIL Fil;

vector<string> files;
vector<string> dirs;

uint32_t *stack_core1;

getDirectoryList *get_directory_list;

volatile bool key_interrupt = false;

string home_string[4] = {
    "Explorer",
    "USB-DAC",
    "DAC",
    "System"};

int x_jpeg = 0;
int y_jpeg = 0;

uint16_t x_tft = 0;
uint16_t y_tft = 0;

uint16_t tft_written_check[2] = {0, 0};
bool tft_start = true;

double jpeg_scale_factor = 0.0;

volatile bool home_screen_mode = true;
volatile uint8_t player_mode = 0; //0:explorer, 1:System
enum player_mode_type
{
    player_explorer = 0,
    usb_dac = 1,
    dac = 2,
    player_system = 3
};

volatile uint16_t player_select = 0;
volatile uint16_t explorer_select = 0;

#define time_long 500
#define time_long_interval 100
volatile uint32_t time_hold = 0;
volatile uint32_t time_hold_multi = 0;

volatile uint8_t key_num = 100;
volatile bool key_long = false;
volatile uint64_t key_count = 100;
volatile bool key_long_pre = false;
volatile uint8_t key_num_pre = 0;

volatile uint16_t dir_start = 0;
volatile uint16_t dir_selected = 0;

volatile bool music_playing = 0;
volatile bool music_playing_pre = 0;

unsigned int duration;

volatile int play_start;
volatile uint32_t play_update;

volatile bool play_time_update = false;

volatile bool player_screen_mode = false;

volatile uint8_t volume = 40;

volatile bool get_dir_name_list = false;
volatile bool get_is_dir = false;
volatile bool file_update = false;

volatile bool pico_tag_wait = false;

volatile bool digital_filter_write = false;
volatile bool digital_filter_read = false;

string music_path = "";
vector<string> musics;
uint16_t music_select = 65534;

volatile bool player_screen_update = false;

volatile bool pause = false;

volatile uint32_t pause_time = 0;

volatile bool let_music_stop = false;

uint8_t buttons_pin[10] = {8, 21, 0, 27, 26, 17, 9, 16, 1, 28};

float conversion_factor = 3.3f / (1 << 12);

float v_bat = 0.0;
string v_bat_string = "";

volatile bool keypad = false;

volatile bool display_on = true;

string information_string[7] = {
    "",
    "        Pico Pi DAP Zero",
    dac_picopidap_zero,
    "",
    version_picopidap_zero,
    "",
    ""};

uint32_t system_bat_time = 0;

FIL fil;
DIR dir;
FRESULT fr;
string pmp_file = "file.txt";
string pmp_file2 = "file2.txt";
string pmp_path = ".ppdap";
string pmp_vol = "volume.txt";

volatile bool file_exist = false;
volatile bool volume_write = false;
volatile bool album_art_write = false;

volatile uint16_t rotate_count = 0;
volatile bool font_larger_than_tft[14] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false};
volatile bool font_larger_than_tft_pre = false;
volatile uint16_t font_pixel[14] = {0};

#define font_larger_than_tft_wait_interval 25
#define font_larger_than_tft_last_wait 1000
uint32_t font_larger_than_tft_wait_time = 0;
volatile uint8_t player_screen_rotate_num = 1;

pico_tag *pico_tag1;

volatile uint32_t audio_start_seek = 0;

volatile bool seeking = false;
volatile bool seek = false;
volatile bool audio_decoder = false;
volatile bool audio_decoder_pre = false;

#define status_bar_height 15

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite *spr_player[4];
TFT_eSprite spr_status_bar[2] = {TFT_eSprite(&tft), TFT_eSprite(&tft)}; // 0:left, 1:right
#define status_bar_right_length 93                                      //15 * 2 + 9 * (3 + 4)
#define status_bar_left_length (TFT_WIDTH - status_bar_right_length)
TFT_eSprite *spr_home[5];
TFT_eSprite *spr_information[7];
vector<TFT_eSprite> sprite_explorer;

bmp_shinonome16 *font_bmp;

array<string, 4> player_screen;

uint32_t reboot_count = 0x7fffffff;
bool system_first_time = false;
bool dac_display_first_time = false;

static const uint8_t brightness[5] = {100, 45, 42, 38, 35};

uint16_t w_jpeg = 0, h_jpeg = 0;

#define album_art_width 160
#define album_art_height 160

#define num_keys 10 // how much keys (buttons)

enum status_bar_type
{
    status_bar_left = 0,
    status_bar_right,
};

#define explorer_row_max 15

int brightness_select = 0;

#define seek_step 1000

string jpeg_file_name = "";

string music_file_data = "";

volatile bool sd_status = true;

volatile int jpeg_file_size_pre = 0;

volatile bool usb_dac_mode = false;

int digital_filter = -1;

int playing_mode = playing_mode_normal;
volatile bool repeat_next = false;

void status_bar_left_update(uint8_t play_mode);
void status_bar_right_update();

void get_v_bat();
uint8_t get_key();
void gpio_callback(uint gpio, uint32_t events);
void playing_time_update();
void playerScreen();
void home_up();
void home_down();
void explorer_up();
void explorer_down();
void explorer_in();
void exploer_out();
array<char, 12> get_char(int bin_num);
int get_width(int num);
uint32_t get_num(char &chara);
FRESULT scan_files_to_string(
    char *path /* Start node to be scanned (***also used as work area***) */
);
FRESULT scan_files(
    char *path /* Start node to be scanned (***also used as work area***) */
);
void init_key();
void core1();

void explorer_tft(vector<TFT_eSprite> *sprite_explorer_tft, uint16_t start, uint16_t num);
void information_tft();

#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
void change_volume(uint8_t vol)
{
    change_volume_cs4398(vol);
}
#endif

#if defined(DAC_DacPlusPro) && defined(NO_SOFT_VOL)
void change_volume(uint8_t vol)
{
    change_volume_DacPlusPro(vol);
}
#endif

void change_digital_filter()
{
#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
    cs4398_change_digital_filter(digital_filter_nums[digital_filter]);
#elif defined(DAC_DacPlusPro)
    DacPlusPro_change_digital_filter(digital_filter_nums[digital_filter]);
#else

#endif
}

void get_v_bat()
{
    v_bat = adc_read() * conversion_factor * 3.0;
    stringstream v_bat_stream;
    v_bat_stream << setprecision(2) << v_bat << "V"; // x.xV
    v_bat_string = v_bat_stream.str();
}

void get_v_bat_2()
{
    v_bat = adc_read() * conversion_factor * 3.0;
    stringstream v_bat_stream;
    v_bat_stream << setprecision(3) << " " << v_bat << "V"; // x.xx V
    v_bat_string = v_bat_stream.str();
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) // call back from Tjpg_Decoder
{
    int x_offset = (tft.width() - album_art_width) / 2;
    if (w_jpeg <= album_art_width && h_jpeg <= album_art_height) // fast
    {
        TFT_eSprite tft_jpeg = TFT_eSprite(&tft);
        tft_jpeg.createSprite(w, h);

        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                tft_jpeg.drawPixel(i, j, bitmap[i + w * j]); // draw on sprite
            }
        }
        tft_jpeg.pushSprite(x_offset + x, y + status_bar_height); // draw display from sprite
    }
    else // slow
    {
        for (int j = 0; j < h; j++)
        {
            for (int i = 0; i < w; i++)
            {
                x_tft = (int)((double)(x + i) / (jpeg_scale_factor));
                y_tft = (int)((double)(y + j) / (jpeg_scale_factor));

                if ((tft_written_check[0] < x_tft) || (tft_written_check[1] < y_tft) || tft_start) // need more check
                {
                    tft_start = false;
                    tft.drawPixel(x_offset + x_tft, y_tft + status_bar_height, bitmap[i + w * j]); // draw display
                    tft_written_check[0] = x_tft;
                    tft_written_check[1] = y_tft;
                }
            }
        }
    }

    if (play_time_update)
    {
        playing_time_update();
        play_time_update = false;
    }

    // Return 1 to decode next block
    return 1;
}

void album_art(string file_name, bool embedded, int art_offset = 0, int art_size = 0)
{
    tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);

    int jpeg_scale = 1;
    x_jpeg = 0;
    y_jpeg = 0;

    x_tft = 0;
    y_tft = 0;

    tft_written_check[0] = 0;
    tft_written_check[1] = 0;

    jpeg_scale_factor = 0.0;

    uint16_t w = 0, h = 0;
    if (embedded)
    {
        TJpgDec->getSdAudioJpgSize(&w, &h, file_name.c_str(), art_offset, art_size);
    }
    else
    {
        TJpgDec->getSdJpgSize(&w, &h, file_name.c_str());
    }

    if (w > 0)
    {
        if (w < album_art_width * 2)
        {
            jpeg_scale = 1;
            TJpgDec->setJpgScale(jpeg_scale);
        }
        else if (w < album_art_width * 4)
        {
            jpeg_scale = 2;
            TJpgDec->setJpgScale(jpeg_scale);
        }
        else if (w < album_art_width * 8)
        {
            jpeg_scale = 4;
            TJpgDec->setJpgScale(jpeg_scale);
        }
        else
        {
            jpeg_scale = 8;
            TJpgDec->setJpgScale(jpeg_scale);
        }

        if (w >= h)
        {
            jpeg_scale_factor = (double)w / ((double)album_art_width * (double)(jpeg_scale));
        }
        else
        {
            jpeg_scale_factor = (double)h / ((double)album_art_width * (double)(jpeg_scale));
        }

        tft_start = true;

        if (embedded)
        {
            TJpgDec->drawSdAudioJpg(0, 0, file_name.c_str(), art_offset, art_size);
        }
        else
        {
            TJpgDec->drawSdJpg(0, 0, file_name.c_str());
        }
    }
}

uint32_t get_cover_offset(string file_name, uint32_t offset) // get jpeg offset for embedded album art
{
    FIL jpeg_fil;
    f_open(&jpeg_fil, file_name.c_str(), FA_READ);
    f_lseek(&jpeg_fil, offset);
    uint8_t read4[4] = {0, 0, 0, 0};
    uint16_t i = 0;
    while (true)
    {
        UINT br;
        uint8_t buff;
        f_read(&jpeg_fil, &buff, 1, &br);
        read4[0] = buff;
        if (read4[3] == 0xFF && read4[2] == 0xD8 && read4[1] == 0xFF && read4[0] == 0xE0) // jpeg start
        {
            int tell;
            tell = f_tell(&jpeg_fil);
            f_close(&jpeg_fil);
            return (tell - 4);
        }
        else
        {
            for (int y = 0; y < 3; y++)
            {
                read4[3 - y] = read4[2 - y];
            }
        }
        i++;
        if (i > 300 * 4) // 300 * 4 : random value, need adjusted
        {
            f_close(&jpeg_fil);
            return 0;
        }
    }
}

void draw_font(TFT_eSprite *spr_font, unsigned char *bmp, bool half_width, int x)
{
    if (half_width)
    {
        for (int y_pixel = 0; y_pixel < 16; y_pixel++)
        {
            for (int x_pixel = 0; x_pixel < 8; x_pixel++)
            {
                if ((bmp[2 * y_pixel] & (1 << x_pixel)) > 0)
                {
                    spr_font->drawPixel((8 - x_pixel) + x, y_pixel, TFT_WHITE);
                }
            }
        }
    }
    else
    {
        for (int y_pixel = 0; y_pixel < 16; y_pixel++)
        {
            for (int x_pixel = 0; x_pixel < 16; x_pixel++)
            {
                if ((bmp[2 * y_pixel] & (1 << x_pixel)) > 0)
                {
                    spr_font->drawPixel((8 - x_pixel) + x, y_pixel, TFT_WHITE);
                }
                if ((bmp[2 * y_pixel + 1] & (1 << x_pixel)) > 0)
                {
                    spr_font->drawPixel((16 - x_pixel) + x, y_pixel, TFT_WHITE);
                }
            }
        }
    }
}

uint8_t get_key()
{
    sleep_ms(10);
    uint8_t key = 0;
    for (key = 0; key < num_keys; key++)
    {
        if (!gpio_get(buttons_pin[key]))
        {
            break;
        }
    }
    key++;

    return key;
}

void gpio_callback(uint gpio, uint32_t events)
{
    key_interrupt = true;
}

void player_tft(int num, TFT_eSprite *sprite_player_tft)
{
    sprite_player_tft->fillScreen(TFT_BLACK);
    font_larger_than_tft[num] = false;
    uint16_t sprite_width = 0;
    vector<array<char, 32>> bmp_array_vector;
    vector<bool> width_is_16;

    vector<string> string_vector;
    int pos;
    unsigned char lead;
    int char_size;

    for (pos = 0; pos < player_screen[num].size(); pos += char_size) // get character size (byte)
    {

        lead = player_screen[num][pos];

        if (lead < 0x80)
        {
            char_size = 1;
        }
        else if (lead < 0xE0)
        {
            char_size = 2;
        }
        else if (lead < 0xF0)
        {
            char_size = 3;
        }
        else
        {
            char_size = 4;
        }
        string_vector.push_back(player_screen[num].substr(pos, char_size));
    }

    for (int j = 0; j < string_vector.size(); j++)
    {
        string font_character = string_vector[j];
        font_bmp->bmp_shinonome16_get(font_character);
        bmp_array_vector.push_back(font_bmp->character);
        width_is_16.push_back(font_bmp->width_is_16);
        if (font_bmp->width_is_16)
        {
            sprite_width += 16;
        }
        else
        {
            sprite_width += 8;
        }
    }
    if (sprite_width < tft.width())
    {
        sprite_width = tft.width();
    }

    sprite_player_tft->deleteSprite();
    sprite_player_tft->setColorDepth(1);
    sprite_player_tft->createSprite(sprite_width, 16);
    unsigned char bmp[32];

    uint16_t x_sprite = 0;
    for (int j = 0; j < string_vector.size(); j++)
    {
        for (int k = 0; k < 32; k++) // 8x32=16x16
        {
            bmp[k] = bmp_array_vector[j][k];
        }
        if (width_is_16[j])
        {
            draw_font(sprite_player_tft, bmp, false, x_sprite);
            x_sprite += 16;
        }
        else
        {
            draw_font(sprite_player_tft, bmp, true, x_sprite);
            x_sprite += 8;
        }
    }
    if (x_sprite > tft.width())
    {
        font_larger_than_tft[num] = true;
    }
    font_pixel[num] = x_sprite;
}

void get_music_file_data()
{
    music_file_data = to_string(pico_tag1->bits_per_sample) + "/" + to_string(pico_tag1->sampling_rate) + " ";
    switch (audio_type_pre)
    {
    case file_start:
        music_file_data += "    ";
        break;

    case wav_file:
        music_file_data += " WAV";
        break;

    case flac_file:
        music_file_data += "FLAC";
        break;

    case mp3_file:
        music_file_data += " MP3";
        break;

    case aac_file:
        music_file_data += " AAC";
        break;

    case opus_file:
        music_file_data += "OPUS";
        break;

    default:
        break;
    }
}

void playing_time_update()
{
    int playing_time_now;
    if (pause || !music_playing)
    {
        playing_time_now = (int)(((long)pause_time - (long)play_start) / 1000);
    }
    else
    {
        playing_time_now = (int)(((int)to_ms_since_boot(get_absolute_time()) - play_start) / 1000);
    }

    if (playing_time_now < 0)
    {
        playing_time_now = 0;
    }

    stringstream p_time;
    p_time << (int)(playing_time_now / 60) << ":" << setfill('0') << right << setw(2) << playing_time_now % 60 << " / " << (int)(duration / 60) << ":" << setfill('0') << right << setw(2) << duration % 60;
    string playing_time = p_time.str();

    while ((playing_time.length() + music_file_data.length()) < tft.width() / 8)
    {
        playing_time += " ";
    }

    if (playing_time_now <= 1)
    {
        get_music_file_data();
    }

    player_screen[0] = playing_time + music_file_data;

    player_tft(0, *spr_player);

    if (display_on)
    {
        spr_player[0]->pushSprite(0, album_art_height + status_bar_height);
        status_bar_right_update();
    }
}

void playerScreen()
{
    get_v_bat();

    string title = pico_tag1->title;
    string album = pico_tag1->album;
    string artist = pico_tag1->artist;

    if (title == "")
    {
        title = musics[music_select];
    }

    player_screen.fill("");
    player_screen[0] = "";
    player_screen[1] = title;
    player_screen[2] = album;
    player_screen[3] = artist;

    get_music_file_data();

    for (int i = 0; i < 4; i++)
    {
        player_tft(i, spr_player[i]);
        spr_player[i]->pushSprite(0, album_art_height + status_bar_height + 16 * i);
    }

    playing_time_update();
}

bool is_audio_file(string file_to_check)
{
    string flac = ".flac";
    string opus = ".opus";
    string wav = ".wav";
    string mp3 = ".mp3";
    string aac = ".aac";
    if ((file_to_check.size() >= flac.size() &&
         file_to_check.find(flac, file_to_check.size() - flac.size()) != std::string::npos) ||
        (file_to_check.size() >= opus.size() &&
         file_to_check.find(opus, file_to_check.size() - opus.size()) != std::string::npos) ||
        (file_to_check.size() >= wav.size() &&
         file_to_check.find(wav, file_to_check.size() - wav.size()) != std::string::npos) ||
        (file_to_check.size() >= mp3.size() &&
         file_to_check.find(mp3, file_to_check.size() - mp3.size()) != std::string::npos) ||
        (file_to_check.size() >= aac.size() &&
         file_to_check.find(aac, file_to_check.size() - aac.size()) != std::string::npos))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void status_bar_left_update(uint8_t play_mode)
{
    spr_status_bar[status_bar_left].fillScreen(TFT_BLACK);
    spr_status_bar[status_bar_left].setCursor(0, 0);
    spr_status_bar[status_bar_left].loadFont(icon_font);
    if (play_mode == 1)
    {
        spr_status_bar[status_bar_left].print("g"); //play
    }
    else if (play_mode == 0)
    {
        spr_status_bar[status_bar_left].print("h"); //pause
    }
    spr_status_bar[status_bar_left].unloadFont();
    if (!sd_status)
    {
        spr_status_bar[status_bar_left].loadFont(num_font);
        spr_status_bar[status_bar_left].print("  SD Error");
        spr_status_bar[status_bar_left].unloadFont();
    }

    switch (playing_mode)
    {
    case playing_mode_normal:
        break;

    case playing_mode_repeat_1:
        spr_status_bar[status_bar_left].loadFont(num_font);
        spr_status_bar[status_bar_left].print(" R1");
        spr_status_bar[status_bar_left].unloadFont();
        break;

    case playing_mode_repeat_all:
        spr_status_bar[status_bar_left].loadFont(num_font);
        spr_status_bar[status_bar_left].print(" RA");
        spr_status_bar[status_bar_left].unloadFont();
        break;

    default:
        break;
    }

    spr_status_bar[status_bar_left].pushSprite(0, 0);
}

void status_bar_right_update()
{
    stringstream p_volume;
    p_volume << setfill('0') << right << setw(3) << (int)volume;
    string volume_str = p_volume.str();
    spr_status_bar[1].fillScreen(TFT_BLACK);

    spr_status_bar[status_bar_right].setCursor(0, 0);

    spr_status_bar[status_bar_right].loadFont(icon_font);
    spr_status_bar[status_bar_right].print("d"); //volume
    spr_status_bar[status_bar_right].unloadFont();

    spr_status_bar[status_bar_right].loadFont(num_font);
    spr_status_bar[status_bar_right].print(volume_str.c_str());
    spr_status_bar[status_bar_right].unloadFont();

    spr_status_bar[status_bar_right].loadFont(icon_font);
    spr_status_bar[status_bar_right].print("c"); //battery
    spr_status_bar[status_bar_right].unloadFont();

    spr_status_bar[status_bar_right].loadFont(num_font);
    spr_status_bar[status_bar_right].print(v_bat_string.c_str());
    spr_status_bar[status_bar_right].unloadFont();

    spr_status_bar[status_bar_right].pushSprite(tft.width() - status_bar_right_length, 0);
}

void invert_sprite(TFT_eSprite *sprite)
{
    for (int a = 0; a < sprite->width(); a++)
    {
        for (int b = 0; b < sprite->height(); b++)
        {
            if (sprite->readPixel(a, b) == TFT_BLACK)
            {
                sprite->drawPixel(a, b, TFT_WHITE);
            }
            else
            {
                sprite->drawPixel(a, b, TFT_BLACK);
            }
        }
    }
}

void home_tft()
{
    for (int i = 0; i < sizeof(home_string) / sizeof(home_string[0]); i++)
    {
        spr_home[i] = new TFT_eSprite(&tft);
        spr_home[i]->setColorDepth(1);
        spr_home[i]->createSprite(tft.width(), 16);

        for (int j = 0; j < home_string[i].length(); j++)
        {
            string home_string_character = home_string[i];
            string string_character = home_string_character.substr(j, 1);
            font_bmp->bmp_shinonome16_get(string_character);
            unsigned char bmp[32];
            array<char, 32> bmp_array;
            bmp_array = font_bmp->character;

            for (int k = 0; k < 32; k++)
            {
                bmp[k] = bmp_array[k];
            }
            draw_font(spr_home[i], bmp, true, j * 8);
        }
        if (i == player_select)
        {
            invert_sprite(spr_home[i]);
        }
        spr_home[i]->pushSprite(0, status_bar_height + 16 * i);
    }
}

void home_up()
{
    if (player_select != 0)
    {
        invert_sprite(spr_home[player_select]);
        spr_home[player_select]->pushSprite(0, status_bar_height + 16 * player_select);
        player_select--;
        invert_sprite(spr_home[player_select]);
        spr_home[player_select]->pushSprite(0, status_bar_height + 16 * player_select);
    }
}

void home_down()
{
    if (player_select + 1 < sizeof(home_string) / sizeof(home_string[0]))
    {
        invert_sprite(spr_home[player_select]);
        spr_home[player_select]->pushSprite(0, status_bar_height + 16 * player_select);
        player_select++;
        invert_sprite(spr_home[player_select]);
        spr_home[player_select]->pushSprite(0, status_bar_height + 16 * player_select);
    }
}

void home_select()
{
    player_mode = player_select;
    home_screen_mode = false;
    for (int i = 0; i < sizeof(home_string) / sizeof(home_string[0]); i++)
    {
        delete spr_home[i];
    }
    player_select = 0;
    if (player_mode == player_explorer) // explorer
    {
        get_directory_list = new getDirectoryList;

        get_directory_list->path = "/";

        get_dir_name_list = true;
        while (get_dir_name_list)
        {
        }

        tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);

        if (get_directory_list->name_list.size() < explorer_row_max)
        {
            if (get_directory_list->name_list.size() == 0) // empty
            {
                explorer_tft(&sprite_explorer, 0, 1);
            }
            else
            {
                explorer_tft(&sprite_explorer, 0, get_directory_list->name_list.size());
            }
        }
        else
        {
            explorer_tft(&sprite_explorer, 0, explorer_row_max - 1);
        }
        invert_sprite(&sprite_explorer[0]);
        for (int i = 0; i < sprite_explorer.size(); i++)
        {
            sprite_explorer[i].pushSprite(0, status_bar_height + 16 * i);
        }
    }
    else if (player_mode == player_system) // system
    {
        system_first_time = true;
        reboot_count = 0x7fffffff;
        get_v_bat_2();
        information_string[4] = version_picopidap_zero;
        information_string[6] = "         Bat = " + v_bat_string;
        information_tft();
        system_bat_time = to_ms_since_boot(get_absolute_time());
    }
    else if (player_mode == usb_dac)
    {
#ifdef pmp_digital_filter
        digital_filter_read = true;
        while(digital_filter_read)
        {
        }
        if (digital_filter < 0)
        {
            digital_filter = 0;
        }
        change_digital_filter();
#endif

        usb_dac_mode = true;
        information_string[3] = "             USB-DAC";
        information_string[4] = "";
        information_string[5] = "";
        information_string[6] = "";
        system_bat_time = to_ms_since_boot(get_absolute_time());
        get_v_bat();
        volume = 100;
        status_bar_right_update();
        status_bar_left_update(false);
        tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
        information_tft();
    }
    else if (player_mode == dac) // system
    {
#ifdef pmp_digital_filter
        digital_filter_read = true;
        while(digital_filter_read)
        {
        }
        if (digital_filter < 0)
        {
            digital_filter = 0;
        }
        information_string[6] = digital_filter_strs[digital_filter];
#else
        information_string[6] = " No digital filter available";
#endif
        information_string[4] = "";
        dac_display_first_time = true;
        information_tft();
    }
}

void information_tft()
{
    for (volatile int i = 0; i < (sizeof(information_string) / sizeof(information_string[0])); i++)
    {
        if (!spr_information[i])
        {
            spr_information[i] = new TFT_eSprite(&tft);
            spr_information[i]->setColorDepth(1);
            spr_information[i]->createSprite(tft.width(), 16);
        }

        spr_information[i]->fillScreen(TFT_BLACK);

        for (int j = 0; j < information_string[i].length(); j++)
        {
            string information_string_character = information_string[i];
            string string_character = information_string_character.substr(j, 1);
            font_bmp->bmp_shinonome16_get(string_character);
            unsigned char bmp[32];
            array<char, 32> bmp_array;
            bmp_array = font_bmp->character;

            for (int k = 0; k < 32; k++)
            {
                bmp[k] = bmp_array[k];
            }
            draw_font(spr_information[i], bmp, true, j * 8);
        }
        spr_information[i]->pushSprite(0, status_bar_height + 16 * i);
    }
}

void explorer_tft(vector<TFT_eSprite> *sprite_explorer_tft, uint16_t start, uint16_t num)
{
    sprite_explorer_tft->resize(0, TFT_eSprite(&tft));
    sprite_explorer_tft->resize(num, TFT_eSprite(&tft));
    for (int i = start; i < start + num; i++)
    {
        uint16_t sprite_width = 0;
        vector<array<char, 32>> bmp_array_vector;
        vector<bool> width_is_16;

        vector<string> string_vector;
        int pos;
        unsigned char lead;
        int char_size;

        for (pos = 0; pos < get_directory_list->name_list[i].size(); pos += char_size)
        {

            lead = get_directory_list->name_list[i][pos];

            if (lead < 0x80)
            {
                char_size = 1;
            }
            else if (lead < 0xE0)
            {
                char_size = 2;
            }
            else if (lead < 0xF0)
            {
                char_size = 3;
            }
            else
            {
                char_size = 4;
            }
            string_vector.push_back(get_directory_list->name_list[i].substr(pos, char_size));
        }

        for (int j = 0; j < string_vector.size(); j++)
        {
            string string_character = string_vector[j];
            font_bmp->bmp_shinonome16_get(string_character);
            bmp_array_vector.push_back(font_bmp->character);
            width_is_16.push_back(font_bmp->width_is_16);
            if (font_bmp->width_is_16)
            {
                sprite_width += 16;
            }
            else
            {
                sprite_width += 8;
            }
        }
        if (sprite_width < tft.width())
        {
            sprite_width = tft.width();
        }

        (*sprite_explorer_tft)[i - start].setColorDepth(1);
        (*sprite_explorer_tft)[i - start].createSprite(sprite_width, 16);
        unsigned char bmp[32];

        uint16_t x_sprite = 0;
        for (int j = 0; j < string_vector.size(); j++)
        {
            for (int k = 0; k < 32; k++)
            {
                bmp[k] = bmp_array_vector[j][k];
            }
            if (width_is_16[j])
            {
                draw_font(&(*sprite_explorer_tft)[i - start], bmp, false, x_sprite);
                x_sprite += 16;
            }
            else
            {
                draw_font(&(*sprite_explorer_tft)[i - start], bmp, true, x_sprite);
                x_sprite += 8;
            }
        }

        if (num == 1)
        {
            font_larger_than_tft[explorer_select] = false;
        }
        else
        {
            font_larger_than_tft[i - start] = false;
        }

        if (x_sprite > tft.width())
        {
            if (num == 1)
            {
                font_larger_than_tft[explorer_select] = true;
            }
            else
            {
                font_larger_than_tft[i - start] = true;
            }
        }
        if (num == 1)
        {
            font_pixel[explorer_select] = x_sprite;
        }
        else
        {
            font_pixel[i - start] = x_sprite;
        }
    }
}

void explorer_up()
{
    rotate_count = 0xffff - 1;
    if (player_select != 0)
    {
        if (player_select == explorer_select)
        {
            invert_sprite(&sprite_explorer[player_select]);
            sprite_explorer[player_select].pushSprite(0, status_bar_height + 16 * player_select);
            player_select--;
            invert_sprite(&sprite_explorer[player_select]);
            sprite_explorer[player_select].pushSprite(0, status_bar_height + 16 * player_select);
            explorer_select--;
        }
        else
        {
            if (explorer_select != 0)
            {
                invert_sprite(&sprite_explorer[explorer_select]);
                sprite_explorer[explorer_select].pushSprite(0, status_bar_height + 16 * explorer_select);
                player_select--;
                explorer_select--;
                invert_sprite(&sprite_explorer[explorer_select]);
                sprite_explorer[explorer_select].pushSprite(0, status_bar_height + 16 * explorer_select);
            }
            else
            {
                invert_sprite(&sprite_explorer[0]);
                for (int i = sprite_explorer.size() - 1; i > 0; i--)
                {
                    sprite_explorer[i].deleteSprite();
                    sprite_explorer[i].createSprite(sprite_explorer[i - 1].width(), sprite_explorer[i - 1].height());
                    sprite_explorer[i].fillScreen(TFT_BLACK);
                    sprite_explorer[i - 1].pushToSprite(&sprite_explorer[i], 0, 0);
                    font_larger_than_tft[i] = font_larger_than_tft[i - 1];
                    font_pixel[i] = font_pixel[i - 1];
                }
                player_select--;
                explorer_select = 0;
                vector<TFT_eSprite> sprite_tmp(1, TFT_eSprite(&tft));
                explorer_tft(&sprite_tmp, player_select, 1);
                sprite_explorer[0].deleteSprite();
                sprite_explorer[0].createSprite(sprite_tmp[0].width(), sprite_tmp[0].height());
                sprite_explorer[0].fillScreen(TFT_BLACK);
                sprite_tmp[0].pushToSprite(&sprite_explorer[0], 0, 0);
                invert_sprite(&sprite_explorer[0]);

                for (int i = 0; i < sprite_explorer.size(); i++)
                {
                    sprite_explorer[i].pushSprite(0, status_bar_height + 16 * i);
                }
            }
        }
    }
}

void explorer_down()
{
    rotate_count = 0xffff - 1;
    if (player_select + 1 < get_directory_list->name_list.size())
    {
        uint16_t explorer_select_pre = explorer_select;
        if ((explorer_select + 1) < sprite_explorer.size())
        {
            explorer_select++;
        }

        if ((player_select + 1) == explorer_select)
        {
            invert_sprite(&sprite_explorer[player_select]);
            sprite_explorer[player_select].pushSprite(0, status_bar_height + 16 * player_select);
            player_select++;
            invert_sprite(&sprite_explorer[player_select]);
            sprite_explorer[player_select].pushSprite(0, status_bar_height + 16 * player_select);
        }
        else
        {
            if (((explorer_select + 1) == sprite_explorer.size()) && ((explorer_select_pre + 1) == sprite_explorer.size()))
            {
                invert_sprite(&sprite_explorer[sprite_explorer.size() - 1]);
                for (int i = 0; i < sprite_explorer.size() - 1; i++)
                {
                    sprite_explorer[i].deleteSprite();
                    sprite_explorer[i].createSprite(sprite_explorer[i + 1].width(), sprite_explorer[i + 1].height());
                    sprite_explorer[i].fillScreen(TFT_BLACK);
                    sprite_explorer[i + 1].pushToSprite(&sprite_explorer[i], 0, 0);
                    font_larger_than_tft[i] = font_larger_than_tft[i + 1];
                    font_pixel[i] = font_pixel[i + 1];
                }
                player_select++;
                explorer_select = sprite_explorer.size() - 1;
                vector<TFT_eSprite> sprite_tmp(1, TFT_eSprite(&tft));
                explorer_tft(&sprite_tmp, player_select, 1);
                sprite_explorer[sprite_explorer.size() - 1].deleteSprite();
                sprite_explorer[sprite_explorer.size() - 1].createSprite(sprite_tmp[0].width(), sprite_tmp[0].height());
                sprite_explorer[sprite_explorer.size() - 1].fillScreen(TFT_BLACK);
                sprite_tmp[0].pushToSprite(&sprite_explorer[sprite_explorer.size() - 1], 0, 0);
                invert_sprite(&sprite_explorer[sprite_explorer.size() - 1]);

                for (int i = 0; i < sprite_explorer.size(); i++)
                {
                    sprite_explorer[i].pushSprite(0, status_bar_height + 16 * i);
                }
            }
            else
            {
                invert_sprite(&sprite_explorer[explorer_select - 1]);
                sprite_explorer[explorer_select - 1].pushSprite(0, status_bar_height + 16 * (explorer_select - 1));

                player_select++;

                invert_sprite(&sprite_explorer[explorer_select]);
                sprite_explorer[explorer_select].pushSprite(0, status_bar_height + 16 * explorer_select);
            }
        }
    }
}

void explorer_in()
{
    rotate_count = 0xffff - 1;
    if (get_directory_list->is_dir[player_select]) // selected is directory
    {
        get_directory_list->path = get_directory_list->path + "/" + get_directory_list->name_list[player_select];
        file_update = true;
        while (file_update)
        {
        }
        player_select = 0;
        explorer_select = 0;

        tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);

        if (get_directory_list->name_list.size() < explorer_row_max)
        {
            if (get_directory_list->name_list.size() == 0)
            {
                explorer_tft(&sprite_explorer, 0, 1);
            }
            else
            {
                explorer_tft(&sprite_explorer, 0, get_directory_list->name_list.size());
            }
        }
        else
        {
            explorer_tft(&sprite_explorer, 0, explorer_row_max - 1);
        }
        invert_sprite(&sprite_explorer[0]);
        for (int i = 0; i < sprite_explorer.size(); i++)
        {
            sprite_explorer[i].pushSprite(0, status_bar_height + 16 * i);
        }
    }
    else // selected is file
    {
        dir_selected = player_select;

        if (music_playing || pause)
        {
            music_playing = 0;
            music_playing_pre = 1;

            let_music_stop = true;

            while (let_music_stop)
            {
            }
            pause = false;
        }

        status_bar_left_update(1);

        music_path = get_directory_list->path;
        musics = get_directory_list->name_list;
        music_select = player_select;
        string file_name = get_directory_list->name_list[dir_selected];
        if (get_directory_list->jpeg_file != "")
        {
            jpeg_file_name = music_path + "/" + get_directory_list->jpeg_file;
        }
        else
        {
            jpeg_file_name = "";
        }
        if (is_audio_file(file_name)) // if selected is music file
        {
            tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
            pico_tag_wait = true;
            while (pico_tag_wait)
            {
            }
            jpeg_file_size_pre = 0;
            album_art_write = true;
            while (album_art_write)
            {
            }
            music_playing = 1;
            music_playing_pre = 0;
            player_screen_mode = true;
            play_start = to_ms_since_boot(get_absolute_time());
            play_update = to_ms_since_boot(get_absolute_time());

            status_bar_left_update(1);
            playerScreen();
        }
    }
}

void exploer_out()
{
    rotate_count = 0xffff - 1;
    if (get_directory_list->path != "/")
    {
        string path_string = "";
        vector<string> path_splitted = split(get_directory_list->path, "/");
        if (path_splitted.size() == 2)
        {
            path_string = "/";
        }
        else
        {
            for (int i = 1; i < (path_splitted.size() - 1); i++)
            {
                path_string += "/" + path_splitted[i];
            }
        }
        get_directory_list->path = path_string;
        file_update = true;
        while (file_update)
        {
        }
        player_select = 0;
        explorer_select = 0;

        for (int i = 0; i < get_directory_list->name_list.size(); i++)
        {
            if (get_directory_list->name_list[i] == path_splitted[path_splitted.size() - 1])
            {
                player_select = i;
                int tmp;
                if (get_directory_list->name_list.size() > (explorer_row_max - 1))
                {
                    //-------------------------------
                    if (i >= 14)
                    {
                        tmp = -13;
                    }
                    else
                    {
                        tmp = -i;
                    }
                }
                else
                {
                    tmp = -i;
                }
                //-------------------------------
                if (tmp < 0)
                {
                    explorer_select = -tmp;
                }
                else
                {
                    explorer_select = 0;
                }
                break;
            }
        }
        if (get_directory_list->name_list.size() < (explorer_row_max - 1))
        {
            player_select = explorer_select;
        }

        tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);

        if (get_directory_list->name_list.size() < explorer_row_max)
        {
            explorer_tft(&sprite_explorer, 0, get_directory_list->name_list.size());
        }
        else
        {
            explorer_tft(&sprite_explorer, player_select - explorer_select, explorer_row_max - 1);
        }
        invert_sprite(&sprite_explorer[explorer_select]);

        for (int i = 0; i < sprite_explorer.size(); i++)
        {
            sprite_explorer[i].pushSprite(0, status_bar_height + 16 * i);
        }
    }
}

void player_screen_to_explorer()
{
    player_screen_mode = false;
    rotate_count = 0xffff - 1;
    player_screen_rotate_num = 1;

    tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
    explorer_tft(&sprite_explorer, 0, 0);
    if (get_directory_list->name_list.size() < explorer_row_max)
    {
        explorer_tft(&sprite_explorer, 0, get_directory_list->name_list.size());
    }
    else
    {
        explorer_tft(&sprite_explorer, player_select - explorer_select, (explorer_row_max - 1));
    }
    invert_sprite(&sprite_explorer[explorer_select]);
    for (int i = 0; i < sprite_explorer.size(); i++)
    {
        sprite_explorer[i].pushSprite(0, status_bar_height + 16 * i);
    }
}

FRESULT scan_files_to_string(
    char *path /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR)
            { /* It is a directory */
                i = strlen(path);
                dirs.push_back(string(&path[i]) + string(fno.fname));
                res = scan_files_to_string(path); /* Enter the directory */
                if (res != FR_OK)
                    break;
                path[i] = 0;
            }
            else
            { /* It is a file. */
                dirs.push_back(string(path) + string(fno.fname));
            }
        }
        f_closedir(&dir);
    }

    return res;
}

FRESULT scan_files(
    char *path /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR)
            { /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path); /* Enter the directory */
                if (res != FR_OK)
                    break;
                path[i] = 0;
            }
        }
        f_closedir(&dir);
    }

    return res;
}

void init_key()
{
    for (int i = 0; i < num_keys; i++)
    {
        gpio_init(buttons_pin[i]);
        gpio_set_dir(buttons_pin[i], GPIO_IN);
        gpio_pull_up(buttons_pin[i]);
        gpio_set_irq_enabled_with_callback(buttons_pin[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    }
}

void rotate_file_name_explorer()
{
    if (font_larger_than_tft[explorer_select])
    {
        if (font_pixel[explorer_select] > (rotate_count + tft.width()))
        {
            if (to_ms_since_boot(get_absolute_time()) - font_larger_than_tft_wait_time > font_larger_than_tft_wait_interval)
            {
                font_larger_than_tft_wait_time = to_ms_since_boot(get_absolute_time());
                rotate_count++;
                font_larger_than_tft_pre = font_larger_than_tft[explorer_select];
                sprite_explorer[explorer_select].pushSprite(-rotate_count, status_bar_height + 16 * explorer_select);
            }
        }
        else
        {
            if (to_ms_since_boot(get_absolute_time()) - font_larger_than_tft_wait_time > font_larger_than_tft_last_wait)
            {
                font_larger_than_tft_wait_time = to_ms_since_boot(get_absolute_time());
                if (rotate_count <= (0xffff - 1))
                {
                    sprite_explorer[explorer_select].pushSprite(0, status_bar_height + 16 * explorer_select);
                    rotate_count = 0xffff;
                }
                else
                {
                    rotate_count = 0;
                }
            }
        }
    }
}

void rotate_music_name_player_screen()
{
    if (font_larger_than_tft[player_screen_rotate_num])
    {
        if (font_pixel[player_screen_rotate_num] > (rotate_count + tft.width()))
        {
            if ((to_ms_since_boot(get_absolute_time()) - font_larger_than_tft_wait_time) > font_larger_than_tft_wait_interval)
            {
                font_larger_than_tft_wait_time = to_ms_since_boot(get_absolute_time());
                rotate_count++;
                font_larger_than_tft_pre = font_larger_than_tft[player_screen_rotate_num];
                spr_player[player_screen_rotate_num]->pushSprite(-rotate_count, album_art_height + status_bar_height + 16 * player_screen_rotate_num);
            }
        }
        else
        {
            if ((to_ms_since_boot(get_absolute_time()) - font_larger_than_tft_wait_time) > font_larger_than_tft_last_wait)
            {
                font_larger_than_tft_wait_time = to_ms_since_boot(get_absolute_time());
                if (rotate_count <= (0xffff - 1))
                {
                    spr_player[player_screen_rotate_num]->pushSprite(0, album_art_height + status_bar_height + 16 * player_screen_rotate_num);
                    rotate_count = 0xffff;
                }
                else
                {
                    rotate_count = 0;
                    player_screen_rotate_num++;
                    if (player_screen_rotate_num > 3)
                    {
                        player_screen_rotate_num = 1;
                    }
                }
            }
        }
    }
    else
    {
        player_screen_rotate_num++;
        if (player_screen_rotate_num > 3)
        {
            player_screen_rotate_num = 1;
        }
    }
}

void core1()
{
    // sleep_ms(1000);

    // ADC for battery voltage sensing
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);

    // tft
    tft.init();
    font_bmp = new bmp_shinonome16();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // display brightess
    pwm_set_gpio_level(TFT_BL, 100);

    // tft.setRotation(0);

    for (int i = 0; i < sizeof(spr_player) / sizeof(spr_player[0]); i++)
    {
        spr_player[i] = new TFT_eSprite(&tft);
        spr_player[i]->setColorDepth(1);
    }

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    for (int i = 0; i < sizeof(spr_status_bar) / sizeof(*spr_status_bar); i++)
    {
        spr_status_bar[i].setColorDepth(1);
    }
    spr_status_bar[0].createSprite(status_bar_left_length, status_bar_height);
    spr_status_bar[1].createSprite(status_bar_right_length, status_bar_height);

    status_bar_left_update(0);
    get_v_bat();
    status_bar_right_update();

    home_tft();

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.setCursor(0, 0);
    tft.setTextSize(1);

    init_key();

    if (digital_filter >= 0)
    {
        change_digital_filter();
    }

    if ((!gpio_get(buttons_pin[usb_dac_button1])) || (!gpio_get(buttons_pin[usb_dac_button2])))
    {
#ifdef pmp_digital_filter
        digital_filter_read = true;
        while(digital_filter_read)
        {
        }
        if (digital_filter < 0)
        {
            digital_filter = 0;
        }
        change_digital_filter();
#endif

        home_screen_mode = false;
        player_mode = usb_dac;
        usb_dac_mode = true;
        information_string[3] = "             USB-DAC";
        information_string[4] = "";
        information_string[5] = "";
        information_string[6] = "";
        system_bat_time = to_ms_since_boot(get_absolute_time());
        get_v_bat();
        volume = 100;
        status_bar_right_update();
        status_bar_left_update(false);
        tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
        information_tft();
    }
    else if (file_exist)
    {
        sleep_ms(10); // wait a bit to avoid uncontrollable when usb is connected
        tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
        home_screen_mode = false;
        pico_tag_wait = true;
        while (pico_tag_wait)
        {
        }
        if (get_directory_list->jpeg_file != "")
        {
            jpeg_file_name = music_path + "/" + get_directory_list->jpeg_file;
        }
        else
        {
            jpeg_file_name = "";
        }
        album_art_write = true;
        while (album_art_write)
        {
        }

        music_playing = 1;
        music_playing_pre = 0;
        pause = true;
        pause_time = to_ms_since_boot(get_absolute_time());
        player_screen_mode = true;
        play_start = to_ms_since_boot(get_absolute_time());
        play_update = to_ms_since_boot(get_absolute_time());
        playerScreen();
    }

    while (1)
    {
        if (home_screen_mode)
        {
            uint8_t key = 100;
            if (key_interrupt)
            {
                key = get_key();
                key_interrupt = false;
            }

            if (key < 11)
            {
                switch (key)
                {
                case 1:

                    break;
                case 2:
                    // up
                    home_up();
                    break;
                case 3:

                    break;
                case 4:
                    break;
                case 5:
                    // select
                    home_select();
                    break;
                case 6:

                    break;
                case 7:

                    break;
                case 8:
                    // down
                    home_down();
                    break;
                case 9:

                case 10:

                    break;

                default:
                    break;
                }
            }
        }
        else if (player_mode == player_explorer)
        {
            if (key_interrupt)
            {
                key_num_pre = key_num;
                key_num = get_key();
                key_interrupt = false;
                if (key_num < 11)
                {
                    time_hold = to_ms_since_boot(get_absolute_time());
                    time_hold_multi = time_hold + time_long + time_long_interval;
                    key_long = false;
                    key_count = 0;
                }
                else
                {
                    key_long = false;
                    key_count = 100;
                }
            }

            if (key_num < 11)
            {
                key_long_pre = key_long;
                if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                {
                    key_long = true;
                }
            }

            /*
            key1 : display on/off
            key2 : go upper (long press supported) / volume up (long press supported)
            key3 : change display brightness
            key4 : previous music file / (long press) seek backword (long press supported)
            key5 : select / in folder / pause music / play music
            key6 : next music file / (long) seek forward (long press supported)
            key7 : back
            key8 : go down (long press supported) / volume down (long press supported)
            key9 : show player screen / (long press) go home screen
            key10 : change playing mode
            */

            if ((key_num == 1) && (key_count == 0)) // display on / off
            {
                display_on = !display_on;
                if (display_on)
                {
                    pwm_set_gpio_level(TFT_BL, brightness[brightness_select]);
                    if (player_screen_mode)
                    {
                        playing_time_update();
                    }
                }
                else
                {
                    pwm_set_gpio_level(TFT_BL, 0);
                }
            }
            else if (key_num == 2)
            {
                if (player_screen_mode) // volume up
                {
                    if (key_count == 0)
                    {
                        volume++;
                        if (volume > 100)
                        {
                            volume = 100;
                        }
#ifdef NO_SOFT_VOL
                        change_volume(volume);
#endif
                        volume_write = 1;
                        status_bar_right_update();
                    }
                    else
                    {
                        if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                        {
                            if (time_hold_multi < to_ms_since_boot(get_absolute_time()))
                            {
                                volume++;
                                if (volume > 100)
                                {
                                    volume = 100;
                                }
#ifdef NO_SOFT_VOL
                                change_volume(volume);
#endif
                                volume_write = 1;
                                status_bar_right_update();
                                time_hold_multi += time_long_interval;
                            }
                        }
                    }
                }
                else // go upper
                {
                    if (key_count == 0)
                    {
                        explorer_up();
                    }
                    else
                    {
                        if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                        {
                            if (time_hold_multi < to_ms_since_boot(get_absolute_time()))
                            {
                                explorer_up();
                                time_hold_multi += time_long_interval;
                            }
                        }
                    }
                }
            }
            else if ((key_num == 3) && (key_count == 0)) // change brightness
            {
                brightness_select++;
                if (brightness_select >= (sizeof(brightness) / sizeof(brightness[0])))
                {
                    brightness_select = 0;
                }
                pwm_set_gpio_level(TFT_BL, brightness[brightness_select]);
            }
            else if ((key_num == 4) && key_long) // seek time
            {
                if (player_screen_mode)
                {
                    if (!seeking)
                    {
                        seeking = true;
                    }
                    if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                    {
                        if (time_hold_multi < to_ms_since_boot(get_absolute_time()))
                        {
                            play_start += seek_step;
                            if (pause)
                            {
                                if ((int)pause_time < play_start)
                                {
                                    play_start = pause_time;
                                }
                            }
                            else
                            {
                                if (play_start > (int)to_ms_since_boot(get_absolute_time()))
                                {
                                    play_start = to_ms_since_boot(get_absolute_time());
                                }
                            }
                            playing_time_update();
                            time_hold_multi += time_long_interval;
                        }
                    }
                }
            }
            else if ((key_num_pre == 4) && (key_num >= (num_keys + 1)) && key_long_pre) // seek
            {
                key_num_pre = key_num;
                seek = true;
                while (seek)
                {
                }
                seeking = false;
            }

            else if ((key_num_pre == 4) && (key_num >= (num_keys + 1)))
            {
                key_num_pre = key_num;
                if (player_screen_mode) // previous music file
                {
                    music_playing = 0;
                    music_playing_pre = 1;

                    let_music_stop = true;

                    while (let_music_stop)
                    {
                    }
                    pause = false;

                    if ((music_select - 1) < 0)
                    {
                        music_select = musics.size() - 1;
                    }
                    else
                    {
                        music_select--;
                    }

                    string file_name = musics[music_select];
                    if (is_audio_file(file_name))
                    {
                        pico_tag_wait = true;
                        while (pico_tag_wait)
                        {
                        }
                        music_playing = 1;
                        music_playing_pre = 0;
                        player_screen_mode = true;
                        play_start = to_ms_since_boot(get_absolute_time());
                        play_update = to_ms_since_boot(get_absolute_time());
                        rotate_count = 0xffff - 1;
                        player_screen_rotate_num = 1;
                        status_bar_left_update(1);
                        if (player_screen_mode)
                        {
                            album_art_write = true;
                            while (album_art_write)
                            {
                            }

                            playerScreen();
                        }
                    }
                }
            }
            else if ((key_num == 4) && (key_count == 0)) // back
            {
                if (!player_screen_mode)
                {
                    exploer_out();
                }
            }
            else if ((key_num == 5) && !key_long_pre && key_long) //
            {
                if (player_screen_mode) // go back
                {
                    player_screen_to_explorer();
                }
            }
            else if ((key_num_pre == 5) && (key_num >= (num_keys + 1)) && key_long_pre)
            {
                key_num_pre = (num_keys + 1);
                key_long = false;
            }
            else if ((key_num_pre == 5) && (key_num >= (num_keys + 1)))
            {
                key_num_pre = key_num;
                if (player_screen_mode)
                {
                    if (music_playing)
                    {
                        if (pause) // play
                        {
                            get_music_file_data();
                            pause = false;
                            play_start = to_ms_since_boot(get_absolute_time()) - (pause_time - play_start);
                            status_bar_left_update(1);
                        }
                        else // pause
                        {
                            pause = true;
                            pause_time = to_ms_since_boot(get_absolute_time());
                            status_bar_left_update(0);
                        }
                    }
                    else // play
                    {
                        music_playing = 1;
                        play_start = to_ms_since_boot(get_absolute_time());
                        status_bar_left_update(1);
                    }
                }
                else // select / in folder
                {
                    explorer_in();
                }
            }
            else if ((key_num == 6) && key_long) // seek time
            {
                if (player_screen_mode)
                {
                    if (!seeking)
                    {
                        seeking = true;
                    }
                    if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                    {
                        if (time_hold_multi < to_ms_since_boot(get_absolute_time()))
                        {
                            play_start -= seek_step;
                            uint32_t duration4seek = 300; // set 300 for unknown duration music file
                            if (duration != 0)
                            {
                                duration4seek = duration;
                            }
                            int playing_time_now;
                            if (pause)
                            {
                                playing_time_now = (int)((pause_time - play_start));
                            }
                            else
                            {
                                playing_time_now = (int)((to_ms_since_boot(get_absolute_time()) - play_start));
                            }
                            if (playing_time_now > duration4seek * 1000)
                            {
                                play_start += playing_time_now - duration4seek * 1000;
                            }
                            playing_time_update();
                            time_hold_multi += time_long_interval;
                        }
                    }
                }
            }
            else if ((key_num_pre == 6) && (key_num >= (num_keys + 1)) && key_long_pre) // seek
            {
                key_num_pre = key_num;
                seek = true;
                while (seek)
                {
                }
                seeking = false;
            }
            else if ((key_num_pre == 6) && (key_num >= (num_keys + 1)))
            {
                if (player_screen_mode) // next music file
                {
                    key_num_pre = key_num;

                    if (playing_mode == playing_mode_repeat_all)
                    {
                        repeat_next =true;
                        music_playing = 0;
                        music_playing_pre = 1;

                        let_music_stop = true;

                        while (let_music_stop)
                        {
                        }
                        pause = false;

                        music_playing = 1;
                        music_playing_pre = 0;

                        while(music_playing_pre == 0)
                        {
                        }

                        album_art_write = true;
                        
                        while(repeat_next)
                        {
                        }

                        while(album_art_write)
                        {
                        }
                        playerScreen();

                        player_screen_mode = true;
                        play_start = to_ms_since_boot(get_absolute_time());
                        play_update = to_ms_since_boot(get_absolute_time());

                        rotate_count = 0xffff - 1;
                        player_screen_rotate_num = 1;
                        status_bar_left_update(1);
                    }
                    else
                    {
                        music_playing = 0;
                        music_playing_pre = 1;

                        let_music_stop = true;

                        while (let_music_stop)
                        {
                        }
                        pause = false;
                        if ((music_select) < (musics.size() - 1))
                        {
                            music_select++;
                        }
                        else
                        {
                            music_select = 0;
                        }
                        string file_name = musics[music_select];
                        if (is_audio_file(file_name))
                        {
                            pico_tag_wait = true;
                            while (pico_tag_wait)
                            {
                            }
                            music_playing = 1;
                            music_playing_pre = 0;
                            player_screen_mode = true;
                            play_start = to_ms_since_boot(get_absolute_time());
                            play_update = to_ms_since_boot(get_absolute_time());

                            rotate_count = 0xffff - 1;
                            player_screen_rotate_num = 1;
                            status_bar_left_update(1);
                            if (player_screen_mode)
                            {
                                album_art_write = true;
                                while (album_art_write)
                                {
                                }

                                playerScreen();
                            }
                        }
                    }
                }
            }
            else if ((key_num == 7) && (key_count == 0)) // back
            {
                if (player_screen_mode)
                {
                    player_screen_to_explorer();
                }
                else
                {
                    exploer_out();
                }
            }
            else if (key_num == 8)
            {
                if (player_screen_mode) // volume down
                {
                    if (key_count == 0)
                    {
                        if (volume != 0)
                        {
                            volume--;
#ifdef NO_SOFT_VOL
                            change_volume(volume);
#endif
                            volume_write = 1;
                            status_bar_right_update();
                        }
                    }
                    else
                    {
                        if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                        {
                            if (time_hold_multi < to_ms_since_boot(get_absolute_time()))
                            {
                                if (volume != 0)
                                {
                                    volume--;
#ifdef NO_SOFT_VOL
                                    change_volume(volume);
#endif
                                    volume_write = 1;
                                    status_bar_right_update();
                                }
                                time_hold_multi += time_long_interval;
                            }
                        }
                    }
                }
                else // go down
                {
                    if (key_count == 0)
                    {
                        explorer_down();
                    }
                    else
                    {
                        if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                        {
                            if (time_hold_multi < to_ms_since_boot(get_absolute_time()))
                            {
                                explorer_down();
                                time_hold_multi += time_long_interval;
                            }
                        }
                    }
                }
            }
            else if ((key_num == 9) && !key_long_pre && key_long) // go home screen
            {
                key_long = false;
                home_screen_mode = true;
                key_num = 100;
                key_num_pre = 0;
                player_mode = player_explorer;
                player_select = 0;
                explorer_select = 0;
                key_count = 100;
                player_screen_mode = false;
                music_playing = 0;
                music_playing_pre = 0;
                delete get_directory_list;

                tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
                home_tft();
            }
            else if ((key_num_pre == 9) && (key_num >= (num_keys + 1))) // show player screen
            {
                key_num_pre = key_num;
                if (!player_screen_mode)
                {
                    player_screen_mode = true;
                    rotate_count = 0xffff - 1;
                    player_screen_rotate_num = 1;
                    tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
                    playerScreen();
                    jpeg_file_size_pre = 0;
                }
                else
                {
                    playerScreen();
                }
            }
            else if ((key_num == 10) && (key_count == 0)) // change playing mode
            {
                playing_mode++;
                playing_mode %= playing_mode_num;
                if (music_playing)
                {
                    status_bar_left_update(true);
                }
                else
                {
                    status_bar_left_update(false);
                }
            }

            if (key_num < 11)
            {
                key_count++;
            }

            if (player_screen_mode) // update battery voltage and playing time
            {
                if (music_playing)
                {
                    if ((to_ms_since_boot(get_absolute_time()) - play_update > 1000))
                    {
                        play_update = to_ms_since_boot(get_absolute_time());
                        if (!pause)
                        {
                            get_v_bat();
                            if (!seeking)
                            {
                                playing_time_update();
                            }
                        }
                    }
                }
            }

            if (play_time_update)
            {
                get_v_bat();
                if (!seeking)
                {
                    playing_time_update();
                }
                play_time_update = false;
            }

            if (display_on)
            {
                if (!player_screen_mode) // rotate file name (explorer)
                {
                    rotate_file_name_explorer();
                }

                if (player_screen_mode) // rotate music name (player screen)
                {
                    rotate_music_name_player_screen();
                }
            }
        }
        else if (player_mode == player_system) // system
        {
            uint8_t key = 100;
            if (key_interrupt)
            {
                key = get_key();
                key_interrupt = false;
            }

            if ((key == 3) || (key == 2))
            {
                reboot_count = to_ms_since_boot(get_absolute_time());
            }
            else if (key == 11)
            {
                if (system_first_time)
                {
                    system_first_time = false;
                }
                else
                {
                    home_screen_mode = true;
                    tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
                    home_tft();
                }
            }
            else
            {
                if ((int)((int)to_ms_since_boot(get_absolute_time()) - (int)reboot_count) > 1000) // flash mode
                {
                    for (int i = 0; i < sizeof(information_string) / sizeof(information_string[0]); i++)
                    {
                        information_string[i] = "";
                    }
                    information_string[3] = "           Flash mode";
                    information_tft();
                    sleep_ms(1000);
                    reset_usb_boot(0, 0);
                }

                if ((to_ms_since_boot(get_absolute_time()) - system_bat_time) > 1000) // update voltage
                {
                    system_bat_time = to_ms_since_boot(get_absolute_time());
                    get_v_bat();
                    status_bar_right_update();
                    get_v_bat_2();
                    information_string[6] = "         Bat = " + v_bat_string;
                    information_tft();
                }
            }
        }
        else if (usb_dac_mode)
        {
            static int uac_bit_pre = 0;
            static int uac_freq_pre = 0;
            if ((uac_bit_pre != bit_uac2) || (uac_freq_pre != freq_uac2))
            {
                uac_bit_pre = bit_uac2;
                uac_freq_pre = freq_uac2;
                if ((bit_uac2 < 0) || (freq_uac2 < 0))
                {
                    information_string[5] = "        No Enough Memory";
                    information_tft();
                    status_bar_left_update(false);                    
                }
                else if ((bit_uac2 != 0) || (freq_uac2 != 0))
                {
                    information_string[5] = "        " + to_string(bit_uac2) + "bit / " + to_string(freq_uac2) + "Hz";
                    information_tft();
                    status_bar_left_update(true);
                }else
                {
                    information_string[5] = "";
                    information_tft();
                    status_bar_left_update(false);
                }
            }

            if ((to_ms_since_boot(get_absolute_time()) - system_bat_time) > 100000) // update voltage
            {
                system_bat_time = to_ms_since_boot(get_absolute_time());
                if (display_on)
                {
                    get_v_bat();
                    status_bar_right_update();
                }
            }

            if (key_interrupt)
            {
                key_num_pre = key_num;
                key_num = get_key();
                key_interrupt = false;
                if (key_num < 11)
                {
                    time_hold = to_ms_since_boot(get_absolute_time());
                    time_hold_multi = time_hold + time_long + time_long_interval;
                    key_long = false;
                    key_count = 0;
                }
                else
                {
                    key_long = false;
                    key_count = 100;
                }
            }

            if (key_num < 11)
            {
                key_long_pre = key_long;
                if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                {
                    key_long = true;
                }
            }

            if ((key_num == 1) && (key_count == 0)) // display on / off
            {
                display_on = !display_on;
                if (display_on)
                {
                    pwm_set_gpio_level(TFT_BL, brightness[brightness_select]);
                    if (player_screen_mode)
                    {
                        playing_time_update();
                    }
                }
                else
                {
                    pwm_set_gpio_level(TFT_BL, 0);
                }
            }

            if (key_num < 11)
            {
                key_count++;
            }
        }
        else if (player_mode == dac) // dac setting
        {
            if ((to_ms_since_boot(get_absolute_time()) - system_bat_time) > 1000) // update voltage
            {
                system_bat_time = to_ms_since_boot(get_absolute_time());
                get_v_bat();
                status_bar_right_update();
            }

            if (key_interrupt)
            {
                key_num_pre = key_num;
                key_num = get_key();
                key_interrupt = false;
                if (key_num < 11)
                {
                    time_hold = to_ms_since_boot(get_absolute_time());
                    time_hold_multi = time_hold + time_long + time_long_interval;
                    key_long = false;
                    key_count = 0;
                }
                else
                {
                    key_long = false;
                    key_count = 100;
                }
            }

            if (key_num < 11)
            {
                key_long_pre = key_long;
                if ((to_ms_since_boot(get_absolute_time()) - time_hold) > time_long)
                {
                    key_long = true;
                }
            }
#ifdef pmp_digital_filter
            if ((key_num == 6) && (key_count == 0)) // next dac digital filter select
            {
                digital_filter++;
                digital_filter %= digital_filter_num;
                information_string[6] = digital_filter_strs[digital_filter];
                information_tft();
            }
            else if ((key_num == 4) && (key_count == 0)) // prev dac digital filter select
            {
                digital_filter--;
                if (digital_filter < 0)
                {
                    digital_filter = digital_filter_num - 1;
                }
                information_string[6] = digital_filter_strs[digital_filter];
                information_tft();
            }
            else if (key_num == 11)
            {
            }
#else
            if (key_num == 11)
            {
            }
#endif
            else if (key_num == 100)
            {
            }
            else if (key_count == 0)
            {
                digital_filter_write = true;
                while(digital_filter_write)
                {
                    sleep_ms(1);
                }
                change_digital_filter();
                home_screen_mode = true;
                tft.fillRect(0, status_bar_height, tft.width(), tft.height() - status_bar_height, TFT_BLACK);
                home_tft();
            }

            if (key_num < 11)
            {
                key_count++;
            }

        }

        if (player_screen_mode && player_screen_update)
        {
            album_art_write = true;
            while (album_art_write)
            {
            }

            player_screen_update = false;
            playerScreen();
        }
    }
}

void draw_album_art()
{
    volatile uint32_t jpeg_file_size = 0;

    if (!TJpgDec)
    {
        TJpgDec = new TJpg_Decoder();
    }
    TJpgDec->setJpgScale(8);
    TJpgDec->setCallback(tft_output);

    if (jpeg_file_name != "")
    {
        uint16_t w = 0, h = 0;

        FIL file_jpeg;
        fr = f_open(&file_jpeg, jpeg_file_name.c_str(), FA_READ | FA_OPEN_EXISTING);
        if (fr == FR_OK)
        {
            jpeg_file_size = f_size(&file_jpeg);
            f_close(&file_jpeg);
        }

        if (jpeg_file_size != jpeg_file_size_pre)
        {
            TJpgDec->getSdJpgSize(&w, &h, jpeg_file_name.c_str());

            w_jpeg = w;
            h_jpeg = h;

            if (w != 0)
            {
                album_art(jpeg_file_name, false);
            }
        }
    }
    else
    {
        uint16_t w = 0, h = 0;
        string music_filename = music_path + "/" + musics[music_select];
        uint32_t art_offset = get_cover_offset(music_filename, pico_tag1->cover_art_offset);
        jpeg_file_size = pico_tag1->cover_art_size - (art_offset - pico_tag1->cover_art_offset);

        if (jpeg_file_size != jpeg_file_size_pre)
        {
            TJpgDec->getSdAudioJpgSize(&w, &h, music_filename.c_str(), art_offset, jpeg_file_size);
            album_art(music_filename, true, art_offset, jpeg_file_size);
        }
    }

    if (TJpgDec)
    {
        delete TJpgDec;
        TJpgDec = NULL;
    }

    jpeg_file_size_pre = jpeg_file_size;
}