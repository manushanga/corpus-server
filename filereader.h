/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  Madura Anushanga <madura.x86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FILEREADER_H
#define FILEREADER_H

#include <cstdio>
#include <cwchar>
#include <string>

#include <iostream>
#include <cerrno>
#include <cstring>

#include "fileutil.h"

class FileReader
{
private:
	FILE *f;
	wchar_t buffer[256];
    unsigned currWord;
    bool fill;
public:
    FileReader(const char *filename, FileUtil::FILEMODE mode);
    inline int iswwhite(wchar_t c)
    {
        return c==L'\n' || c== L'\r' || c==L'\t' || c==L' ' ;/*||
                c==0x000B || c==0x000C || c==0x0085 || c==0x2028 ||
                c==0x0085 || c==0x2089 || c==0x2029 || c==0x2000;*/
    }
    // http://en.wikipedia.org/wiki/Unicode#Abstract_characters
	inline int iswsymbol(wchar_t c) 
    {   wchar_t d=c & 0xFF00;
        wchar_t e=c & 0xFFF0;
        return   d == 0x2200 || d==0x2100 || d==0x2300 || (c!=0x0020 && e==0x0020) || e==0x0030 ||
                (c>=0x2013 && c<=0x201E) || (c>=0x2020 && c<=0x2022) ||
                c==0x2026 || c==0x2030 || c==0x2032 || c==0x2033 ||
                (c>=0x2039 && c<=0x203A) || c==0x203C || c==0x203E ||
                c==0x2044 || c==0x204A || c==0x20A7 || c==0x20AC ||
                c==0x20AF ;
	}
	inline bool isEOF() { return (bool)std::feof(f); }
	void getWord(std::wstring& word);
	inline FILE *ptr() { return f; }
    ~FileReader();
};

#endif // FILEREADER_H
