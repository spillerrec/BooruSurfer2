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

#include <Poco/Mutex.h>

#include <string>
#include <vector>

class Booru{
	private:
		std::string site;
		
		template<typename T>
		struct Cache{
			T value;
			bool saved;
			Cache( T value, bool saved ) : value(value), saved(saved) { }
		};
		
		std::vector<Cache<Post>> postCache;
		std::vector<Cache<Tag>>   tagCache;
		
		bool getPost( Post& p );
		bool  getTag( Tag&  t );
		void insertPost( Post& p );
		void insertTag ( Tag&  t );
		
		Poco::Mutex postMutex;
		Poco::Mutex  tagMutex;
	
		Transaction beginBatch();
		
	public:
		Booru( std::string site );
		
		bool load( Post& p );
		bool load( Tag& p );
		
		void save( Post& p );
		void save( Tag& p );
		
		void flushPosts();
		void flushTags();
};

#endif

