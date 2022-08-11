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

#ifndef AK449X
#define AK449X

#include "stdint.h"
#include <string>

using namespace std;

#define ak449x_address 0x10
#define vol_min 0b00000000
#define vol_max 0b11111111

#define pmp_digital_filter "AK449X.txt"

#define digital_filter_num 6

static const uint8_t digital_filter_nums[6] = {
    0,
    1,
    2,
    3,
    4,
    5
};

static const string digital_filter_strs[6] = {
    "         Sharp roll-off",
    "         Slow roll-off",
    "  hort delay sharp roll-off",
    "  Short delay slow roll-off",
    "     Super Slow roll-off",
    "  Low dispersion Short delay",
};

extern void ak449x_setup();
extern void change_volume_ak449x(uint8_t vol);
extern void ak449x_change_digital_filter(int digital_filter);

#endif //AK449X 
