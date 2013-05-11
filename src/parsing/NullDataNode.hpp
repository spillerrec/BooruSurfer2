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

#ifndef NULL_DATA_NODE_H
#define NULL_DATA_NODE_H

#include "DataNodeInterface.hpp"
#include <string>

class NullDataNode : public DataNodeInterface{
	public:
		static DataNodeInterface* get_instance(){
			static NullDataNode* instance = NULL;
			return instance ? instance : instance = new NullDataNode();
		}
		
		virtual bool is_valid( void* p ) const{ false; }
		
		virtual bool as_boolean( void* p ) const{ false; }
		virtual int as_int( void* p ) const{ return 0; }
		virtual std::string as_string( void* p ) const{ return ""; }
		
		virtual void* get_by_name( void* p, const std::string& child_name ) const{ return NULL; }
		virtual void* get_by_index( void* p, const int index ) const{ return NULL; }
		virtual unsigned size( void* p ) const{ return 0; }
};

#endif

