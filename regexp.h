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

#ifndef REGEXP_H
#define REGEXP_H

#include <cstring>
#include <string>
#include <iostream>
class RegExp
{
public:
	static int validate_post(const char *text, std::string& query, int& lc, int& lr, char instance){
		register int i=0,ok=1, state=0;
		int len=std::strlen(text)-3;
		std::string q;
		//std::cout<<"<p>";
		while (i<len && ok) {
			//std::cout<<text[i];
			switch (state)
			{
				case 0:
					if (text[i] == instance && text[i+1]=='=' ) {
						state=1;
						i+=2;
					} else {
						ok = 0;
					}
					break;
				case 1: // gather query

					if (text[i] == 'l' && text[i+1] == 'c' && text[i+2] == '=') {
						state=2;
						query = q;
						lc=text[i+3]-'0';
						i+=6;
					} else {
						q.push_back(text[i]);
						i++;
					}
					break;
				case 2: // gathered lc
					//std::cout<<"ss"<<text[i];
					if (text[i] =='l' && text[i+1] == 'r' && text[i+2] == '=') {
						state=3;
						lr=text[i+3]-'0';
						i+=6;
					} else {
						ok=0;
					}
					break;
				case 3: // gathered lr
					if (isspace(text[i]))
						i++;
					else 
						ok=0;
					break;
				default:
					ok=0;
			}
		}
		//std::cout<<"</p>\n";
		return ok;
	}
	static int check(wchar_t *text)
	{
		register int ok=1, state=0;
		while (*text != L'\0' && ok)
		{
			switch (state)
			{
			case 0: // initial, must be a char or escape
				if (*text==L'\\')
				{
					state=2;
					text++;
				}
				else if (*text==L'*' || *text==L'?')
				{
					ok=0;
				}
				else
				{
					state=1;
					text++;
				}
				break;
			case 1: // char & op check
				if (*text==L'\\')
				{
					state=2;
					text++;
				}
				else if (*text==L'*' || *text==L'?')
				{
					state=3;
					text++;
				}
				else
				{
					text++;
				}
				break;

			case 2: // escape
				if (*text==L'*' || *text==L'?'){
					text++;
					state=1;
				} else {
					ok=0;
				}
				break;
			case 3: // op
				if (*text==L'*' || *text==L'?') {
					ok=0;
				} else {
					text++;
					state=1;
				}
				break;
			}
		}
		return ok;
	}
	
	static int match(wchar_t *_regexp, wchar_t *_text)
	{
		register int ok=1;
		register wchar_t *regexp=_regexp,*text=_text;

		while (*text!=L'\0' && *regexp!=L'\0' && ok)
		{
			if (*regexp == L'\\' && *text == regexp[1])
			{
				text++;
				regexp+=2;
			}
			else if (regexp[1] == L'*')
			{
				regexp+=2;
				while (regexp[1] == L'*' && *regexp!=L'\0' ){
					regexp+=2;
				}
				while (*text!=L'\0'){
					if (match(regexp,text))
						return 1;
					text++;
				}
			}
			else if (regexp[1] == L'?')
			{
				regexp+=2;
				if (match(regexp,text)){
					return 1;
				}
				text++;

				if (match(regexp,text)){
					return 1;
				}
			}
			else if (*regexp == L'.' || *text == *regexp)
			{
				text++;
				regexp++;
			}
			else
			{
				// error, end here
				ok = 0;

			}
		}
		if (*regexp!=L'\0' && *text==L'\0')
		{
			if (regexp[1] == L'?' || regexp[1] == L'*'){
				regexp+=2;
			}
		}
		return  *text == L'\0' && *regexp==L'\0' && ok;
	}
};

#endif // REGEXP_H
