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

#ifndef DATA_NODE_H
#define DATA_NODE_H

#include "DataNodeInterface.hpp"
#include "NullDataNode.hpp"
#include <string>

class DataNode{
	private:
		void* node;
		DataNodeInterface* interface;

	public:
		DataNode(){
			interface = NullDataNode::get_instance();
		}
		
		DataNode( void* n, DataNodeInterface* i ){
			node = n;
			interface = i;
		}
		
		bool is_valid() const{ return interface->is_valid( node ); }
		
		bool as_boolean() const{ return interface->as_boolean( node ); }
		int as_int() const{ return interface->as_int( node ); }
		std::string as_string() const{ return interface->as_string( node ); }
		
		DataNode operator[] ( const std::string& child_name ){
			return DataNode( interface->get_by_name( node, child_name ), interface );
		}
		DataNode operator[] ( const int index ) const{
			return DataNode( interface->get_by_index( node, index ), interface );
		}
		int size() const{ return interface->size( node ); }
};

#endif

