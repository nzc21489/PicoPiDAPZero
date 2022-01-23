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

#ifndef NO_SOFT_VOL
void soft_gain(int buffsel)
{
    if (bit_num_playing == 16)
    {
        for (int i = 0; i < i2s_buff_size; i++)
        {
            i2s_buff[buffsel][i] *= gain_wav;
        }
    }
    else if (bit_num_playing == 24)
    {
        for (int i = 0; i < i2s_buff_size; i += 3)
        {
            uint8_t *buff_8 = (uint8_t *)&i2s_buff[buffsel][i];
            int32_t buff_gained_32[2];
            uint8_t *buff_gained_8 = (uint8_t *)&buff_gained_32;

            *(buff_gained_8++) = 0;
            buff_8 += 2;
            *(buff_gained_8++) = *(buff_8--);
            *(buff_gained_8++) = *(buff_8--);
            *(buff_gained_8++) = *(buff_8--);
            *(buff_gained_8++) = 0;
            buff_8 = (uint8_t *)&i2s_buff[buffsel][i] + 5;
            *(buff_gained_8++) = *(buff_8--);
            *(buff_gained_8++) = *(buff_8--);
            *(buff_gained_8++) = *(buff_8--);

            buff_8 = (uint8_t *)&i2s_buff[buffsel][i];
            buff_gained_8 = (uint8_t *)&buff_gained_32;

            buff_gained_32[0] *= gain_wav;
            buff_gained_32[1] *= gain_wav;

            buff_gained_8 += 3;
            *(buff_8++) = *(buff_gained_8--);
            *(buff_8++) = *(buff_gained_8--);
            *(buff_8++) = *(buff_gained_8--);
            buff_gained_8 = (uint8_t *)&buff_gained_32[1] + 3;
            *(buff_8++) = *(buff_gained_8--);
            *(buff_8++) = *(buff_gained_8--);
            *(buff_8++) = *(buff_gained_8--);
        }
    }
    else // 32bit
    {
        for (int i = 0; i < i2s_buff_size; i += 2)
        {
            int16_t *buff_16 = (int16_t *)&i2s_buff[buffsel][i];
            int32_t buff_gained_32[1];
            int16_t *buff_gained_16 = (int16_t *)&buff_gained_32;
            buff_16 += 1;
            *(buff_gained_16++) = *(buff_16--);
            *(buff_gained_16++) = *(buff_16--);

            buff_gained_32[0] *= gain_wav;

            *(++buff_16) = *(--buff_gained_16);
            *(++buff_16) = *(--buff_gained_16);
        }
    }
}
#endif

void wav_start(uint8_t bit_num)
{
    i2s_buff_size = i2s_buff_size_wav;
    init_i2s(bit_num);
    bit_num_playing = bit_num;
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

#ifndef NO_SOFT_VOL
        soft_gain(0);
        soft_gain(1);
#endif
        i2s_start();
    }
    else if (play_stop)
    {
        play_stop = false;

        // set 0 to i2s_buff to avoid noise
        memset(&i2s_buff[0][0], 0, i2s_buff_size * 2);
        memset(&i2s_buff[1][0], 0, i2s_buff_size * 2);


#ifndef NO_SOFT_VOL
        soft_gain(0);
        soft_gain(1);
#endif
    }
    if (int_count_i2s > (i2s_buff_count - 1))
    {
        if (wav_end_byte != 0)
        {
            if ((f_tell(&f_wav) + i2s_buff_size * 2) > wav_end_byte)
            {
                f_read(&f_wav, &i2s_buff[(int_count_i2s + 1) % 2][0], (wav_end_byte - f_tell(&f_wav)), &byte_read);
            }
            else
            {
                f_read(&f_wav, &i2s_buff[(int_count_i2s + 1) % 2][0], i2s_buff_size * 2, &byte_read);
            }
        }
        else
        {
            f_read(&f_wav, &i2s_buff[(int_count_i2s + 1) % 2][0], i2s_buff_size * 2, &byte_read);
        }

#ifndef NO_SOFT_VOL
        soft_gain((int_count_i2s + 1) % 2);
#endif

        if (byte_read != i2s_buff_size * 2)
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
}

void wav_set_gain(float gain)
{
    gain_wav = gain;
}