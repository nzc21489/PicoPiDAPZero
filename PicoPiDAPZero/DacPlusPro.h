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

#ifndef DACPLUSPRO
#define DACPLUSPRO

#include "stdint.h"
#include <string>

using namespace std;

#define DacPlusPro_address 0x4D
#define vol_max 0b00110000
#define vol_min 0b11111111

#define pmp_digital_filter "PCM512X.txt"

#define digital_filter_num 4
extern uint8_t DacPlusPro_filter;
static const uint8_t digital_filter_nums[4] = {
    1,
    2,
    3,
    7
};

static const string digital_filter_strs[4] = {
    "       FIR interpolation",
    "        Low latency IIR",
    "       High attenuation",
    " Ringing-less low latency FIR"
};

extern void DacPlusPro_setup();
// extern void DacPlusPro_mute();
// extern void DacPlusPro_unmute();
extern void DacPlusPro_change_bit_freq(uint8_t bit, uint32_t freq);
extern void change_volume_DacPlusPro(uint8_t vol);
extern void DacPlusPro_change_digital_filter(int digital_filter);

#endif //DACPLUSPRO