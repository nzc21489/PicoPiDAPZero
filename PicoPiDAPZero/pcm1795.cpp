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

#include "pcm1795.h"

#include "hardware/i2c.h"

#define i2c_port_pcm7195 i2c1

uint8_t pcm1795_filter = 0;

int send_i2c(uint8_t reg, uint8_t value)
{
    uint8_t reg_data[2];
    reg_data[0] = reg;
    reg_data[1] = value;
    int bw = i2c_write_blocking(i2c_port_pcm7195, pcm1795_address, &reg_data[0], 2, false);
    return bw;
}

void pcm1795_setup()
{
    sleep_ms(50);
    
    // set format I2S 32bit, Attenuation enable
    send_i2c(18, 0b11000000);

    change_volume_pcm1795(vol_min);
    send_i2c(18, 0b11000000);
}

void change_volume_pcm1795(uint8_t vol)
{
    uint8_t vol_value_send = ((vol_max - vol_min) * vol / 100) + vol_min;

    if (vol == 0)
    {
        vol_value_send = vol_min;
    }
    send_i2c(16, vol_value_send);
    send_i2c(17, vol_value_send);
    send_i2c(18, 0b11000000); // update volume
}
 
void pcm1795_change_digital_filter(int digital_filter)
{
    if ((digital_filter == 0) || (digital_filter == 1))
    {
        // digital filter
        send_i2c(19, (digital_filter << 1));
    }
} 
