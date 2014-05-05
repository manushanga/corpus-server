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

#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <ctime>

class Timer
{
private:
	struct timespec tstart, tstop;
public:
    inline Timer()
	{
		clock_gettime( CLOCK_REALTIME, &tstart);
	};
	inline double stop()
	{
		clock_gettime( CLOCK_REALTIME, &tstop);
		return ( tstop.tv_sec - tstart.tv_sec )
             + (double)( tstop.tv_nsec - tstart.tv_nsec )
               / (double)1000000000L;
	};
	
    ~Timer()
	{
		
	};
};

#endif // TIMER_H
