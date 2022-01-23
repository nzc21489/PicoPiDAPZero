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

#include "stdio.h"
#include <array>
#include <vector>
#include "getDirectoryList.h"
#include "split.h"
#include <algorithm>
using namespace std;

void getDirectoryList::update()
{
    name_list.resize(0);
    is_dir.resize(0);
    jpeg_file = "";
    bool first = true;

    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    res = f_opendir(&dir, path.c_str());

    vector<string> file_dir;
    vector<string> file_not_dir;
    if (res == FR_OK)
    {
        while (true)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
            {
                break; /* Break on error or end of dir */
            }
            if (fno.fattrib & AM_DIR)
            { /* It is a directory */
                file_dir.push_back(fno.fname);
            }
            else
            { /* It is a file. */
                file_not_dir.push_back(fno.fname);
            }
        }
    }
    sort(file_dir.begin(), file_dir.end());
    sort(file_not_dir.begin(), file_not_dir.end());

    for (int i = 0; i < file_dir.size(); i++)
    {
        if (file_dir[i].find(".") == 0)
        {
            // remove directory start with "."
        }
        else
        {
            name_list.push_back(file_dir[i]);
            is_dir.push_back(1);
        }
    }
    for (int i = 0; i < file_not_dir.size(); i++)
    {
        string flac = ".flac";
        string opus = ".opus";
        string wav = ".wav";
        string mp3 = ".mp3";
        string aac = ".aac";
        if ((file_not_dir[i].size() >= flac.size() &&
             file_not_dir[i].find(flac, file_not_dir[i].size() - flac.size()) != std::string::npos) ||
            (file_not_dir[i].size() >= opus.size() &&
             file_not_dir[i].find(opus, file_not_dir[i].size() - opus.size()) != std::string::npos) ||
            (file_not_dir[i].size() >= wav.size() &&
             file_not_dir[i].find(wav, file_not_dir[i].size() - wav.size()) != std::string::npos) ||
            (file_not_dir[i].size() >= mp3.size() &&
             file_not_dir[i].find(mp3, file_not_dir[i].size() - mp3.size()) != std::string::npos) ||
            (file_not_dir[i].size() >= aac.size() &&
             file_not_dir[i].find(aac, file_not_dir[i].size() - aac.size()) != std::string::npos))
        {

            name_list.push_back(file_not_dir[i]);
            is_dir.push_back(0);
        }

        string jpeg = ".jpeg";
        string jpg = ".jpg";
        if ((file_not_dir[i].size() >= jpeg.size() &&
             file_not_dir[i].find(jpeg, file_not_dir[i].size() - jpeg.size()) != std::string::npos) ||
            (file_not_dir[i].size() >= jpg.size() &&
             file_not_dir[i].find(jpg, file_not_dir[i].size() - jpg.size()) != std::string::npos))
        {
            if (first){
                jpeg_file = file_not_dir[i];
                first = false;
            }
        }
    }

    f_closedir(&dir);
}

vector<string> getDirectoryList::get_dir_name_list()
{
    if (path != path_pre)
    {
        update();
    }
    path_pre = path;
    return name_list;
}

vector<int> getDirectoryList::get_is_dir()
{
    if (path != path_pre)
    {
        update();
    }
    path_pre = path;
    return is_dir;
}