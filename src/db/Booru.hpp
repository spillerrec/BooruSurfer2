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
#include "../objects/Note.hpp"
#include "../objects/Tag.hpp"
#include "../objects/Index.hpp"

#include <mutex>
#include <string>
#include <vector>

void resetDatabaseConnections();

class Booru{
	private:
		std::string site;
		
		void saveToDb( const Note& item );
		void saveToDb( const Tag&  item );
		void saveToDb( const Post& item );
		
		template<typename T>
		struct CacheItem{
			T value;
			bool saved;
			CacheItem( T value, bool saved ) : value(value), saved(saved) { }
		};
		
		template<typename T>
		class Cache{
			private:
				std::mutex lock;
				std::vector<CacheItem<T>> items;
				
			public:
				bool get( T& find ){
					std::lock_guard<std::mutex> locker( lock );
					for( auto& item : items )
						if( item.value.id == find.id ){
							find = item.value;
							return true;
						}
					return false;
				}
				
				void insert( T& item, bool saved ){
					std::lock_guard<std::mutex> locker( lock );
					items.emplace_back( item, saved );
				}
				
				void replace( const T& t, bool saved = false ){
					std::lock_guard<std::mutex> locker( lock );
					for( auto& item : items )
						if( item.value.id == t.id ){
							item.value = t;
							item.saved = saved;
						}
				}
				
				void flush( Booru& booru ){
					std::lock_guard<std::mutex> locker( lock );
					
					//Find all items to save
					std::vector<CacheItem<T>*> saved;
					for( auto& item : items )
						if( !item.saved )
							saved.emplace_back( &item );
					
					//Save items, but reduce transactions as much as possible
					if( saved.size() == 0 ) return;
					if( saved.size() == 1 ) booru.saveToDb( saved[0]->value );
					else{
						auto batch = booru.beginBatch();
						for( auto& cache : saved )
							booru.saveToDb( cache->value );
						batch.close();
					}
					
					//Commit successful, apply changes
					for( auto& cache : saved )
						cache->saved = true;
				}
		};
		
		Cache<Post> posts;
		Cache<Tag>  tags;
		Cache<Note> notes;
	
		Transaction beginBatch();
		
	public:
		Booru( std::string site );
		
		bool load( Note& n );
		bool load( Post& p );
		bool load( Post& p, Image::Size level );
		bool load( Tag& p );
		bool load( Index& index, IndexId id );
		
		void save( Note& n );
		void save( Post& p );
		void save( Tag& p );
		void save( Index& index );
		
		Index iteratePosts( IndexId id );
		
		std::vector<Tag> searchTags( std::string query );
		
		void flushNotes(){ notes.flush( *this ); }
		void flushPosts(){ posts.flush( *this ); }
		void flushTags (){ tags .flush( *this ); }
};

#endif

