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

#ifndef INDEX_HPP
#define INDEX_HPP

#include "Identity.hpp"
#include "Post.hpp"

#include <string>
#include <vector>

struct IndexId{
	std::string query;
	int page;
	int limit;
};

class Index : public Identity<IndexId>{
	public:
		int amount = -1; //Upper bound
		//Resource<Post> posts{ true };
		std::vector<Post> posts;
		
	public:
		Index( ID_T id ){ this->id = id; }
		
};

#endif

