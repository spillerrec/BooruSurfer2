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

#ifndef RESOURCE_H
#define RESOURCE_H

#include <vector>

template<class object, class indentifier = unsigned>
class Resource{
	private:
		std::vector<indentifier> list;
		bool exists; //if true, then the resource exists, even if not in the list
		
	public:
		Resource( bool exists ) : exists( exists ){ }
		Resource( std::vector<indentifier> list ) : list( list ), exists( !list.empty() ){ }
		
		std::vector<indentifier> get() const{ return list; }
};

#endif

