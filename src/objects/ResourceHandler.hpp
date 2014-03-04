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

/*	Abstract class, which links to some resource which contents is not known.
 *	We might have an ID (or list of them), or a boolean determining if any exists
 */

#ifndef RESOURCE_HANDLER_HPP
#define RESOURCE_HANDLER_HPP

#include <map>

template<class object, class indentifier = unsigned>
class ResourceHandler{
	private:
		//TODO: do mutex
		std::map<indentifier,object> list;
		
	public:
		void add( object obj ){
			//TODO: check loaded state
			list.insert( {obj.id, obj} );
		}
		object get( indentifier id ) const{
			auto it = list.find( id );
			if( it != list.end() )
				return it->second;
			else
				return object( id );
		}
		
		bool get_checked( indentifier id, object& obj ){
			auto it = list.find( id );
			if( it != list.end() ){
				obj = it->second;
				return true;
			}
			return false;
		}
};

#endif

