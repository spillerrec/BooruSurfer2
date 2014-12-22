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

#include "ProxyPage.hpp"

#include "../exceptions/utils.hpp"

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"

#include <iostream>
#include <fstream>
#include <istream>
#include <algorithm>

using namespace std;


string ProxyPage::serve( vector<string> args, vector<header> &headers ) const{
	require( args.size() == 3, "Wrong amount of arguments" );
	
	int pos1 = args[2].find_first_of( " " );
	int pos2 = args[2].find_first_of( " ", pos1+1 );
	if( pos1 == string::npos || pos2 == string::npos )
		return "parsing failed";
	
	string site = args[2].substr( 0, pos1 );
	int id = getInt( args[2].substr( pos1 + 1, pos2-pos1-1 ), "Not a valid id" );
	
	Api& api = ApiHandler::get_instance()->get_by_shorthand( site );
	
	//TODO: use the filename to detect image size
	Post p = api.get_post( id );
	require( p.id != 0, "Could not retrive post" ); //TODO: make get_post throw
	
	Image img = p.get_image_size( Image::from_string( args[1] ) );
	
	//Detect mime-type
	int pos = img.url.find_last_of( "." );
	
	//Determine file extension from url, which may contain arguments
	auto end = img.url.find( "?" );
	auto start = img.url.rfind( ".", end );
	auto ext = img.url.substr( start+1, end - start - 1 );
	
	headers.push_back( header( "Content-Type", get_mime( ext ) ) );
	headers.push_back( header( "Cache-Control", "max-age=31536000" ) );
	
	return api.get_from_url( img.url, { { "Referer", api.original_post_url( id ) } } );
}

