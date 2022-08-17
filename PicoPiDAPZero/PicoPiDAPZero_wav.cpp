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

#pragma GCC optimize("O2")

#include "PicoPiDAPZero.h"
#include "ff.h"
#include "pico_i2s.h"

#define i2s_buff_size_wav (4096 * 3)
int bit_num_playing;
FIL f_wav;
volatile bool first = true;
bool file_opened = false;
bool play_stop = false;
float gain_wav = 1.0;
uint32_t wav_end_byte = 0;

int16_t *wav_buff;
int32_t vol_db;

static const int vol_div  = 1 << 16;
void set_buffer(int buffsel)
{
    if (gain_wav < 0.99)
    {
        for (int i = 0; i < i2s_buff_size; i += 2)
        {
            int32_t *buff_i2s_32 = (int32_t *)&i2s_buff[buffsel][i];
            int32_t *buff_wav_32 = (int32_t *)&wav_buff[i];
            // buff_i2s_32[0] = buff_wav_32[0] * gain_wav;
            buff_i2s_32[0] = (int32_t)(((int64_t)buff_wav_32[0] * (int64_t)vol_db) / (int64_t)vol_div);
        }
    }
    else
    {
        memcpy(&i2s_buff[buffsel][0], wav_buff, i2s_buff_size * 2);
    }
}

void convert_24_to_32()
{
    uint8_t *buff_8 = (uint8_t *)&wav_buff[0];
    uint8_t tmp[3];
    int num24bit_count = i2s_buff_size * 2 * 3 / 4 - 1;
    for (int i = (i2s_buff_size * 2 - 1); i > 0; i -= 4)
    {
        tmp[2] = buff_8[num24bit_count--];
        tmp[1] = buff_8[num24bit_count--];
        tmp[0] = buff_8[num24bit_count--];
        buff_8[i - 0] = tmp[2];
        buff_8[i - 1] = tmp[1];
        buff_8[i - 2] = tmp[0];
        buff_8[i - 3] = 0;
    }
}

void convert_16_to_32()
{
    uint8_t *buff_8 = (uint8_t *)&wav_buff[0];
    uint8_t tmp[2];
    int num16bit_count = i2s_buff_size - 1;
    for (int i = (i2s_buff_size * 2 - 1); i > 0; i -= 4)
    {
        tmp[1] = buff_8[num16bit_count--];
        tmp[0] = buff_8[num16bit_count--];
        buff_8[i - 0] = tmp[1];
        buff_8[i - 1] = tmp[0];
        buff_8[i - 2] = 0;
        buff_8[i - 3] = 0;
    }
}

void wav_start(uint8_t bit_num)
{
    i2s_buff_size = i2s_buff_size_wav;
    init_i2s(32);
    bit_num_playing = bit_num;

    if (!wav_buff)
    {
        wav_buff = new (std::nothrow) int16_t [i2s_buff_size_wav + 4];
        if (wav_buff == NULL)
        {
            return;
        }
        memset(&wav_buff[0], 0, (i2s_buff_size * 2 + 4));
    }
}

void wav_open(string wav_file_path)
{
    if (file_opened)
    {
        f_close(&f_wav);
    }
    f_open(&f_wav, wav_file_path.c_str(), FA_OPEN_EXISTING | FA_READ);
    file_opened = true;
}

void wav_set_end_byte(uint32_t end_byte)
{
    wav_end_byte = end_byte;
}

bool wav_loop()
{
    UINT byte_read;
    if (first)
    {
        first = false;

        // set 0 to i2s_buff to avoid noise
        memset(&i2s_buff[0][0], 0, i2s_buff_size * 2);
        memset(&i2s_buff[1][0], 0, i2s_buff_size * 2);

        set_buffer(0);
        set_buffer(1);

        i2s_start();
    }
    else if (play_stop)
    {
        play_stop = false;

        // set 0 to i2s_buff to avoid noise
        memset(&i2s_buff[0][0], 0, i2s_buff_size * 2);
        memset(&i2s_buff[1][0], 0, i2s_buff_size * 2);

        set_buffer(0);
        set_buffer(1);
    }
    if (int_count_i2s > (i2s_buff_count - 1))
    {
        uint32_t bytes_to_read;
        if (bit_num_playing == 16)
        {
            bytes_to_read = i2s_buff_size;
        }
        else if (bit_num_playing == 24)
        {
            bytes_to_read = i2s_buff_size * 2 * 3 / 4;
        }
        else // 32bit
        {
            bytes_to_read = i2s_buff_size * 2;
        }

        if (wav_end_byte != 0)
        {
            if ((f_tell(&f_wav) + bytes_to_read) > wav_end_byte)
            {
                f_read(&f_wav, &wav_buff[0], (wav_end_byte - f_tell(&f_wav)), &byte_read);
                if (bit_num_playing == 24)
                {
                    convert_24_to_32();
                }
                else if (bit_num_playing == 16)
                {
                    convert_16_to_32();
                }
            }
            else
            {
                f_read(&f_wav, &wav_buff[0], bytes_to_read, &byte_read);
                if (bit_num_playing == 24)
                {
                    convert_24_to_32();
                }
                else if (bit_num_playing == 16)
                {
                    convert_16_to_32();
                }
            }
        }
        else
        {
            f_read(&f_wav, &wav_buff[0], bytes_to_read, &byte_read);
            if (bit_num_playing == 24)
            {
                convert_24_to_32();
            }
            else if (bit_num_playing == 16)
            {
                convert_16_to_32();
            }
        }

        set_buffer((int_count_i2s + 1) % 2);

        if (byte_read != bytes_to_read)
        {
            return false;
        }
        i2s_buff_count = int_count_i2s + 1;
    }
    return true;
}

void wav_stop()
{
    if (file_opened)
    {
        f_close(&f_wav);
    }
    play_stop = true;
    memset(&i2s_buff[0][0], 0, (i2s_buff_size + 4) * 2);
    memset(&i2s_buff[1][0], 0, (i2s_buff_size + 4) * 2);
}

void wav_seek(uint32_t seek_point)
{
    f_lseek(&f_wav, seek_point);
}

void wav_end()
{
    first = true;
    deinit_i2s();

    if (file_opened)
    {
        f_close(&f_wav);
    }

    delete[] wav_buff;
    wav_buff = NULL;
}

void wav_set_gain(float gain)
{
    gain_wav = gain;
    vol_db = (int32_t)(gain * (float)(1 << 16));
}