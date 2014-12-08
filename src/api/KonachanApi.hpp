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

#ifndef KONACHAN_API_H
#define KONACHAN_API_H

#include "DanApi.hpp"

class KonachanApi : public DanApi{
	protected:
		static const char* const kona_post_strings[POST_ITEM_SIZE];
		
	public:
		KonachanApi( std::string short_hand="kona" ) : DanApi( short_hand ) { }
		
		virtual std::string get_name() const override{ return "Konachan"; }
		virtual std::string get_url() const override{ return "http://konachan.com/"; }
		
		const char* const* post_table() const{ return kona_post_strings; }
		
		virtual ThemeColor main_color() override{ return {0x71,0x57,0x5A}; }
		virtual ThemeColor secondary_color() override{ return {0xFF,0xEC,0xCE}; }
		
		virtual std::string original_post_url( unsigned id ) override{
			return get_url() + "post/show/" + std::to_string( id ) + "/";
		}
		virtual std::string original_index_url( std::string search ) override{
			return get_url() + "post/?tags=" + search;
		}
};

#endif

