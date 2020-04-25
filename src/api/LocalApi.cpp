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

#include "LocalApi.hpp"
#include "ApiHandler.hpp"

using namespace std;

LocalApi::LocalApi( string short_hand )
	:	Api( short_hand ) { }

Post LocalApi::fetch_post( unsigned id, Image::Size level ){
	Post post;
	post.id = id;
	ApiHandler::get_instance()->get_by_shorthand( site_name ).booru.load( post, level );
	return post;
}
Post LocalApi::get_post( unsigned id, Image::Size level, bool /*force_refresh*/ )
	{ return fetch_post( id, level ); }

Index LocalApi::get_index( string search, int page, int limit )
	{ return Booru( site_name ).iteratePosts( { search, page, limit } ); }
