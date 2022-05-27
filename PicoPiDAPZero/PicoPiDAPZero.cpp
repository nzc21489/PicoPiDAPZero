/*
  Copyright (C) 2022  nzc21489

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma GCC optimize("O2")

#include "PicoPiDAPZero.h"

#include "AudioFileSourceSD.h"
#include "AudioGeneratorFLAC.h"
// #include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"
#include "AudioGeneratorAAC.h"
#include "AudioGeneratorOpus.h"
#include "AudioOutputI2S.h"

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

volatile file_type audio_type_pre = file_start;

AudioFileSourceSD *source = NULL;
AudioGenerator *decoder = NULL;

AudioOutputI2S *out = NULL;

uint8_t bit_pre = 0;
uint32_t sampling_rate_pre = 0;

volatile bool wav_playing = true;

#ifndef NO_SOFT_VOL
#include <math.h>
void change_volume(uint8_t vol)
{
    if (out)
    {
        if (vol == 0)
        {
            out->SetGain(0.f);
        }
        else
        {
            out->SetGain(powf(10.0, (((float)vol - 100.f) / 20.f)));
        }
    }
    if (vol == 0)
    {
        wav_set_gain(0.f);
    }
    else
    {
        wav_set_gain((powf(10.0, (((float)vol - 100.f) / 20.f))));
    }
}
#endif

bool init_sd()
{
    FRESULT fr;
    fr = f_opendir(&dir, (pmp_path.c_str()));
    if (fr == FR_OK)
    {
        fr = f_open(&fil, (pmp_path + "/" + pmp_file).c_str(), FA_READ | FA_OPEN_EXISTING);
        if (fr == FR_OK)
        {
            char line[300];
            f_gets(line, sizeof(line), &fil);
            string m_path = line;
            f_close(&fil);

            fr = f_open(&fil, (pmp_path + "/" + pmp_file2).c_str(), FA_READ | FA_OPEN_EXISTING);
            if (fr == FR_OK)
            {
                f_gets(line, sizeof(line), &fil);
                string m_sel = line;
                f_close(&fil);

                if ((m_path.size() > 0) && (m_sel.size() > 0))
                {
                    file_exist = true;
                    music_path = m_path;
                    music_select = stoi(m_sel);
                    get_directory_list = new getDirectoryList;

                    get_directory_list->path = music_path;
                    get_directory_list->update();
                    musics = get_directory_list->name_list;
                }
            }
        }

        fr = f_open(&fil, (pmp_path + "/" + pmp_vol).c_str(), FA_READ | FA_OPEN_EXISTING);
        if (fr == FR_OK)
        {
            char line[100];
            f_gets(line, sizeof(line), &fil);
            string vol = line;
            if (vol.size() > 0)
            {
                volume = stoi(vol);
            }
            f_close(&fil);
        }

#ifdef pmp_digital_filter
        fr = f_open(&fil, (pmp_path + "/" + pmp_digital_filter).c_str(), FA_READ | FA_OPEN_EXISTING);
        if (fr == FR_OK)
        {
            char line[100];
            f_gets(line, sizeof(line), &fil);
            string d_f = line;
            if (d_f.size() > 0)
            {
                digital_filter = stoi(d_f);
            }
            f_close(&fil);
        }
#endif

        f_closedir(&dir);
    }
    else
    {
        FRESULT res;
        res = f_mkdir(pmp_path.c_str());
        if (res != FR_OK)
        {
            return false;
        }
    }
    return true;
}

void music_decoder_start()
{
    printf("music decoder start\n");
    music_playing = false;

    if (!file_exist)
    {
        char buff;
        UINT bw;
        fr = f_open(&fil, (pmp_path + "/" + pmp_file).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
        if (fr == FR_OK)
        {
            sleep_ms(10);
            f_write(&fil, music_path.c_str(), strlen(music_path.c_str()), &bw);
            f_close(&fil);
        }

        // write again
        // work around for not writing file well
        if (bw < strlen(music_path.c_str()))
        {
            fr = f_open(&fil, (pmp_path + "/" + pmp_file).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
            if (fr == FR_OK)
            {
                sleep_ms(100);
                f_write(&fil, music_path.c_str(), strlen(music_path.c_str()), &bw);
                f_close(&fil);
            }
        }

        fr = f_open(&fil, (pmp_path + "/" + pmp_file2).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
        if (fr == FR_OK)
        {
            sleep_ms(10);
            f_write(&fil, (to_string(music_select)).c_str(), strlen(to_string(music_select).c_str()), &bw);
            f_close(&fil);
        }

        // write again
        if (bw < strlen(to_string(music_select).c_str()))
        {
            fr = f_open(&fil, (pmp_path + "/" + pmp_file2).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
            if (fr == FR_OK)
            {
                sleep_ms(100);
                f_write(&fil, (to_string(music_select)).c_str(), strlen(to_string(music_select).c_str()), &bw);
                f_close(&fil);
            }
        }
    }
    else
    {
        file_exist = false;
    }

    if (volume_write)
    {
        fr = f_open(&fil, (pmp_path + "/" + pmp_vol).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
        if (fr == FR_OK)
        {
            char buff;
            UINT bw;
            f_write(&fil, (to_string(volume)).c_str(), strlen((to_string(volume)).c_str()), &bw);
            f_close(&fil);
        }
        volume_write = false;
    }

    string file_name = musics[music_select];

    audio_start_seek = 0;

    uint16_t buffer_count = (4096 * 4);

    string flac = ".flac";
    string opus = ".opus";
    string wav = ".wav";
    string mp3 = ".mp3";
    string aac = ".aac";

    file_type audio_type;

    if (file_name.size() >= flac.size() &&
        file_name.find(flac, file_name.size() - flac.size()) != std::string::npos)
    {
        if (audio_type_pre != flac_file)
        {
            if (decoder)
            {
                delete decoder;
                decoder = NULL;
            }
            decoder = new AudioGeneratorFLAC();
        }
        audio_type = flac_file;
    }
    else if (file_name.size() >= opus.size() &&
             file_name.find(opus, file_name.size() - opus.size()) != std::string::npos)
    {
        if (audio_type_pre != opus_file)
        {
            if (decoder)
            {
                delete decoder;
                decoder = NULL;
            }

            if (out)
            {
                delete out;
            }

            decoder = new AudioGeneratorOpus();
            buffer_count = (1024 * 14);
            out = new AudioOutputI2S(buffer_count);
        }
        audio_type = opus_file;
    }
    else if (file_name.size() >= wav.size() &&
             file_name.find(wav, file_name.size() - wav.size()) != std::string::npos)
    {
        if (audio_type_pre != wav_file)
        {
            if (decoder)
            {
                delete decoder;
                decoder = NULL;
            }
            // decoder = new AudioGeneratorWAV();

            if (out)
            {
                delete out;
                out = NULL;
            }
            wav_start(pico_tag1->bits_per_sample);
        }

        audio_start_seek = pico_tag1->audio_start;
        audio_type = wav_file;
    }
    else if (file_name.size() >= mp3.size() &&
             file_name.find(mp3, file_name.size() - mp3.size()) != std::string::npos)
    {
        if (audio_type_pre != mp3_file)
        {
            if (decoder)
            {
                delete decoder;
                decoder = NULL;
            }
            decoder = new AudioGeneratorMP3();
        }
        audio_type = mp3_file;
        audio_start_seek = pico_tag1->audio_start - pico_tag1->audio_start % pico_tag1->frame_size;
    }
    else if (file_name.size() >= aac.size() &&
             file_name.find(aac, file_name.size() - aac.size()) != std::string::npos)
    {
        if (audio_type_pre != aac_file)
        {
            if (decoder)
            {
                delete decoder;
                decoder = NULL;
            }
            decoder = new AudioGeneratorAAC();
        }
        audio_type = aac_file;
    }

    if (audio_type != wav_file)
    {
        if (audio_type_pre == wav_file)
        {
            wav_end();
            out = new AudioOutputI2S(buffer_count);
            out->SetRate(pico_tag1->sampling_rate);
            out->SetBitsPerSample(pico_tag1->bits_per_sample);
            out->SetGain(1.0);
            out->begin(true);
        }

        if ((pico_tag1->bits_per_sample != bit_pre) || (pico_tag1->sampling_rate != sampling_rate_pre))
        {
            if (out)
            {
                delete out;
                out = NULL;
            }

#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
            cs4398_set_FM(pico_tag1->sampling_rate);
#endif

#ifdef DAC_DacPlusPro
            DacPlusPro_change_bit_freq(32, pico_tag1->sampling_rate);
#else
            si5351_set_clock(32, pico_tag1->sampling_rate);
#endif

            sleep_ms(50);
            out = new AudioOutputI2S(buffer_count);
            out->SetRate(pico_tag1->sampling_rate);
            out->SetBitsPerSample(pico_tag1->bits_per_sample);
            // out->SetGain(1.0);
            change_volume(volume);
            out->begin(true);
        }

        bit_pre = pico_tag1->bits_per_sample;
        sampling_rate_pre = pico_tag1->sampling_rate;
        audio_type_pre = audio_type;

        string music_file_path = music_path + "/" + musics[music_select];
        printf("file path = %s\n", music_file_path.c_str());
        if (source->open(music_file_path.c_str()))
        {
            decoder->begin(source, out);
            if (audio_start_seek > 0)
            {
                source->seek(audio_start_seek, SEEK_SET);
            }
        }
        else
        {
            printf("failed to open music file\n");
        }
    }
    else
    {
        if ((pico_tag1->bits_per_sample != bit_pre) || (pico_tag1->sampling_rate != sampling_rate_pre))
        {
            if (out)
            {
                delete out;
                out = NULL;
            }

#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
            cs4398_set_FM(pico_tag1->sampling_rate);
#endif

#ifdef DAC_DacPlusPro
            DacPlusPro_change_bit_freq(32, pico_tag1->sampling_rate);
#else
            si5351_set_clock(32, pico_tag1->sampling_rate);
#endif

            sleep_ms(50);

            if (audio_type_pre == wav_file)
            {
                wav_end();
                wav_start(pico_tag1->bits_per_sample);
            }
        }

        bit_pre = pico_tag1->bits_per_sample;
        sampling_rate_pre = pico_tag1->sampling_rate;
        audio_type_pre = wav_file;

        string music_file_path = music_path + "/" + musics[music_select];
        printf("file path = %s\n", music_file_path.c_str());

        wav_open(music_file_path);
        wav_seek(audio_start_seek);
        wav_set_end_byte(pico_tag1->audio_end);
        wav_playing = true;
    }

    play_start = to_ms_since_boot(get_absolute_time());
    music_playing = true;
}

void seek_audio()
{
    uint32_t seek_point = 0;
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

    if (playing_time_now == 0)
    {
        if (audio_type_pre == wav_file)
        {
            wav_seek(audio_start_seek);
        }
        else
        {
            source->seek(audio_start_seek, SEEK_SET);
        }
    }
    else
    {
        seek_point = (uint32_t)(((uint64_t)pico_tag1->audio_end - (uint64_t)pico_tag1->audio_start) * (uint64_t)playing_time_now / (uint64_t)(duration4seek * 1000));
        if (pico_tag1->bits_per_sample == 24)
        {
            seek_point += (6 - (seek_point - pico_tag1->audio_start) % (pico_tag1->channel * pico_tag1->bits_per_sample / 8)); // wav
        }
        else
        {
            seek_point += (8 * (pico_tag1->bits_per_sample / 32) - (seek_point - pico_tag1->audio_start) % (pico_tag1->channel * pico_tag1->bits_per_sample / 8)); // wav
        }

        if (audio_type_pre == wav_file)
        {
            wav_seek(seek_point);
        }
        else
        {
            source->seek(seek_point, SEEK_SET);
        }
    }
}

void read_digital_filter()
{
#ifdef pmp_digital_filter
        fr = f_open(&fil, (pmp_path + "/" + pmp_digital_filter).c_str(), FA_READ | FA_OPEN_EXISTING);
        if (fr == FR_OK)
        {
            char line[100];
            f_gets(line, sizeof(line), &fil);
            string d_f = line;
            if (d_f.size() > 0)
            {
                digital_filter = stoi(d_f);
            }
            f_close(&fil);
        }
#endif
}

void write_digital_filter()
{
#ifdef pmp_digital_filter
    fr = f_open(&fil, (pmp_path + "/" + pmp_digital_filter).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
    if (fr == FR_OK)
    {
        char buff;
        UINT bw;
        f_write(&fil, (to_string(digital_filter)).c_str(), strlen((to_string(digital_filter)).c_str()), &bw);
        f_close(&fil);
    }
#endif
}

void next_music_repeat_all()
{
    if (music_select < (musics.size() - 1))
    {
        music_select++;
        if (!get_directory_list->is_dir[music_select])
        {
            string music_filename = music_path + "/" + musics[music_select];
            if (pico_tag1)
            {
                delete pico_tag1;
            }
            pico_tag1 = pico_tag_get_tag(music_filename);
            duration = pico_tag1->duration;
            pico_tag_wait = false;

            repeat_next = false;

            if (album_art_write)
            {
                draw_album_art();
                album_art_write = false;
            }

            player_screen_update = true;

            music_decoder_start();
            return;
        }
    }
    int count = 0;
    string dir = get_directory_list->path;
    while (1)
    {
        while (count < get_directory_list->name_list.size())
        {
            if (get_directory_list->is_dir[count])
            {
                get_directory_list->path = get_directory_list->path + "/" + get_directory_list->name_list[count];
                get_directory_list->update();
                int count2 = 0;
                while (count2 < get_directory_list->name_list.size())
                {
                    if (!get_directory_list->is_dir[count2])
                    {
                        music_path = get_directory_list->path;
                        music_select = count2;
                        musics = get_directory_list->name_list;

                        string music_filename = music_path + "/" + musics[music_select];
                        printf("%s\n", music_filename.c_str());
                        if (pico_tag1)
                        {
                            delete pico_tag1;
                        }
                        pico_tag1 = pico_tag_get_tag(music_filename);
                        duration = pico_tag1->duration;
                        pico_tag_wait = false;
                        repeat_next = false;

                        printf("album art\n");
                        if (get_directory_list->jpeg_file != "")
                        {
                            jpeg_file_name = music_path + "/" + get_directory_list->jpeg_file;
                        }
                        else
                        {
                            jpeg_file_name = "";
                        }
                        draw_album_art();
                        album_art_write = false;

                        player_screen_update = true;
                        
                        music_decoder_start();
                        return;
                    }
                    count2++;
                }
                count = 0;
            }
            else
            {
                count++;
            }
        }
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
        get_directory_list->update();

        for (int i = 0; i < get_directory_list->name_list.size(); i++)
        {
            if (get_directory_list->name_list[i] == path_splitted[path_splitted.size() - 1])
            {
                count = i + 1;
            }
        }

        if (get_directory_list->path == "/")
        {
            if (count == get_directory_list->name_list.size())
            {
                count = 0;
            }
        }
    }
}

int main()
{
    setup_si5351_i2c();

#if defined(DAC_CS4398) || defined(DAC_Zero_HAT_DAC_CS4398)
    cs4398_setup();
#endif

#ifdef DAC_DacPlusPro
    DacPlusPro_setup();
    DacPlusPro_change_bit_freq(32, 44100);
#else
    si5351_set_clock(32, 44100);
#endif

    set_sys_clock_pll(1596 * MHZ, 6, 2); // 133MHz

    board_init();
    tusb_init();
    usb_serial_init();

    gpio_set_function(TFT_BL, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(TFT_BL);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 100);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(TFT_BL, 0);

    // sleep_ms(2000);
    // printf("Pico Pi DAP Zero\n");

    init_key();

    int status = f_mount(&FatFs, "", 0); /* Give a work area to the default drive */

    if (gpio_get(buttons_pin[sd_init_skip_button1]) && gpio_get(buttons_pin[sd_init_skip_button2]))
    {
        if (!init_sd())
        {
            printf("init_sd error\n");
            sd_status = false;
        }
    }
    change_volume(volume);

    stack_core1 = new uint32_t[stack_core1_size];
    multicore_launch_core1_with_stack(core1, stack_core1, stack_core1_size);

    // audio decoder source
    source = new AudioFileSourceSD();

    char buff[256];
    strcpy(buff, "/");

    while (1)
    {
        audio_decoder_pre = audio_decoder;
        audio_decoder = true;

        if ((audio_type_pre == wav_file) && wav_playing)
        {
            if (!pause)
            {
                if (music_playing && !wav_playing)
                {
                    if ((playing_mode == playing_mode_repeat_all) && (repeat_next))
                    {
                        next_music_repeat_all();
                        repeat_next = false;
                    }
                    else{
                        music_decoder_start();
                    }
                }
                if (!wav_loop())
                {
                    wav_stop();
                    if (playing_mode == playing_mode_repeat_1)
                    {
                        play_start = to_ms_since_boot(get_absolute_time());
                        music_decoder_start();
                    }
                    else if (playing_mode == playing_mode_normal)
                    {
                        if (music_select < (musics.size() - 1))
                        {
                            music_select++;
                            player_screen_rotate_num = 1;
                            rotate_count = 0xffff - 1;
                            play_start = to_ms_since_boot(get_absolute_time());
                            string music_filename = music_path + "/" + musics[music_select];
                            if (pico_tag1)
                            {
                                delete pico_tag1;
                                pico_tag1 = NULL;
                            }
                            pico_tag1 = pico_tag_get_tag(music_filename);
                            duration = pico_tag1->duration;
                            player_screen_update = true;
                            music_decoder_start();
                        }
                        else
                        {
                            if (music_playing){
                                pause_time = to_ms_since_boot(get_absolute_time());
                                status_bar_left_update(0);
                            }
                            music_playing = false;
                            pause = false;
                        }
                    }
                    else if(playing_mode == playing_mode_repeat_all)
                    {
                        next_music_repeat_all();
                    }
                }
                else
                {
                    wav_playing = true;
                }
            }
        }
        else
        {
            if ((decoder) && (decoder->isRunning()))
            {
                if (!pause)
                {
                    if (!decoder->loop())
                    {
                        if (!seeking)
                        {
                            decoder->stop();
                            if (playing_mode == playing_mode_repeat_1)
                            {
                                play_start = to_ms_since_boot(get_absolute_time());
                                music_decoder_start();
                            }
                            else if (playing_mode == playing_mode_normal)
                            {
                                if (music_select < (musics.size() - 1))
                                {
                                    music_select++;
                                    player_screen_rotate_num = 1;
                                    rotate_count = 0xffff - 1;
                                    play_start = to_ms_since_boot(get_absolute_time());
                                    string music_filename = music_path + "/" + musics[music_select];
                                    if (pico_tag1)
                                    {
                                        delete pico_tag1;
                                        pico_tag1 = NULL;
                                    }
                                    pico_tag1 = pico_tag_get_tag(music_filename);
                                    duration = pico_tag1->duration;
                                    player_screen_update = true;
                                    music_decoder_start();
                                }
                                else
                                {
                                    if (music_playing){
                                        pause_time = to_ms_since_boot(get_absolute_time());
                                        status_bar_left_update(0);
                                    }
                                    music_playing = false;
                                    pause = false;
                                }
                            }
                            else if(playing_mode == playing_mode_repeat_all)
                            {
                                next_music_repeat_all();
                            }
                        }
                    }
                }
            }
            else
            {
                if (music_playing)
                {
                    if (!seeking)
                    {
                        if ((playing_mode == playing_mode_repeat_all) && (repeat_next))
                        {
                            next_music_repeat_all();
                            repeat_next = false;
                        }
                        else{
                            music_decoder_start();
                        }
                    }
                    else
                    {
                        audio_decoder = false;
                    }
                }
            }
        }

        if (let_music_stop)
        {
            printf("stop\n");
            if (audio_type_pre == wav_file)
            {
                wav_stop();
            }
            else
            {
                decoder->stop();
            }
            pause = false;
            let_music_stop = false;
            music_playing = false;
            wav_playing = false;
        }

        if (seek)
        {
            printf("seek audio\n");
            seek_audio();
            seek = false;
        }

        if (!audio_decoder_pre)
        {
            seek_audio();
        }

        if (get_dir_name_list)
        {
            printf("get_directory_name_list\n");
            get_directory_list->get_dir_name_list();
            get_dir_name_list = false;
        }

        if (get_is_dir)
        {
            printf("get_directory_list\n");
            get_directory_list->get_is_dir();
            get_is_dir = false;
        }

        if (file_update)
        {
            printf("file_update\n");
            get_directory_list->update();
            file_update = false;
        }

        if (pico_tag_wait)
        {
            printf("pico_tag_wait\n");
            string music_filename = music_path + "/" + musics[music_select];
            if (pico_tag1)
            {
                delete pico_tag1;
            }
            pico_tag1 = pico_tag_get_tag(music_filename);
            duration = pico_tag1->duration;
            pico_tag_wait = false;
        }

        if (album_art_write)
        {
            printf("album art\n");
            draw_album_art();
            album_art_write = false;
        }

        if (usb_dac_mode)
        {
            printf("USB-DAC mode\n");
            if (decoder)
            {
                delete decoder;
                decoder = NULL;
            }

            if (out)
            {
                delete out;
            }

            if (audio_type_pre == wav_file)
            {
                wav_end();
            }
            uac2_main();
        }

        if (digital_filter_read)
        {
            read_digital_filter();
            digital_filter_read = false;
        }

        if (digital_filter_write)
        {
            write_digital_filter();
            digital_filter_write = false;
        }

        music_playing_pre = music_playing;
    }

    return 0;
}