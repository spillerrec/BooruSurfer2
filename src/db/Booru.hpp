/*	This file is part of rss2rss.

	rss2rss is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	rss2rss is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with rss2rss.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BOORU_HPP
#define BOORU_HPP

#include "Database.hpp"
#include "Statement.hpp"

#include "../objects/Post.hpp"
#include "../objects/Tag.hpp"

#include <string>

class Booru{
	private:
		Database db;
		std::string site;
		
		Statement* load_tags{ nullptr };
		Statement* save_tags{ nullptr };
		
		Statement* load_posts{ nullptr };
		Statement* save_posts{ nullptr };
		
	private:
		Statement& loadTags();
		Statement& saveTags();
		
		Statement& loadPosts();
		Statement& savePosts();
	
	public:
		Booru( std::string site );
		~Booru();
		
		Transaction beginBatch(){ return db; }
		
		bool load( Post& p );
		bool load( Tag& p );
		
		void save( Post& p );
		void save( Tag& p );
};

#endif

