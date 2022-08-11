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

#include "ak449x.h"

#include "hardware/i2c.h"

#define i2c_port_ak449x i2c1

uint8_t ak449x_filter = 2;

uint8_t ak449x_register_value[10] = {
    0b10001111, // 0
    0b10100010, // 1
    0b00000000, // 2
    0b00000000, // 3
    0b00000000, // 4
    0b00000000, // 5
    0b00000000, // 6
    0b00000001, // 7
    0b00000000, // 8
    0b00000000  // 9
};

int send_i2c(uint8_t reg, uint8_t value)
{
    uint8_t reg_data[2];
    reg_data[0] = reg;
    reg_data[1] = value;
    int bw = i2c_write_blocking(i2c_port_ak449x, ak449x_address, &reg_data[0], 2, false);
    return bw;
}

void ak449x_setup()
{
    sleep_ms(300);
    for (uint8_t i = 0; i < 10; i++)
    {
        send_i2c(i, ak449x_register_value[i]);
    }

    // reset ak449x
    ak449x_register_value[0] &= 0b11111110;
    send_i2c(0, ak449x_register_value[0]);

    ak449x_register_value[0] |= 0b00000001;
    send_i2c(0, ak449x_register_value[0]);
}

void change_volume_ak449x(uint8_t vol)
{
    uint8_t vol_value_send = ((vol_max - vol_min) * vol / 100);

    if (vol == 0)
    {
        vol_value_send = vol_min;
    }

    printf("vol = %d\n", vol_value_send);

    ak449x_register_value[3] = vol_value_send;
    ak449x_register_value[4] = vol_value_send;

    send_i2c(3, vol_value_send);
    send_i2c(4, vol_value_send);
}
 
void ak449x_change_digital_filter(int digital_filter)
{
    switch (digital_filter)
    {
    case 0:
        ak449x_register_value[1] &= 0b11011111;
        ak449x_register_value[2] &= 0b11111110;
        ak449x_register_value[5] &= 0b11111110;
        break;

    case 1:
        ak449x_register_value[1] &= 0b11011111;
        ak449x_register_value[2] |= 0b00000001;
        ak449x_register_value[5] &= 0b11111110;
        break;

    case 2:
        ak449x_register_value[1] |= 0b00100000;
        ak449x_register_value[2] &= 0b11111110;
        ak449x_register_value[5] &= 0b11111110;
        break;

    case 3:
        ak449x_register_value[1] |= 0b00100000;
        ak449x_register_value[2] |= 0b00000001;
        ak449x_register_value[5] &= 0b11111110;
        break;

    case 4:
        ak449x_register_value[1] &= 0b11011111;
        ak449x_register_value[2] &= 0b11111110;
        ak449x_register_value[5] |= 0b00000001;
        break;

    case 5:
        ak449x_register_value[1] |= 0b00100000;
        ak449x_register_value[2] &= 0b11111110;
        ak449x_register_value[5] |= 0b00000001;
        break;
        
    default:
        break;
    }

    send_i2c(1, ak449x_register_value[1]);
    send_i2c(2, ak449x_register_value[2]);
    send_i2c(5, ak449x_register_value[5]);
} 

