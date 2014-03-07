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

#ifndef SAN_API_H
#define SAN_API_H

#include "Api.hpp"

class SanApi : public Api{
	protected:
		
		
	public:
		virtual std::string get_name() const{ return "Sankaku-chan"; }
		virtual std::string get_shorthand() const{ return "san"; }
		virtual std::string get_url() const{ return "http://chan.sankakucomplex.com/"; }
		
		
		Post get_post( unsigned id );
		Index get_index( std::string search, int page, int limit=-1 );
		
		virtual ThemeColor main_color() override{ return {0xFF,0xFF,0xFF}; }
		virtual ThemeColor secondary_color() override{ return {0xFF,0x76,0x1C}; }
};

#endif

