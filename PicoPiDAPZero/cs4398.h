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

#ifndef CS4398
#define CS4398

#include "stdint.h"
#include <string>

using namespace std;

#ifdef DAC_CS4398
#define cs4398address 0b1001100
#define pmp_digital_filter "digital_filter_CS4398.txt"
#else
#define cs4398address 0b1001101
#define pmp_digital_filter "digital_filter_Zero_HAT_DAC_CS4398.txt"
#endif

#define vol_max 0b00000000
#define vol_min 0b11111111

#define digital_filter_num 2
static const uint8_t digital_filter_nums[2] = {
    0,
    1
};

static const string digital_filter_strs[2] = {
    "         fast roll off",
    "         slow roll off"
};

extern void cs4398_setup();
extern void cs4398_mute();
extern void cs4398_unmute();
extern void cs4398_set_FM(int sampling_rate);
extern void change_volume_cs4398(uint8_t vol);
extern void cs4398_change_digital_filter(int digital_filter);

#endif //CS4398