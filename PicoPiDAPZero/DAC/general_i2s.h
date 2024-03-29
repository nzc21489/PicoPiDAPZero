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

#ifndef GENERAL_I2S
#define GENERAL_I2S

#include "stdint.h"
#include <string>
#include "hardware/i2c.h"
#include "dac.h"

using namespace std;

class general_i2s : public dac
{
private:

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
    general_i2s();
};

#endif //GENERAL_I2S
