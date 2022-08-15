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
#include "si5351_config.h"
#include "stdint.h"
#include <algorithm>
using namespace std;

#define Si5351A_address 0x60

void setup_si5351_i2c()
{

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    bi_decl(bi_2pins_with_func(sda_pin, scl_pin, GPIO_FUNC_I2C));
    i2c_init(i2c_port, 100000);
}

int write_i2c_si5351a(uint8_t reg, uint8_t value)
{
    uint8_t reg_data[2];
    reg_data[0] = reg;
    reg_data[1] = value;
    int bw = i2c_write_blocking(i2c_port, Si5351A_address, &reg_data[0], 2, false);
    return bw;
}

int read_i2c_si5351a(uint8_t reg, uint8_t *data)
{
    i2c_write_blocking(i2c_port, Si5351A_address, &reg, 1, true);
    int br = i2c_read_blocking(i2c_port, Si5351A_address, data, 1, false);
    return br;
}

int gcd(long long A, long long B)
{
    while (A != B)
    {
        if (A > B)
            A -= B;
        else
            B -= A;
    }
    return B;
}

bool si5351_set_clock(int clock0, int clock1, int clock2)
{
    uint8_t clock_invertion[3] = {0};
    if (clock0 < 0)
    {
        clock_invertion[0] = 0x10;
        clock0 *= -1;
    }
    if (clock1 < 0)
    {
        clock_invertion[1] = 0x10;
        clock1 *= -1;
    }
    if (clock2 < 0)
    {
        clock_invertion[2] = 0x10;
        clock2 *= -1;
    }

    int fastest_out_clock = max({clock0, clock1, clock2});

    uint8_t clock_disable = 0;
    uint8_t clock_disable_state = 0;
    int clock_div[3] = {1, 1, 1};
    if (clock0 == 0)
    {
        clock_disable |= 0b00000001;
        clock_disable_state |= 0b00000010;
    }
    else
    {
        clock_div[0] = fastest_out_clock / clock0;
    }
    if (clock1 == 0)
    {
        clock_disable |= 0b00000010;
        clock_disable_state |= 0b00001000;
    }
    else
    {
        clock_div[1] = fastest_out_clock / clock1;
    }
    if (clock2 == 0)
    {
        clock_disable |= 0b00000100;
        clock_disable_state |= 0b00100000;
    }
    else
    {
        clock_div[2] = fastest_out_clock / clock2;
    }

    uint8_t reg0;
    read_i2c_si5351a(0, &reg0);
    int max_fvco = 900 * 1000 * 1000; // Hz
    if ((reg0 & 0b11) == 0)           // reg0[1:0] : REVID, REVID = 0 : Si5351A-A
    {
        max_fvco = 720 * 1000 * 1000;
    }

    // fout = fvco / (div_a + div_b/div_c)
    // div_b = 1, div_c = 0;
    int div_a = max_fvco / fastest_out_clock;
    if ((div_a % 2) == 1) // div_a with even integer is preferred
    {
        div_a--;
    }
    int fvco_freq = fastest_out_clock * div_a;

    // fractional rate
    // fvco = xtal_clock * (pll_a + pll_b/pll_c)
    int pll_a, pll_b, pll_c;

    pll_a = fvco_freq / xtal_clock;
    int pll_fractional = fvco_freq % xtal_clock;

    long long pll_b_tmp = 0;
    long long pll_c_tmp = xtal_clock / (1000 * 1000);

    while (1)
    {
        long long tmp = (long)fvco_freq * pll_c_tmp;
        if ((tmp % xtal_clock) == 0)
        {
            pll_b_tmp = (long long)(tmp / xtal_clock - pll_a * pll_c_tmp);
            break;
        }
        pll_c_tmp *= 10;
    }

    int gcd_bc = gcd(pll_b_tmp, pll_c_tmp);
    pll_b = pll_b_tmp / gcd_bc;
    pll_c = pll_c_tmp / gcd_bc;

    int MSNA_P1 = 128 * pll_a + (int)(128 * pll_b / pll_c) - 512;
    int MSNA_P2 = 128 * pll_b - pll_c * (int)(128 * pll_b / pll_c);
    int MSNA_P3 = pll_c;

    uint8_t R[3] = {0, 0, 0};
    int MS0_P1 = 128 * div_a * clock_div[0] - 512;
    int div_2x = 1;
    while ((MS0_P1 > (1 << 16)) || (div_a * clock_div[0] / div_2x > 254))
    {
        R[0]++;
        div_2x *= 2;
        MS0_P1 = 128 * div_a * clock_div[0] / div_2x - 512;
    }
    int MS1_P1 = 128 * div_a * clock_div[1] - 512;
    div_2x = 1;
    while ((MS1_P1 > (1 << 16)) || (div_a * clock_div[1] / div_2x > 254))
    {
        R[1]++;
        div_2x *= 2;
        MS1_P1 = 128 * div_a * clock_div[1] / div_2x - 512;
    }
    int MS2_P1 = 128 * div_a * clock_div[2] - 512;
    div_2x = 1;
    while ((MS2_P1 > (1 << 16)) || (div_a * clock_div[2] / div_2x > 254))
    {
        R[2]++;
        div_2x *= 2;
        MS2_P1 = 128 * div_a * clock_div[2] / div_2x - 512;
    }

    uint8_t xtal_cl;
    if (load_capacitance == 6)
    {
        xtal_cl = 0b01010010;
    }
    else if (load_capacitance == 8)
    {
        xtal_cl = 0b10010010;
    }
    else if (load_capacitance == 10)
    {
        xtal_cl = 0b11010010;
    }
    else
    {
        xtal_cl = 0b11010010;
    }

    // disable output
    write_i2c_si5351a(3, 0xff);

    // power down output drivers
    write_i2c_si5351a(16, 0x80);
    write_i2c_si5351a(17, 0x80);
    write_i2c_si5351a(18, 0x80);

    // write settings
    write_i2c_si5351a(2, 0x53);                                                  // Interrupt Status Mask
    write_i2c_si5351a(16, (0x4f | clock_invertion[0]));                          // clock invertion, Select MultiSynth 0 as the source for CLK0, Drive Strength 8mA
    write_i2c_si5351a(17, (0x4f | clock_invertion[1]));                          // clock invertion, Select MultiSynth 1 as the source for CLK1, Drive Strength 8mA
    write_i2c_si5351a(18, (0x4f | clock_invertion[2]));                          // clock invertion, Select MultiSynth 2 as the source for CLK2, Drive Strength 8mA
    write_i2c_si5351a(19, 0x80);                                                 // clk3 power down
    write_i2c_si5351a(20, 0x80);                                                 // clk4 power down
    write_i2c_si5351a(21, 0x80);                                                 // clk5 power down
    write_i2c_si5351a(22, 0x80);                                                 // clk6 power down
    write_i2c_si5351a(23, 0x80);                                                 // clk7 power down
    write_i2c_si5351a(24, clock_disable_state);                                  // clock disable
    write_i2c_si5351a(26, (MSNA_P3 >> 8) & 0xff);                                // MSNA_P3[15:8]
    write_i2c_si5351a(27, (MSNA_P3 & 0xff));                                     // MSNA_P3[7:0]
    write_i2c_si5351a(28, (MSNA_P1 >> 16) & 0x03);                               // MSNA_P1[17:16]
    write_i2c_si5351a(29, ((MSNA_P1 >> 8) & 0xff));                              // MSNA_P1[15:8]
    write_i2c_si5351a(30, (MSNA_P1 & 0xff));                                     // MSNA_P1[7:0]
    write_i2c_si5351a(31, (((MSNA_P3 >> 12) & 0xf0) | ((MSNA_P2 >> 16) & 0xf))); // MSNA_P3[19:16], MSNA_P2[19:16]
    write_i2c_si5351a(32, (MSNA_P2 >> 8) & 0xff);                                // MSNA_P2[15:8]
    write_i2c_si5351a(33, (MSNA_P2 & 0xff));                                     // MSNA_P2[7:0]
    write_i2c_si5351a(34, (MSNA_P3 >> 8) & 0xff);                                // MSNB_P3[15:8]
    write_i2c_si5351a(35, (MSNA_P3 & 0xff));                                     // MSNB_P3[7:0]
    write_i2c_si5351a(36, (MSNA_P1 >> 16) & 0x03);                               // MSNB_P1[17:16]
    write_i2c_si5351a(37, ((MSNA_P1 >> 8) & 0xff));                              // MSNB_P1[15:8]
    write_i2c_si5351a(38, (MSNA_P1 & 0xff));                                     // MSNB_P1[7:0]
    write_i2c_si5351a(39, (((MSNA_P3 >> 12) & 0xf0) | ((MSNA_P2 >> 16) & 0xf))); // MSNB_P3[19:16], MSNA_P2[19:16]
    write_i2c_si5351a(40, (MSNA_P2 >> 8) & 0xff);                                // MSNB_P2[15:8]
    write_i2c_si5351a(41, (MSNA_P2 & 0xff));                                     // MSNB_P2[7:0]
    write_i2c_si5351a(42, 0);                                                    // MS0_P3[15:8]
    write_i2c_si5351a(43, 1);                                                    // MS0_P3[7:0]
    write_i2c_si5351a(44, (R[0] << 4) | ((MS0_P1 >> 16) & 0x03));                // R0_DIV[2:0], MS0_DIVBY4[1:0], MS0_P1[17:16]
    write_i2c_si5351a(45, (MS0_P1 >> 8) & 0xff);                                 // MS0_P1[15:8]
    write_i2c_si5351a(46, MS0_P1 & 0xff);                                        // MS0_P1[7:0]
    write_i2c_si5351a(47, 0);                                                    // MS0_P3[19:16], MS0_P2[19:16]
    write_i2c_si5351a(48, 0);                                                    // MS0_P2[15:8]
    write_i2c_si5351a(49, 0);                                                    // MS0_P2[7:0]
    write_i2c_si5351a(50, 0);                                                    // MS1_P3[15:8]
    write_i2c_si5351a(51, 1);                                                    // MS1_P3[7:0]
    write_i2c_si5351a(52, (R[1] << 4) | ((MS1_P1 >> 16) & 0x03));                // R1_DIV[2:0], MS1_DIVBY4[1:0], MS1_P1[17:16]
    write_i2c_si5351a(53, (MS1_P1 >> 8) & 0xff);                                 // MS1_P1[15:8]
    write_i2c_si5351a(54, MS1_P1 & 0xff);                                        // MS1_P1[7:0]
    write_i2c_si5351a(55, 0);                                                    // MS1_P3[19:16], MS1_P2[19:16]
    write_i2c_si5351a(56, 0);                                                    // MS1_P2[15:8]
    write_i2c_si5351a(57, 0);                                                    // MS1_P2[7:0]
    write_i2c_si5351a(58, 0);                                                    // MS2_P3[15:8]
    write_i2c_si5351a(59, 1);                                                    // MS2_P3[7:0]
    write_i2c_si5351a(60, (R[2] << 4) | ((MS2_P1 >> 16) & 0x03));                // R2_DIV[2:0], MS2_DIVBY4[1:0], MS2_P1[17:16]
    write_i2c_si5351a(61, (MS2_P1 >> 8) & 0xff);                                 // MS2_P1[15:8]
    write_i2c_si5351a(62, MS2_P1 & 0xff);                                        // MS2_P1[7:0]
    write_i2c_si5351a(63, 0);                                                    // MS2_P3[19:16], MS2_P2[19:16]
    write_i2c_si5351a(64, 0);                                                    // MS2_P2[15:8]
    write_i2c_si5351a(65, 0);                                                    // MS2_P2[7:0]
    write_i2c_si5351a(149, 0);                                                   // Spread Spectrum Disable
    write_i2c_si5351a(183, xtal_cl);                                             // Crystal Load Capacitance Selection

    // soft reset
    write_i2c_si5351a(177, 0xac);

    // enable output
    write_i2c_si5351a(3, clock_disable); // Output Enable Control

    return true;
}