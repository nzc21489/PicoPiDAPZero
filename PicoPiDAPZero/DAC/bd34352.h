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

#ifndef BD34352
#define BD34352

#include "stdint.h"
#include <string>
#include "hardware/i2c.h"
#include "dac.h"

using namespace std;

class bd34352 : public dac
{
private:
    static const uint8_t vol_max = 0b00000000;
    static const uint8_t vol_min = 0b11011100;
    uint8_t dac_address = 0x1C;
    uint32_t sampling_rate = 44100;
    uint8_t digital_filter = 0;
    i2c_inst_t dac_i2c_port = *i2c0;
    const string digital_filter_text = "digital_filter_BD34352.txt";
    const uint8_t digital_filter_num = 2;
    const uint8_t digital_filter_nums[2] = {
        0,
        1,
    };
    const string digital_filter_strs[2] = {
    "         Sharp roll-off",
    "         Slow roll-off"
    };

    uint8_t get_bck_divider(uint8_t bit, uint32_t freq);

public:
    void set_dac_address(uint8_t address);
    void set_i2c_port(i2c_inst_t i2c_port);
    void setup();
    void mute();
    void unmute();
    void set_bit_freq(uint8_t bit, uint32_t freq);
    bool set_volume(uint8_t vol);
    uint8_t get_digital_filter_num();
    string get_digital_filter_strs(uint8_t filter_num);
    void set_digital_filter(int filter_num);
    string get_digital_filter_text_name();
    dac_type get_dac();
    bd34352();
};

#endif // BD34352