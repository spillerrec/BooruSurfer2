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
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>

#include "OpenSearchPage.hpp"

#include <iostream>

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "../exceptions/utils.hpp"
#include "../parsing/OpenSearchDescription.hpp"
#include "UrlHandler.hpp"

using namespace std;
using namespace pugi;

string OpenSearchPage::serve( vector<string> args, vector<header> &headers ) const{
	require( args.size() >= 2, "Too few arguments" );
	Api& api = ApiHandler::get_instance()->get_by_shorthand( args[1] );
	UrlHandler url( &api );
	
	cout << "Attempting to load this" << '\n';
	
	string search = "";
	if( args.size() == 3 )
		search = args[2];
	
	OpenSearchDescription osd;
	osd.short_name = "Booru: " + api.get_shorthand();
	osd.long_name = api.get_name();
	osd.description = "Search " + api.get_name() + " for images";
	osd.input_encoding = "UTF-8";
	//TODO: image
	osd.adult_content = false;
	
	OpenSearchDescription::Url search_url;
	//TODO: get absolute name
	search_url.temp = "http://localhost:8000/index/" + args[1] + "/{searchTerms}";
	search_url.type = "text/html";
//	search_url.rel = "results";
	osd.urls.push_back( search_url );
	
	//headers.push_back( header( "Content-Type", "application/opensearchdescription+xml" ) );
	return osd.save();
}

