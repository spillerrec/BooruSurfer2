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

#include <Poco/Timestamp.h>

#include "PostPage.hpp"
#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "../exceptions/utils.hpp"

#include "Styler.hpp"

using namespace std;
using namespace HTML;

string PostPage::serve( vector<string> args, vector<header> &headers ) const{
	require( args.size() == 3, "fail" );
	Api& api = ApiHandler::get_instance()->get_by_shorthand( args[1] );
	UrlHandler url( &api );
	
	unsigned id = getInt( args[2], "Post id not an number!" );
	cout << "Post: " << id << "\n";
	Post post = api.get_post( id );
	
	Styler s( &api, "Post: " + url.image_tags( post, 128 ) );
	headers.push_back( content_type() );
	
	//Quick link to image
	//TODO: change behaviour once we have pools
	s.head( link(s.doc, REL("next"), HREF( url.image_url( post, Image::ORIGINAL, true ) ) ) );
	
	s.head( link(s.doc, REL("shortcut icon"), HREF( "/favicon/" + api.get_shorthand() + "/post" ) ) );
	
	auto navi = s.main_navigation( "" );
	navi( li(s.doc)( "Options" )(
				ul(s.doc)(
					li(s.doc)( a(s.doc, HREF( url.image_url( post, Image::ORIGINAL, true ) ))( "Save post" ) )
				)
			) );
	s.nav( navi );
	
	//Post info
	auto info = aside( s.doc, CLASS( "post_info" ) );
	info( h3(s.doc)( "Info:" ) );
	s.post_info( info, post, true );
	
	s.tag_list( info, api.tag_handler.getAll( post.tags ), "Tags:" );
	
	s.body(
			div( s.doc, ID("container") )(
					s.post( post )
				,	info
				)
		);
	
	return s.doc;
}

