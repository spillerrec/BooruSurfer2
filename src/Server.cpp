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

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/URI.h>

#include "Server.hpp"
#include "pages/PageHandler.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <iostream>
#include <algorithm>

using namespace Poco;
using namespace Poco::Net;
using namespace std;

string Server::remove_reserved( const string& input ){
	string output = input;
	boost::replace_all( output, "$", "%24" );
	boost::replace_all( output, "&", "%26" );
	boost::replace_all( output, "+", "%2B" );
	boost::replace_all( output, ",", "%2C" );
	boost::replace_all( output, "/", "%2F" );
	boost::replace_all( output, ":", "%3A" );
	boost::replace_all( output, ";", "%3B" );
	boost::replace_all( output, "=", "%3D" );
	boost::replace_all( output, "?", "%3F" );
	boost::replace_all( output, "@", "%40" );
	return output;
}
string Server::encode_str( const string& input ){
	string output, reserved; //TODO: reserved, wtf?
	URI::encode( input, reserved, output );
	return output;
}
string Server::unencode_str( const string& input ){
	string output;
	URI::decode( input, output );
	return output;
}

class RequestHandler : public HTTPRequestHandler {
	public:
		virtual void handleRequest( HTTPServerRequest& req, HTTPServerResponse& response ) override {
			PageHandler pages;
			
			//Split on '/' and remove empty parts
			vector<string> args;
			boost::split( args, req.getURI(), boost::is_any_of( "/" ) ); //TODO: avoid is_any_of() ?
			args.erase( remove_if( args.begin(), args.end(), [](string arg){ return arg.empty(); } ), args.end() );
			
			for( auto& arg : args )
				arg = Server::unencode_str( arg );
			
			//Create content
			string contents;
			vector<APage::header> headers;
			if( args.size() == 0 )
				contents = pages.get_root()->serve( args, headers );
			else
				contents = pages.get( args[0] )->serve( args, headers );
			
			//Add headers
			response.setStatus( HTTPResponse::HTTP_OK );
			for( APage::header h : headers )
				response.add( h.first, h.second );
			
			//Send content
			response.sendBuffer( contents.c_str(), contents.size() );
		}
};

class RequestHandlerFactory : public HTTPRequestHandlerFactory {
	public:
		virtual HTTPRequestHandler* createRequestHandler( const HTTPServerRequest& request ) override{
			return new RequestHandler;
		}
};


int Server::main( const vector<string>& unknown ){
	HTTPServer s( new RequestHandlerFactory, ServerSocket( 8000 ), new HTTPServerParams );
	
	s.start();
	waitForTerminationRequest();
	s.stop();
	
	return Application::EXIT_OK;
}




