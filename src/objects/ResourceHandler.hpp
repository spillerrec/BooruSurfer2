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

#include "../db/Booru.hpp"
#include "Resource.hpp"

template<class object, class indentifier = unsigned>
class ResourceHandler{
	private:
		Booru& booru;
		
	public:
		ResourceHandler( Booru& booru ) : booru(booru) { }
		void add( object& obj ){ booru.save( obj ); }
		
		template<typename Iter, typename Func>
		void addAll( Iter begin, Iter end, Func f ){
			for( auto start=begin; start!=end; ++start ){
				auto obj = f( *start );
				booru.save( obj );
			}
		}
		
		template<typename... Args>
		object get( indentifier id, Args... args ) const{
			object temp( id );
			return booru.load( temp, args... ) ? temp : object( id );
		}
		
		std::vector<object> getAll( const Resource<object>& resource ){
			std::vector<object> list;
			list.reserve( resource.list.size() );
			
			for( auto id : resource.list )
				list.emplace_back( get( id ) );
			
			return list;
		}
		
		template<typename... Args>
		bool get_checked( indentifier id, object& obj, Args... args ){
			obj.id = id;
			return booru.load( obj, args... );
		}
};

#endif

