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
#include "../objects/Post.hpp"

class Api{
	protected:
		std::string get_from_url( std::string url ) const; //temporary
		
	public:
		virtual std::string get_name() const = 0;
		virtual std::string get_shorthand() const = 0;
		
		virtual Post get_post( unsigned id ) const = 0;
		virtual std::vector<Post> get_index( std::string search, int page, int limit=-1 ) const = 0;
		
		virtual ~Api(){ }
};

#endif

