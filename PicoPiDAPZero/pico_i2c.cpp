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

#include "pico_i2c.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

void setup_i2c(uint8_t sda, uint8_t scl, i2c_inst_t i2c_port)
{
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
    bi_decl(bi_2pins_with_func(sda, scl, GPIO_FUNC_I2C));
    i2c_init(&i2c_port, 100000);
}

i2c_inst_t get_i2c_port(uint8_t sda, uint8_t scl)
{
    if ((sda % 4) == 0)
    {
        return i2c0_inst;
    }
    else
    {
        return i2c1_inst;
    }
}

int read_i2c(i2c_inst_t i2c_port, uint8_t i2c_address, uint8_t reg, uint8_t* data)
{
    int bw = i2c_write_blocking(&i2c_port, i2c_address, &reg, 1, true);
    if (bw != 1)
    {
        return bw;
    }
    int br = i2c_read_blocking(&i2c_port, i2c_address, data, 1, false);
    return br;
}

int write_i2c(i2c_inst_t i2c_port, uint8_t i2c_address, uint8_t reg, uint8_t value)
{
    uint8_t reg_data[2];
    reg_data[0] = reg;
    reg_data[1] = value;
    int bw = i2c_write_blocking(&i2c_port, i2c_address, &reg_data[0], 2, false);
    return bw;
}

int write_i2c_multi(i2c_inst_t i2c_port, uint8_t i2c_address, uint8_t *value, int num)
{
    int bw = i2c_write_blocking(&i2c_port, i2c_address, value, num, false);
    return bw;
}

vector<uint8_t> i2c_bus_scan(i2c_inst_t i2c_port)
{
    vector<uint8_t> i2c_devices;
    for (int addr = 0; addr < 128; addr++)
    {
        uint8_t read_data;
        if (((addr & 0x78) != 0) && ((addr & 0x78) != 0x78))
        {
            int ret = i2c_read_timeout_us(&i2c_port, addr, &read_data, 1, false, 1000);
            if (ret == 1)
            {
                i2c_devices.push_back(addr);
            }
        }
    }
    return i2c_devices;
}