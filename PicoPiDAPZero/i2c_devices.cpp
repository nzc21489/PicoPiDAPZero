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

#include "i2c_devices.h"
#include "pico_i2c.h"
#include "version_picopidap_zero.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

string dac_data_string[12] = {
    "",
#ifdef ModelB
    "    Pico Pi DAP Zero Model B",
#else
    "        Pico Pi DAP Zero",
#endif
    "            I2S DAC",
    "",
    version_picopidap_zero,
    "",
    "",
    "",
    "", // I2C_DAC : Address
    "", // Si5351A : Address
    "", // MachXO2 : Address
    ""  // Other : Address (Address ...)
};

bool mach_xo2 = false;

void get_i2c_devices(i2c_inst_t i2c_port, uint8_t* i2c_device_data, bool i2c_bus_internal)
{
    if (mach_xo2)
    {
        return;
    }
    
    vector<uint8_t> i2c_devices = i2c_bus_scan(i2c_port);

    for (int i = 0; i < 3; i++)
    {
        i2c_device_data[i] = 0;
    }
    uint8_t devices = 0;

    bool device_other = false;
    char str_addr_hex[10];

    for (int i = 0; i < i2c_devices.size(); i++)
    {
        if (i2c_devices[i] == 0x60) // Si5351A
        {

            i2c_device_data[0] = 1;

            if (i2c_bus_internal)
            {
                dac_data_string[9] = "    Si5351A Internal : ";
            }
            else
            {
                dac_data_string[9] = "    Si5351A External : ";
            }
            sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
            dac_data_string[9] += str_addr_hex;
        }
        else if (i2c_devices[i] >= 0x10 && i2c_devices[i] <= 0x13) // AK449X
        {
            i2c_device_data[1] = dac_ak449x;
            i2c_device_data[2] = i2c_devices[i];
            dac_data_string[2] = "             AK449X";
            dac_data_string[8] = "         AK449X : ";
            sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
            dac_data_string[8] += str_addr_hex;
        }
        else if (i2c_devices[i] >= 0x1C && i2c_devices[i] <= 0x1F) // BD34352
        {
            i2c_device_data[1] = dac_bd34352;
            i2c_device_data[2] = i2c_devices[i];
            dac_data_string[2] = "             BD34352";
            dac_data_string[8] = "         BD34352 : ";
            sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
            dac_data_string[8] += str_addr_hex;
        }
        else if (i2c_devices[i] >= 0x4c && i2c_devices[i] <= 0x4f) // PCM512X or PCM1795 or CS4398
        {
            uint8_t data_tmp;
            read_i2c(i2c_port, i2c_devices[i], 1, &data_tmp);
            data_tmp  = (data_tmp >> 3);
            if (data_tmp == 0b01110)
            {
                i2c_device_data[1] = dac_cs4398;
                i2c_device_data[2] = i2c_devices[i];
                dac_data_string[2] = "             CS4398";
                dac_data_string[8] = "         CS4398 : ";
                sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
                dac_data_string[8] += str_addr_hex;
            }
            else
            {
                read_i2c(i2c_port, i2c_devices[i], 16, &data_tmp);
                if (data_tmp == 0xff)
                {
                    i2c_device_data[1] = dac_pcm1795;
                    i2c_device_data[2] = i2c_devices[i];
                    dac_data_string[2] = "             PCM1795";
                    dac_data_string[8] = "         PCM1795 : ";
                    sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
                    dac_data_string[8] += str_addr_hex;
                }
                else
                {
                    if (i2c_devices[i] == 0x4D)
                    {
                        i2c_device_data[1] = dac_DacPlusPro;
                        i2c_device_data[2] = i2c_devices[i];
                        dac_data_string[2] = "           DacPlusPro";
                        dac_data_string[8] = "         PCM512X : ";
                        sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
                        dac_data_string[8] += str_addr_hex;
                    }
                    else
                    {
                        i2c_device_data[1] = dac_pcm512x;
                        i2c_device_data[2] = i2c_devices[i];
                        dac_data_string[2] = "             PCM512X";
                        dac_data_string[8] = "          PCM512X : ";
                        sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
                        dac_data_string[8] += str_addr_hex;
                    }
                }
            }
            
        }
        else if (i2c_devices[i] == 0x40 || i2c_devices[i] == 0x41 || i2c_devices[i] == 0x43) // MachXO2
        {
            if (!mach_xo2)
            {
                mach_xo2 = true;
                dac_data_string[10] = "   MachXO2 :";
            }
            dac_data_string[10] += " ";
            sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
            dac_data_string[10] += str_addr_hex;
        }
        else
        {
            if (!device_other)
            {
                device_other = true;
                dac_data_string[11] = "     Other :";
            }
            dac_data_string[11] += " ";
            sprintf(str_addr_hex, "0x%x", i2c_devices[i]);
            dac_data_string[11] += str_addr_hex;
        }
    }
}