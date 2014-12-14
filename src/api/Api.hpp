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

#ifndef API_H
#define API_H

#include <string>
#include <vector>
#include <utility>
#include "../objects/ResourceHandler.hpp"
#include "../objects/Post.hpp"
#include "../objects/Tag.hpp"
#include "../objects/Index.hpp"

class Api{
	protected:
		std::string site_name;
		Booru booru{ site_name };
		
	public:
		struct ThemeColor{
			int red, green, blue;
		};
		ResourceHandler<Tag,std::string> tag_handler{ booru };
		ResourceHandler<Post> post_handler{ booru };
		void flush();
	public:
		std::string get_from_url( std::string url
			,	std::vector<std::pair<std::string,std::string> > headers = std::vector<std::pair<std::string,std::string> >()
			) const; //temporary
		
	public:
		Api( std::string site_name ) : site_name(site_name) { }
		virtual std::string get_name() const = 0;
		virtual std::string get_shorthand() const{ return site_name; }
		virtual std::string get_url() const = 0;
		
		virtual Post get_post( unsigned id ) = 0;
		virtual Index get_index( std::string search, int page, int limit=-1 ) = 0;
		
		virtual ThemeColor main_color(){ return {255,255,255}; }
		virtual ThemeColor secondary_color(){ return {0,0,0}; }
		
		virtual std::string original_post_url( unsigned id ) = 0;
		virtual std::string original_index_url( std::string search ) = 0;
		
		virtual ~Api(){ }
};

#endif

