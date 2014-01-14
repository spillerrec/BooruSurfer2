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

//Include Poco before anything else
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

#include "Api.hpp"

#include <iostream>
#include <sstream>

using namespace Poco::Net;
using namespace Poco;
using namespace std;

static string istream2string( const istream& stream ){
	return string(static_cast<stringstream const&>(stringstream() << stream.rdbuf()).str());
}

string http_get( URI uri, HTTPRequest& req, HTTPResponse& res ){
	cout << "http_get()\n";
	HTTPClientSession session(uri.getHost(), uri.getPort());
	session.sendRequest(req);
	return istream2string( session.receiveResponse(res) );
}

string https_get( URI uri, HTTPRequest& req, HTTPResponse& res ){
	cout << "https_get()\n";
	const auto context = new Context( Context::CLIENT_USE, "", Context::VERIFY_NONE );
	HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
	session.sendRequest(req);
	return istream2string( session.receiveResponse(res) );
}

string Api::get_from_url( string url, vector<pair<string,string> > headers ) const{
	if( url.empty() ){
		cout << "ERROR: no url\n";
		return "";
	}
	
	try{
		URI uri( url );
		cout << "Url: " << url << " - " << uri.getPathAndQuery() << "\n";
		
		HTTPRequest req(HTTPRequest::HTTP_GET, uri.getPathAndQuery(), HTTPMessage::HTTP_1_1);
		HTTPResponse res;
		
		//Add headers
		for( auto h : headers )
			req.add( h.first, h.second );
		req.add( "User-Agent", "Opera/9.80 (Windows NT 6.2; Win64; x64) Presto/2.12.388 Version/12.15" );
		//TODO: do something with the user-agent
		
		//Get response
		string str = ( url.find( "https://" ) == 0 )
			?	https_get( uri, req, res )
			:	http_get( uri, req, res )
			;
		
		switch( res.getStatus() ){
			case HTTPResponse::HTTP_OK:
				return str;
			
			case HTTPResponse::HTTP_MOVED_PERMANENTLY:
			case HTTPResponse::HTTP_FOUND:
			case HTTPResponse::HTTP_SEE_OTHER:
				if( res.has( "Location" ) ){
					cout << "Doing redirect, from: " << url << "\n";
					cout << "                  to: " << res.get( "Location" ) << "\n";
					return get_from_url( res.get( "Location" ) );
				}
				else{
					cout << "Could not find location to redirect to\n";
					return "";
				}
			
			default:
				cout << "Returned with status code: " << res.getStatus() << "\n";
				cout << str;
				return "";
		}
	}
	catch( Exception &ex ){
		cout << "Some exception happened during url request\n";
		cout << ex.displayText() << "\n";
		return "";
	}
}

