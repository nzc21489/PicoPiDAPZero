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

#ifndef VERSION_PICOPIDAP_ZERO_H
#define VERSION_PICOPIDAP_ZERO_H

#include <string>
using namespace std;

static const string version_picopidap_zero = "         Version 0.2.3";

#if defined(DAC_CS4398)
static const string dac_picopidap_zero = "             CS4398";
#elif defined(DAC_Zero_HAT_DAC_CS4398)
static const string dac_picopidap_zero = "       Zero HAT DAC CS4398";
#elif defined(DAC_DacPlusPro)
static const string dac_picopidap_zero = "           DacPlusPro";
#else
static const string dac_picopidap_zero = "";
#endif

#endif // VERSION_PICOPIDAP_ZERO_H