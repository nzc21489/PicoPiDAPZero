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

#include "bmp_shinonome16.h"
#include "shinonome16_utf.h"

bmp_shinonome16::bmp_shinonome16()
{
    return;
}

bmp_shinonome16::~bmp_shinonome16()
{
}

array<char, 32> bmp_shinonome16::get_char(int num)
{
    array<char, 32> chara;
    for (int i = 0; i < 32; i++)
    {
        chara[i] = __font_bitmap__[16 * 16 / 8 * num + i];
    }
    return chara;
}

bool bmp_shinonome16::get_width(int num)
{
    int width = __font_widths__[num];
    if (width == 8){
        return false;
    }else{
        return true;
    }
}

uint16_t bmp_shinonome16::get_index(int num)
{
    int left = 0;                                                /* start key of index */
    int right = sizeof(__font_index__) / sizeof(unsigned short); /* end key of index */
    int mid;                                                     /* middle key of index */
    int value = num;                                             /* search value */

    while (left <= right)
    {
        mid = (left + right) / 2; /* calc of middle key */
        if (__font_index__[mid] == value)
        {
            return mid;
        }
        else if (__font_index__[mid] < value)
        {
            left = mid + 1; /* adjustment of left(start) key */
        }
        else
        {
            right = mid - 1; /* adjustment of right(end) key */
        }
    }
    return 0x20 - 1; // 0x20 = " "(space)  //63 = "?"
}

void bmp_shinonome16::bmp_shinonome16_get(string chara)
{
    const char *utf8_char = (&chara)->c_str(); //utf8_char_string.c_str();
    const char *it = utf8_char;
    const char *end = utf8_char + strlen(utf8_char);

    uint32_t code = utf8::next(it, end);

    character.fill(0);

    int num = get_index(code);
    character = get_char(num);
    width_is_16 = get_width(num);
}