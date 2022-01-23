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

#include "DacPlusPro.h"

#include "hardware/i2c.h"

#define i2c_port_DacPlusPro i2c1

int send_i2c(uint8_t reg, uint8_t value)
{
    uint8_t reg_data[2];
    reg_data[0] = reg;
    reg_data[1] = value;
    int bw = i2c_write_blocking(i2c_port_DacPlusPro, DacPlusPro_address, &reg_data[0], 2, false);
    return bw;
}

void DacPlusPro_setup()
{
    // standby
    send_i2c(2, 0b10000);

    //gpio 3,4,6 output
    send_i2c(8, 0b101100);

    //gpio 3 output
    send_i2c(82, 0b10);

    //gpio 4 output
    send_i2c(83, 0b10);

    //gpio 6 output
    send_i2c(85, 0b10);

    //gpio 4,6 output
    send_i2c(86, 0b101000);

    // BCK LRCK output
    send_i2c(9, 0b10001);

    // BCK LRCK divider functional
    send_i2c(12, 0b11);

    // // LRCK divier
    // send_i2c(33, 0b11111);

    // clock detection setting
    send_i2c(37, 0b1111011);

    // diable pll
    send_i2c(4, 0b0);

    // dac clock source sck
    send_i2c(14, 0b110000);

    // dsp clock divider = 1
    send_i2c(27, 0b0);

    //dac clock divider = 4
    send_i2c(28, 0b11);

    //ncp clock divider = 4
    send_i2c(29, 0b11);

    // osr clock divider = 8
    send_i2c(30, 0b111);

    // BCK divider = 16
    send_i2c(32, 0b1111);

    // LRCK divier = 32
    send_i2c(33, 0b11111);

    // IDAC
    send_i2c(35, 0b10);

    // IDAC
    send_i2c(36, 0b0);

    change_volume_DacPlusPro(vol_min);

    // sync request ?
    send_i2c(19, 0b10001);

    // sync request ?
    send_i2c(19, 0b10000);

    // normal operation
    send_i2c(2, 0b0);
}

uint8_t get_bck_divider(uint8_t bit, uint32_t freq)
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

void DacPlusPro_change_bit_freq(uint8_t bit, uint32_t freq)
{
    // mute
    send_i2c(3, 0b10001);

    // standby
    send_i2c(2, 0b10000);

    //gpio 4 output
    send_i2c(86, 0b001000);

    // select mclk
    if (freq % 48000 == 0) // 48k
    {
        //gpio 3,4 output
        send_i2c(86, 0b001100);
    }
    else // 44.1k
    {
        //gpio 4,6 output
        send_i2c(86, 0b101000);
    }

    //dac clock divider = 8
    send_i2c(28, 0b111);

    //ncp clock divider = 4
    send_i2c(29, 0b11);

    // osr clock divider = 8
    send_i2c(30, (32 / ((int)(freq / 44100)))-1);

    // BCK divider
    send_i2c(32, (get_bck_divider(bit, freq) - 1));

    // LRCK divier
    if (bit <= 16)
    {
        send_i2c(33, 31);
    }
    else
    {
        send_i2c(33, 63);
    }

    // fs speed
    if (freq <= 48000)
    {
        send_i2c(34, 0);
    }
    else if (freq <= 96000)
    {
        send_i2c(34, 1);
    }
    else if (freq <= 192000)
    {
        send_i2c(34, 2);
    }
    else
    {
        send_i2c(34, 3);
    }

    // sync request ?
    send_i2c(19, 0b10001);

    // sync request ?
    send_i2c(19, 0b10000);

    // normal operation
    send_i2c(2, 0b0);

    // unmute
    send_i2c(3, 0b0);
}

void DacPlusPro_mute()
{

}

void DacPlusPro_unmute()
{

}

void change_volume_DacPlusPro(uint8_t vol)
{
    uint8_t vol_value_send = (vol_max - vol_min) * vol / 100;
    if (vol == 0)
    {
        vol_value_send = vol_min;
    }
    send_i2c(61, vol_value_send);
    send_i2c(62, vol_value_send);
}
 
