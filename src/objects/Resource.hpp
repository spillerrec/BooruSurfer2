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

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <algorithm>
#include <vector>

template<class identity>
class Resource{
	public:
		typedef identity Object;
		std::vector<typename identity::ID_T> list;
		bool exists = true; //If true, content might exist even if list is empty
		
		Resource(){ }
		Resource( bool exists ) : exists(exists){ }
		Resource( std::vector<typename identity::ID_T> list ) : list(list){ }
		
		bool contains(typename identity::ID_T id ) const
			{ return std::find( list.begin(), list.end(), id ) != list.end(); }
		
		void add( typename identity::ID_T id ){
			list.push_back( id );
		}
};

#endif

