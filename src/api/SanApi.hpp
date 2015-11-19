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
#include "../Server.hpp"

class SanApi : public Api{
	protected:
		
		
	public:
		SanApi( std::string short_hand="san" ) : Api( short_hand ) { }
		
		virtual std::string get_name() const{ return "Sankaku-chan"; }
		virtual std::string get_url() const{ return "https://chan.sankakucomplex.com/"; }
		
		
		Post get_post( unsigned id, Image::Size level );
		Index get_index( std::string search, int page, int limit=-1 );
		
		virtual ThemeColor main_color() override{ return {0xFF,0xFF,0xFF}; }
		virtual ThemeColor secondary_color() override{ return {0xFF,0x76,0x1C}; }
		
		virtual std::string original_post_url( unsigned id ) override{
			return get_url() + "post/show/" + std::to_string( id );
		}
		virtual std::string original_index_url( std::string search ) override{
			return get_url() + "?tags=" + Server::remove_reserved( Server::encode_str( search ) );
		}
};

#endif

