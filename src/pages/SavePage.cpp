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

#include "SavePage.hpp"
#include "ProxyPage.hpp"
#include "UrlHandler.hpp"
#include "../exceptions/utils.hpp"
#include "../exceptions/ResourceMissing.hpp"

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"

#include "Styler.hpp"

#include <fstream>

using namespace std;

static string messagePage( const Api& api, string message ){
	BasicStyler s( "Saved" );
	s.body( HTML::p( s.doc )( message ) );
	s.head( link( s.doc, HTML::REL("shortcut icon"), HTML::HREF( "/favicon/" + api.get_shorthand() + "/saved" ) ) );
	return s.doc;
}


string SavePage::serve( vector<string> args, vector<header> &headers ) const{
	require( args.size() == 3, "fail" );
	
	//Parse parameters
	auto input = ProxyPage::parseParameters( args );
	Api& api = ApiHandler::get_instance()->get_by_shorthand( input.site );
	auto post = api.get_post( input.id, input.level );
	Image img = post.get_image_size( input.level );
	auto filename = "out/" + args[2];
	
	//Sanity checking
	if( img.isLocal() ){
		if( !post.saved ){
			debug( "Database inconsistency, image is local, but not saved" );
			post.saved = true;
			api.booru.save( post );
		}
		
		return messagePage( api, "Already saved" );
	}	
	
	//Save file
	auto result = ProxyPage().getImage( api, post, img );
	ofstream file( filename, ios_base::out | ios_base::binary );
	result.first->writeAll( file );
	
	//Save post in DB
	post.saved = true;
	post.full.setLocalPath( filename ); //TODO: Bug if input.level differs!
	api.booru.save( post );
	
	return redirect_page( UrlHandler( &api ).image_url( post, input.level ) );
//	return messagePage( api, "Saved" );
}

