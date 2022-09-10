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
#include "pico/multicore.h"
#include "hardware/pwm.h"

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

#include "i2c_devices.h"

#include <math.h>

volatile file_type audio_type_pre = file_start;

AudioFileSourceSD *source = NULL;
AudioGenerator *decoder = NULL;

AudioOutputI2S *out = NULL;

uint8_t bit_pre = 0;
uint32_t sampling_rate_pre = 0;

volatile bool wav_playing = true;

volatile uint8_t volume_pre = 0;

void change_volume(uint8_t vol)
{
#ifndef SOFT_VOL
#ifdef ModelB
    if (dac1->set_volume(vol) && (!softvol))
#else
    if (dac1->set_volume(vol))
#endif
    {
        if (out)
        {
            out->SetGain(1.0);
        }
        wav_set_gain(1.0);        
    }
    else
#endif
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
}

void set_si5351(uint32_t sample_rate)
{
    if (dac1 != NULL)
    {
        if (dac1->get_dac() == dac_pcm512x)
        {
#ifdef ModelB
            i2c_inst_t i2c_port1 = get_i2c_port(sda_pin2, scl_pin2);
            setup_i2c(sda_pin2, scl_pin2, i2c_port1);
            si5351_set_clock(si5351_i2c_port, 0x60, 0, 0, 0);
            i2c_inst_t i2c_port2 = get_i2c_port(sda_pin, scl_pin);
            setup_i2c(sda_pin, scl_pin, i2c_port2);
#else
            si5351_set_clock(si5351_i2c_port, 0x60, 0, 0, 0);
#endif
            return;
        }
    }

    if (external_si5351)
    {
#ifdef ModelB
        i2c_inst_t i2c_port2 = get_i2c_port(sda_pin, scl_pin);
        setup_i2c(sda_pin, scl_pin, i2c_port2);
#endif
        if (sample_rate == 0)
        {
            si5351_set_clock(si5351_i2c_port, 0x60, 0, 0, 0);
        }
        else if (sample_rate % 48000 == 0)
        {
            si5351_set_clock(si5351_i2c_port, 0x60, sample_rate, -sample_rate * 64, -mclk_48k);
        }
        else
        {
            si5351_set_clock(si5351_i2c_port, 0x60, sample_rate, -sample_rate * 64, -mclk_44_1k);
        }
    }
    else
    {
#ifdef ModelB
        i2c_inst_t i2c_port1 = get_i2c_port(sda_pin2, scl_pin2);
        setup_i2c(sda_pin2, scl_pin2, i2c_port1);
#endif
        if (sample_rate == 0)
        {
            si5351_set_clock(si5351_i2c_port, 0x60, 0, 0, 0);
        }
        else 
        if (sample_rate % 48000 == 0)
        {
            si5351_set_clock(si5351_i2c_port, 0x60, -mclk_48k, -sample_rate * 64, sample_rate);
        }
        else
        {
            si5351_set_clock(si5351_i2c_port, 0x60, -mclk_44_1k, -sample_rate * 64, sample_rate);
        }
    }
#ifdef ModelB
    i2c_inst_t i2c_port2 = get_i2c_port(sda_pin, scl_pin);
    setup_i2c(sda_pin, scl_pin, i2c_port2);
#endif
}

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

        if (dac1->get_digital_filter_num() > 0)
        {
            fr = f_open(&fil, (pmp_path + "/" + dac1->get_digital_filter_text_name()).c_str(), FA_READ | FA_OPEN_EXISTING);
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
        }

        fr = f_open(&fil, (pmp_path + "/" + pmp_playing_mode).c_str(), FA_READ | FA_OPEN_EXISTING);
        if (fr == FR_OK)
        {
            char line[100];
            f_gets(line, sizeof(line), &fil);
            string p_m = line;
            if (p_m.size() > 0)
            {
                playing_mode = stoi(p_m);
            }
            f_close(&fil);
        }

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

    if (playing_mode_write)
    {
        fr = f_open(&fil, (pmp_path + "/" + pmp_playing_mode).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
        if (fr == FR_OK)
        {
            char buff;
            UINT bw;
            f_write(&fil, (to_string(playing_mode)).c_str(), strlen((to_string(playing_mode)).c_str()), &bw);
            f_close(&fil);
        }
        playing_mode_write = false;
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

            dac1->set_bit_freq(32, pico_tag1->sampling_rate);
            set_si5351(pico_tag1->sampling_rate);

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

            dac1->set_bit_freq(32, pico_tag1->sampling_rate);
            set_si5351(pico_tag1->sampling_rate);

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

    change_volume(0);
    volume_pre = 0;
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
    if (dac1->get_digital_filter_num() > 0)
    {
        fr = f_open(&fil, (pmp_path + "/" + dac1->get_digital_filter_text_name()).c_str(), FA_READ | FA_OPEN_EXISTING);
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
        else
        {
            digital_filter = -1;
        }
    }
}

void write_digital_filter()
{
    if (dac1->get_digital_filter_num() > 0)
    {
        fr = f_open(&fil, (pmp_path + "/" + dac1->get_digital_filter_text_name()).c_str(), FA_WRITE | FA_CREATE_ALWAYS);
        if (fr == FR_OK)
        {
            char buff;
            UINT bw;
            f_write(&fil, (to_string(digital_filter)).c_str(), strlen((to_string(digital_filter)).c_str()), &bw);
            f_close(&fil);
        }
    }
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
#ifdef ModelB
    i2c_inst_t i2c_port1 = get_i2c_port(sda_pin2, scl_pin2);
    setup_i2c(sda_pin2, scl_pin2, i2c_port1);
    uint8_t i2c_dac_clk[3]; // [0] : 0 -> no clock, 1 -> Si5351A, [1] : DAC, [2] : DAC Address

    get_i2c_devices(i2c_port1, i2c_dac_clk, true);

    for (int i = 0; i < 3; i++)
    {
        i2c_dac_clk[i] = 0;
    }

    i2c_inst_t i2c_port2 = get_i2c_port(sda_pin, scl_pin);
    setup_i2c(sda_pin, scl_pin, i2c_port2);

    get_i2c_devices(i2c_port2, i2c_dac_clk, false);

    if (i2c_dac_clk[0] == 1)
    {
        setup_i2c(sda_pin2, scl_pin2, i2c_port1);
        si5351_i2c_port = i2c_port1;
        set_si5351(0);
        external_si5351 = true;
        setup_i2c(sda_pin, scl_pin, i2c_port2);
        si5351_i2c_port = i2c_port2;
    }
    else
    {
        si5351_i2c_port = i2c_port1;
    }

    gpio_init(sw1_pin);
    gpio_set_dir(sw1_pin, GPIO_IN);
    gpio_pull_up(sw1_pin);
#else
    i2c_inst_t i2c_port1 = get_i2c_port(sda_pin, scl_pin);
    setup_i2c(sda_pin, scl_pin, i2c_port1);
    uint8_t i2c_dac_clk[3]; // [0] : 0 -> no clock, 1 -> Si5351A, [1] : DAC, [2] : DAC Address

    si5351_i2c_port = i2c_port1;

#ifdef EXT_CLK
    external_si5351 = true;
    get_i2c_devices(i2c_port1, i2c_dac_clk, false);
#else
    get_i2c_devices(i2c_port1, i2c_dac_clk, true);
#endif
#endif // ModelB
    if (i2c_dac_clk[1] == dac_cs4398)
    {
        dac1 = new cs4398();
        dac1->set_dac_address(i2c_dac_clk[2]);
#ifdef ModelB
        dac1->set_i2c_port(i2c_port2);
#else
        dac1->set_i2c_port(i2c_port1);
#endif
        dac1->setup();
        set_si5351(44100);
    }
    else if ((i2c_dac_clk[1] == dac_DacPlusPro) || (i2c_dac_clk[1] == dac_pcm512x))
    {
        dac1 = new pcm512x();
        dac1->set_dac_address(i2c_dac_clk[2]);
#ifdef ModelB
        dac1->set_i2c_port(i2c_port2);
#else
        dac1->set_i2c_port(i2c_port1);
#endif
        set_si5351(0);
        dac1->setup();
    }
    else
    {
        set_si5351(44100);
        sleep_ms(300);
#ifdef ModelB
        get_i2c_devices(i2c_port2, i2c_dac_clk, true);
#else
#ifdef EXT_CLK
        get_i2c_devices(i2c_port1, i2c_dac_clk, false);
#else
        get_i2c_devices(i2c_port1, i2c_dac_clk, true);
#endif
#endif // ModelB

        if (i2c_dac_clk[1] == dac_pcm1795)
        {
            dac1 = new pcm1795();
        }
        else if (i2c_dac_clk[1] == dac_ak449x)
        {
            dac1 = new ak449x();
        }
        else
        {
            dac1 = new general_i2s();
#ifdef DAC_FPGA_DeltaSigma
            dac_data_string[2] = "         FPGA DeltaSigma";
#endif
        }

        dac1->set_dac_address(i2c_dac_clk[2]);
#ifdef ModelB
        dac1->set_i2c_port(i2c_port2);
#else
        dac1->set_i2c_port(i2c_port1);
#endif
        dac1->setup();
    }

#ifdef ModelB
    if (!gpio_get(sw1_pin))
    {
        if (dac1->get_dac() == dac_general_i2s)
        {
            dac_data_string[2] = "         FPGA DeltaSigma";
            mclk_44_1k *= 4;
            mclk_48k *= 4;
        }
        else
        {
            softvol = true;
            dac_data_string[2].erase(dac_data_string[2].begin(), dac_data_string[2].begin() + 3);
            dac_data_string[2] += " SoftVol";
        }
    }
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
                    else if (playing_mode == playing_mode_repeat_directory)
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
                            music_select = 0;
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
                            else if (playing_mode == playing_mode_repeat_directory)
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
                                    music_select = 0;
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

        if (volume != volume_pre)
        {
            change_volume(volume);
            volume_pre = volume;
        }

        music_playing_pre = music_playing;
    }

    return 0;
}