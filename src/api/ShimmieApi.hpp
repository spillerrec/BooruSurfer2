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

#ifndef SHIMMIE_API_H
#define SHIMMIE_API_H

#include "Api.hpp"
#include "../server/Server.hpp"

class ShimmieApi : public Api{
	protected:
		
		
	public:
		ShimmieApi( std::string short_hand="shimmie" ) : Api( short_hand ) { }
		
		virtual std::string get_name() const{ return "Shimmie Testbed"; }
		virtual std::string get_url() const{ return "http://shimmie.shishnet.org/v2/"; }
		
		
		Post get_post( unsigned id, Image::Size level );
		Index get_index( std::string search, int page, int limit=-1 );
		
		virtual ThemeColor main_color() override{ return {0xFF,0xFF,0xFF}; }
		virtual ThemeColor secondary_color() override{ return {0xFF,0x76,0x1C}; }
		
		virtual std::string original_post_url( unsigned id ) override{
			return get_url() + "post/view/" + std::to_string( id );
		}
		virtual std::string original_index_url( std::string search ) override{
			return get_url() + "post/list/" + Server::remove_reserved( Server::encode_str( search ) );
		}
};

class Rule34Api : public ShimmieApi{
	public:
		Rule34Api() : ShimmieApi( "rule34" ) { }
		
		std::string get_name() const override{ return "Rule 34"; }
		std::string get_url() const override{ return "http://rule34.paheal.net/"; }
		
		ThemeColor main_color() override{ return {0x00,0x43,0x08}; }
		ThemeColor secondary_color() override{ return {0xAC,0xE4,0xA3}; }
};

#endif

