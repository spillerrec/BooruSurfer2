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
#include "../server/Server.hpp"

#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "../parsing/DataNode.hpp"
#include "../parsing/StringViewUtils.hpp"

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
		std::vector<T> convert_vector( std::vector<std::string_view> list ) const{
			std::vector<T> converted;
			converted.reserve( list.size() );
			for( auto& s : list )
				converted.push_back( boost::lexical_cast<T>( s ) );
			return converted;
		}
		
		template<class T1>
		Resource<T1> get_resource( DataNode parent, PostItem resource ) const{
			const char* const name = post_table()[ resource ];
			if( name == NULL ) //TODO: when will this be null?
				return Resource<T1>( false );
			
			DataNode node = parent[ name ];
			if( !node )
				return Resource<T1>( false );
			
			if( node.as_boolean() )
				return Resource<T1>( true );
			
			if( !node.as_string().empty() ){
				//TODO: split string
				
				//Split and remove empty
				auto list = splitAllOn( node.as_string(), ' ' );
				removeEmpty( list );
				
				return Resource<T1>( convert_vector<typename T1::ID_T>( list ) );
			}
			
			return Resource<T1>( false );
		}
		
		virtual Tag::Type tag_type_id( int id ) const;
		virtual Tag parse_tag( DataNode node ) const;
		virtual Post parse_post( DataNode node ) const;
		virtual Image get_image( DataNode parent, PostItem url, PostItem width, PostItem height, PostItem size ) const;
		
	public:
		DanApi( std::string short_hand="dan" ) : Api( short_hand ) { }
		
		void load_tag_file();
		
		virtual std::string get_name() const{ return "Danbooru"; }
		virtual std::string get_url() const{ return "http://danbooru.donmai.us/"; }
		
		virtual const char* const* post_table() const{ return post_strings; }
		
		Post get_post( unsigned id, Image::Size level );
		Index get_index( std::string search, int page, int limit=-1 );
		
		virtual ThemeColor main_color() override{ return {0xA4,0x81,0x5E}; }
		virtual ThemeColor secondary_color() override{ return {0xFF,0xFF,0xFF}; }
		
		virtual std::string original_post_url( unsigned id ) override{
			return get_url() + "posts/" + std::to_string( id );
		}
		virtual std::string original_index_url( std::string search ) override{
			return get_url() + "posts/?tags=" + Server::remove_reserved( Server::encode_str( search ) );
		}
};

#endif

