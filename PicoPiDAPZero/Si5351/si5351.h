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

#ifndef SI5351_H
#define SI5351_H

#include "pico/stdlib.h"
#include "stdint.h"

#ifdef EXT_CLK
#include "si5351_reg_ext_clk.h"
#elif defined(DAC_fpga)
#include "si5351_reg_fpga_dac.h"
#elif defined(DAC_fpga_ext_clk)
#include "si5351_reg_fpga_dac_ext_clk.h"
#else
#include "si5351_reg.h"
#endif

#include "hardware/i2c.h"

#define i2c_port i2c1
#define sda_pin 2
#define scl_pin 3

void setup_si5351_i2c();
void si5351_send(int reg_count, const si5351a_revb_register_t *si5351_regs);
void si5351_set_clock(uint8_t audio_bit, uint32_t audio_frequency);

#endif // SI5351_H