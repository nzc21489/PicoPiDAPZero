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

#include "pcm512x.h"

#include "pico_i2c.h"

pcm512x::pcm512x()
{
    
}

void pcm512x::set_dac_address(uint8_t address)
{
    dac_address = address;
}

void pcm512x::set_i2c_port(i2c_inst_t i2c_port)
{
    dac_i2c_port = i2c_port;
}

void pcm512x::setup()
{
    // standby
    write_i2c(dac_i2c_port, dac_address, 2, 0b10000);

    //gpio 3,4,6 output
    write_i2c(dac_i2c_port, dac_address, 8, 0b101100);

    //gpio 3 output
    write_i2c(dac_i2c_port, dac_address, 82, 0b10);

    //gpio 4 output
    write_i2c(dac_i2c_port, dac_address, 83, 0b10);

    //gpio 6 output
    write_i2c(dac_i2c_port, dac_address, 85, 0b10);

    //gpio 4,6 output
    write_i2c(dac_i2c_port, dac_address, 86, 0b101000);

    // BCK LRCK output
    write_i2c(dac_i2c_port, dac_address, 9, 0b10001);

    // BCK LRCK divider functional
    write_i2c(dac_i2c_port, dac_address, 12, 0b11);

    // // LRCK divier
    // write_i2c(dac_i2c_port, dac_address, 33, 0b11111);

    // clock detection setting
    write_i2c(dac_i2c_port, dac_address, 37, 0b1111011);

    // diable pll
    write_i2c(dac_i2c_port, dac_address, 4, 0b0);

    // dac clock source sck
    write_i2c(dac_i2c_port, dac_address, 14, 0b110000);

    // dsp clock divider = 1
    write_i2c(dac_i2c_port, dac_address, 27, 0b0);

    //dac clock divider = 4
    write_i2c(dac_i2c_port, dac_address, 28, 0b11);

    //ncp clock divider = 4
    write_i2c(dac_i2c_port, dac_address, 29, 0b11);

    // osr clock divider = 8
    write_i2c(dac_i2c_port, dac_address, 30, 0b111);

    // BCK divider = 16
    write_i2c(dac_i2c_port, dac_address, 32, 0b1111);

    // LRCK divier = 32
    write_i2c(dac_i2c_port, dac_address, 33, 0b11111);

    // IDAC
    write_i2c(dac_i2c_port, dac_address, 35, 0b10);

    // IDAC
    write_i2c(dac_i2c_port, dac_address, 36, 0b0);

    set_volume(0);

    // digital filter
    write_i2c(dac_i2c_port, dac_address, 43, 1);

    // sync request ?
    write_i2c(dac_i2c_port, dac_address, 19, 0b10001);

    // sync request ?
    write_i2c(dac_i2c_port, dac_address, 19, 0b10000);

    // normal operation
    write_i2c(dac_i2c_port, dac_address, 2, 0b0);
}

void pcm512x::mute()
{
    write_i2c(dac_i2c_port, dac_address, 3, 0b10001);
}

void pcm512x::unmute()
{
    write_i2c(dac_i2c_port, dac_address, 3, 0b0);
}

uint8_t pcm512x::get_bck_divider(uint8_t bit, uint32_t freq)
{
    int div = 1;
    if (bit == 24)
    {
        bit = 32;
    }
    if (freq % 48000 == 0)
    {
        div = 32 * 48000 * 8 * 2 / (bit * freq * 2);
    }
    else
    {
        div = 32 * 44100 * 8 * 2 / (bit * freq * 2);
    }

    return div;
}

void pcm512x::set_bit_freq(uint8_t bit, uint32_t freq)
{
    // mute
    write_i2c(dac_i2c_port, dac_address, 3, 0b10001);

    // standby
    write_i2c(dac_i2c_port, dac_address, 2, 0b10000);

    //gpio 4 output
    write_i2c(dac_i2c_port, dac_address, 86, 0b001000);

    // select mclk
    if (freq % 48000 == 0) // 48k
    {
        //gpio 3,4 output
        write_i2c(dac_i2c_port, dac_address, 86, 0b001100);
    }
    else // 44.1k
    {
        //gpio 4,6 output
        write_i2c(dac_i2c_port, dac_address, 86, 0b101000);
    }

    //dac clock divider = 8
    write_i2c(dac_i2c_port, dac_address, 28, 0b111);

    //ncp clock divider = 4
    write_i2c(dac_i2c_port, dac_address, 29, 0b11);

    // osr clock divider = 8
    write_i2c(dac_i2c_port, dac_address, 30, (32 / ((int)(freq / 44100))) - 1);

    // BCK divider
    write_i2c(dac_i2c_port, dac_address, 32, (get_bck_divider(bit, freq) - 1));

    // LRCK divier
    if (bit <= 16)
    {
        write_i2c(dac_i2c_port, dac_address, 33, 31);
    }
    else
    {
        write_i2c(dac_i2c_port, dac_address, 33, 63);
    }

    // fs speed
    if (freq <= 48000)
    {
        write_i2c(dac_i2c_port, dac_address, 34, 0);
    }
    else if (freq <= 96000)
    {
        write_i2c(dac_i2c_port, dac_address, 34, 1);
    }
    else if (freq <= 192000)
    {
        write_i2c(dac_i2c_port, dac_address, 34, 2);
    }
    else
    {
        write_i2c(dac_i2c_port, dac_address, 34, 3);
    }

    // sync request ?
    write_i2c(dac_i2c_port, dac_address, 19, 0b10001);

    // sync request ?
    write_i2c(dac_i2c_port, dac_address, 19, 0b10000);

    // normal operation
    write_i2c(dac_i2c_port, dac_address, 2, 0b0);

    // right volume = left volume
    write_i2c(dac_i2c_port, dac_address, 60, 1);

    // change volume
    write_i2c(dac_i2c_port, dac_address, 61, vol_max);
    write_i2c(dac_i2c_port, dac_address, 62, vol_max);

    // unmute
    write_i2c(dac_i2c_port, dac_address, 3, 0b0);
}

bool pcm512x::set_volume(uint8_t vol)
{
    uint8_t vol_value_send = vol_min - ((vol_min - vol_max) * vol / 100);

    if (vol == 0)
    {
        vol_value_send = vol_min;
    }
    sleep_ms(2);
    write_i2c(dac_i2c_port, dac_address, 62, vol_value_send);
    sleep_ms(2);
    write_i2c(dac_i2c_port, dac_address, 61, vol_value_send);
    return true;
}

uint8_t pcm512x::get_digital_filter_num()
{
    return digital_filter_num;
}

string pcm512x::get_digital_filter_strs(uint8_t filter_num)
{
    return digital_filter_strs[filter_num];
}

void pcm512x::set_digital_filter(int filter_num)
{
    if (filter_num < digital_filter_num)
    {
        write_i2c(dac_i2c_port, dac_address, 43, digital_filter_nums[filter_num]);
    }
}

string pcm512x::get_digital_filter_text_name()
{
    return digital_filter_text;
}

dac_type pcm512x::get_dac()
{
    return dac_pcm512x;
}