/*	This file is part of BooruSurfer2.

	BooruSurfer2 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	BooruSurfer2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with BooruSurfer2.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIME_HANDLING_HPP
#define TIME_HANDLING_HPP

#include <string>

struct Time{
	private:
		int64_t value{ 0 };
		
		Time( int64_t value ) : value(value) { }
	
	public:
		Time() = default;
		
		//Construct a time from a 'YYYY-MM-DD hh:mm:ss' formatted string
		static Time FromSimpleFormat( std::string_view formatted_string );
		
		///Construct a time from a Unix time
		static Time FromUnixTime( int64_t time ){ return { time }; }
		
		int64_t toUnixTime() const{ return value; }
		
		bool isValid() const{ return value != 0; }
		
		std::string formatSimple() const;
		std::string formatRFC822() const;
		
		bool operator<( Time other ) const{ return value < other.value; }
		
		void addSeconds( int64_t seconds ){ value += seconds; }
};

std::string get_thumbnail( std::string path );

#endif

