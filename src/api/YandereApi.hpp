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

#ifndef YANDERE_API_H
#define YANDERE_API_H

#include "KonachanApi.hpp"

class YandereApi : public KonachanApi{
	public:
		YandereApi( std::string short_hand="yandere" ) : KonachanApi( short_hand ) { }
		
		virtual std::string get_name() const override{ return "Yandere"; }
		virtual std::string get_url() const override{ return "https://yande.re/"; }
		
		virtual ThemeColor main_color() override{ return {0xFA,0xB0,0xB1}; }
		virtual ThemeColor secondary_color() override{ return {0xFF,0xF3,0xE4}; }
};

#endif

