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

#include "filereader.h"
#include "logger.h"
FileReader::FileReader(const char *filename, FileUtil::FILEMODE mode)
{
	char smode[32];
    currWord = 256;
	std::sprintf(smode,"r,ccs=%s",FileUtil::getFilemode(mode));
    f=std::fopen(filename,smode);
    fill=true;
}

void FileReader::getWord(std::wstring& word)
{
    if (fill) {
        std::fwscanf(f,L" %255ls",buffer);
        currWord =0;
        fill=false;
    }
    wchar_t neww[256];
    neww[0]=L'\0';
	int i=0;
    while (buffer[currWord] != L'\0')
	{
		wchar_t c=buffer[currWord];


		int h=iswsymbol(c);
        if ( !h )
		{
			neww[i]=c;
            i++;
            currWord++;
        } else {
            if (i==0){
                if (h){
                    neww[i]=c;
                    i++;
                    currWord++;
                    break;
                }
            } else
                if (!h)
                    currWord++;
                break;
        }
	}
    if (buffer[currWord]==L'\0')
        fill=true;
    neww[i]=L'\0';
    if (neww[0]==L'\0'){
        neww[0]=L'%';
        neww[1]=L'\0';
    }
    //LOG(neww);
	word.assign(neww);

}

FileReader::~FileReader()
{
	std::fclose(f);
}
