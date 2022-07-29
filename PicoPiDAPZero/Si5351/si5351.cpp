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

#include "si5351.h"
#include "pico/binary_info.h"
#include <stdio.h>

const uint8_t Si5351_ADDR = 0x60;

void setup_si5351_i2c()
{

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    bi_decl(bi_2pins_with_func(sda_pin, scl_pin, GPIO_FUNC_I2C));
    i2c_init(i2c_port, 100000);
}

void si5351_send(int reg_count, const si5351a_revb_register_t *si5351_regs)
{
    // disable output
    uint8_t data[2];
    data[0] = 3;
    data[1] = 0xff;
    i2c_write_blocking(i2c_port, Si5351_ADDR, data, 2, false);

    // power down output drivers
    data[0] = 16;
    data[1] = 0x80;
    i2c_write_blocking(i2c_port, Si5351_ADDR, data, 2, false);
    data[0] = 17;
    data[1] = 0x80;
    i2c_write_blocking(i2c_port, Si5351_ADDR, data, 2, false);
    data[0] = 18;
    data[1] = 0x80;
    i2c_write_blocking(i2c_port, Si5351_ADDR, data, 2, false);

    for (volatile int i = 0; i < reg_count; i++)
    {
        uint8_t write_data[2];
        write_data[0] = (si5351_regs[i].address) & 0xff;
        write_data[1] = si5351_regs[i].value;
        if (si5351_regs[i].address == 17)
        {
            write_data[1] |= 0b10000; // clock1 invert
        }
        i2c_write_blocking(i2c_port, Si5351_ADDR, write_data, 2, false);
    }

    // soft reset
    data[0] = 177;
    data[1] = 0xac;
    i2c_write_blocking(i2c_port, Si5351_ADDR, data, 2, false);

    // enable output
    data[0] = 3;
    data[1] = 0x00;
    i2c_write_blocking(i2c_port, Si5351_ADDR, data, 2, false);
}

void si5351_set_clock(uint8_t bit, uint32_t freq)
{
    switch (freq)
    {
    case 44100:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_44100[0]);
    }
    break;

    case 48000:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_48000[0]);
    }
    break;

    case 88200:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_88200[0]);
    }
    break;

    case 96000:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_96000[0]);
    }
    break;

    case 176400:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_176400[0]);
    }
    break;

    case 192000:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_192000[0]);
    }
    break;

    case 352800:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_352800[0]);
    }
    break;

    case 384000:
    {
        si5351_send(SI5351A_REVB_REG_CONFIG_NUM_REGS, &si5351a_32_384000[0]);
    }
    break;

    default:
        break;
    }
}