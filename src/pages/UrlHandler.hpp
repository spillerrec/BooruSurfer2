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

#ifndef URL_HANDLER_H
#define URL_HANDLER_H

#include "../objects/Post.hpp"
#include <string>
#include <vector>

class Api;

class UrlHandler{
	private:
		Api* api;
	public:
		UrlHandler( Api* api ) : api(api) { }
		
		std::string post_url( const Post& p ) const;
		std::string index_url( const std::vector<Tag>& tags = std::vector<Tag>(), unsigned page=1, int amount = -1 ) const;
		
		std::string image_tags( const Post& p, int lenght );
		std::string image_url( const Post& p, Image::Size size );
};

#endif

