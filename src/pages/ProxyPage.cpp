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
#include <Poco/Net/HTTPClientSession.h>

#include "ProxyPage.hpp"

#include "../exceptions/utils.hpp"

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"

#include <iostream>
#include <fstream>
#include <istream>
#include <algorithm>

using namespace std;

struct RequestReader : public FilePage::StreamReader{
	unique_ptr<Poco::Net::HTTPClientSession> session;
	RequestReader( Api::UrlResponse response )
		:	session( std::move( response.first ) ), StreamReader( response.second )
		{ }
};

ProxyPage::Parameters ProxyPage::parseParameters( APage::Arguments args ){
	require( args.size() == 3, "Wrong amount of arguments" );
	
	int pos1 = args[2].find_first_of( " " );
	int pos2 = args[2].find_first_of( " ", pos1+1 );
	if( pos1 == string::npos || pos2 == string::npos )
		throw runtime_error( "parsing failed" );
	
	string site = args[2].substr( 0, pos1 );
	int id = getInt( args[2].substr( pos1 + 1, pos2-pos1-1 ), "Not a valid id" );
	
	
	//TODO: use the filename to detect image size
	auto level = Image::from_string( args[1] );
	
	return { site, id, level };
}

//TODO: can we make Api const here?
FilePage::Result ProxyPage::getImage( Api& api, const Post& post, const Image& img ) const{
	//Determine file extension from url, which may contain arguments
	auto end = img.url.find( "?" );
	auto start = img.url.rfind( ".", end );
	auto ext = img.url.substr( start+1, end - start - 1 );
	
	if( img.isLocal() )
		return { unique_ptr<Reader>( new FileReader( img.localPath() ) ), ext };
	else{
		auto reader = unique_ptr<Reader>(
				new RequestReader( api.getFromUrl( img.url, { { "Referer", api.original_post_url( post.id ) } } ) )
			);
		return { std::move(reader), ext };
	}
}

FilePage::Result ProxyPage::getReader( APage::Arguments args ) const{
	auto input = parseParameters( args );
	Api& api = ApiHandler::get_instance()->get_by_shorthand( input.site );
	auto post = api.get_post( input.id, input.level );
	Image img = post.get_image_size( input.level );
	
	//QUICK-FIX: No thumbnails when using LocalApi
	if( input.level == Image::THUMB && post.saved && img.url.empty() )
		img = post.get_image_size( Image::ORIGINAL );
	
	return getImage( api, post, img );
}
