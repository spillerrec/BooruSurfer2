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

#ifndef JSON_DATA_NODE_H
#define JSON_DATA_NODE_H

#include "DataNodeInterface.hpp"
#include "DataNode.hpp"
#include "NullDataNode.hpp"
#include <jansson.h>

class JsonDataNode : public DataNodeInterface{
	public:
		static DataNodeInterface* get_instance(){
			static JsonDataNode* instance = NULL;
			return instance ? instance : instance = new JsonDataNode();
		}
		
		static DataNode from_string( std::string str ){
			json_t* parsed = json_loads( str.c_str(), 0, NULL );
			return DataNode( parsed, parsed ? get_instance() : NullDataNode::get_instance() );
		}
		static DataNode from_file( std::string path ){
			json_t* parsed = json_load_file( path.c_str(), 0, NULL );
			return DataNode( parsed, parsed ? get_instance() : NullDataNode::get_instance() );
		}
		
		virtual bool is_valid( void* p ) const{ return !json_is_null( (json_t*)p ); }
		
		virtual bool as_boolean( void* p ) const{
			return json_is_boolean( (json_t*)p );
		}
		virtual int as_int( void* p ) const{
			return json_integer_value( (json_t*)p );
		}
		virtual std::string as_string( void* p ) const{
			const char* str = json_string_value( (json_t*)p );
			return str ? std::string( str ) : "";
		}
		
		virtual void* get_by_name( void* p, const std::string& child_name ) const{
			return json_object_get( (json_t*)p, child_name.c_str() );
		}
		virtual void* get_by_index( void* p, const int index ) const{
			return json_array_get( (json_t*)p, index );
		}
		virtual unsigned size( void* p ) const{ return json_array_size( (json_t*)p ); }
};

#endif

