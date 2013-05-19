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

#ifndef DAN_API_H
#define DAN_API_H

#include "Api.hpp"

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "../parsing/DataNode.hpp"

class DanApi : public Api{
	protected:
		enum PostItem{
			POST_ID,
			HASH,
			AUTHOR,
			CREATED,
			PARENT_ID,
			CHILDREN,
			NOTES,
			COMMENTS,
			POOLS,
			SOURCE,
			TAGS,
			SCORE,
			RATING,
			STATUS,
			
			URL,
			WIDTH,
			HEIGHT,
			SIZE,
			
			THUMB_URL,
			THUMB_WIDTH,
			THUMB_HEIGHT,
			THUMB_SIZE,
			
			PREVIEW_URL,
			PREVIEW_WIDTH,
			PREVIEW_HEIGHT,
			PREVIEW_SIZE,
			
			REDUCED_URL,
			REDUCED_WIDTH,
			REDUCED_HEIGHT,
			REDUCED_SIZE,
			
			POST_ITEM_SIZE
		};
		
		static const char* const post_strings[POST_ITEM_SIZE];
		
		template<class T>
		std::vector<T> convert_vector( std::vector<std::string> list ) const{
			std::vector<T> converted;
			converted.reserve( list.size() );
			for( std::string s : list )
				converted.push_back( boost::lexical_cast<T>( s ) );
			return converted;
		}
		
		template<class T1, class T2 = unsigned>
		Resource<T1,T2> get_resource( DataNode parent, PostItem resource ) const{
			const char* const name = post_table()[ resource ];
			if( name == NULL )
				return Resource<T1,T2>( false );
			
			DataNode node = parent[ name ];
			if( !node )
				return Resource<T1,T2>( false );
			
			if( node.as_boolean() )
				return Resource<T1,T2>( true );
			
			if( !node.as_string().empty() ){
				//TODO: split string
				
				//Split and remove empty
				std::string text = node.as_string();
				std::vector<std::string> list;
				boost::split( list, text, boost::is_any_of( " " ) ); //TODO: avoid is_any_of() ?
				list.erase( remove_if( list.begin(), list.end(), [](std::string arg){ return arg.empty(); } ), list.end() );
				
				return Resource<T1,T2>( convert_vector<T2>( list ) );
			}
			
			return Resource<T1,T2>( false );
		}
		
		virtual Post parse_post( DataNode node ) const;
		virtual Image get_image( DataNode parent, PostItem url, PostItem width, PostItem height, PostItem size ) const;
		
	public:
		std::string get_name() const{ return "Danbooru"; }
		std::string get_shorthand() const{ return "dan"; }
		
		virtual const char* const* post_table() const{ return post_strings; }
		
		Post get_post( unsigned id ) const;
		std::vector<Post> get_index( std::string search, int page, int limit=-1 ) const;
};


template<>
inline std::vector<std::string> DanApi::convert_vector<std::string>( std::vector<std::string> list ) const{
	return list;
}

#endif

