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

#include "ProxyPage.hpp"

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"

#include <iostream>
#include <fstream>
#include <istream>
#include <algorithm>

using namespace std;


string ProxyPage::serve( vector<string> args, vector<header> &headers ) const{
	if( args.size() != 3 )
		return "fail";
	
	int pos1 = args[2].find_first_of( " " );
	int pos2 = args[2].find_first_of( " ", pos1+1 );
	if( pos1 == string::npos || pos2 == string::npos )
		return "parsing failed";
	
	string site = args[2].substr( 0, pos1 );
	int id;
	try{
		id = stoi( args[2].substr( pos1 + 1, pos2-pos1-1 ) );
	}
	catch( ... ){
		return "Not a valid id";
	}
	Api *api = ApiHandler::get_instance()->get_by_shorthand( site );
	if( !api )
		return "Not a site";
	
	//TODO: use the filename to detect image size
	Post p = api->get_post( id );
	Image img = p.get_image_size( Image::from_string( args[1] ) );
	
	//Detect mime-type
	int pos = img.url.find_last_of( "." );
	if( pos != string::npos )
		headers.push_back( header( "Content-Type", get_mime( img.url.substr( pos + 1 ) ) ) );
	
	string referer = api->get_url();
	if( api->get_shorthand() == "san" )
		referer += "post/show/" + to_string( id );
	return api->get_from_url( img.url, { { "Referer", referer } } );
}

