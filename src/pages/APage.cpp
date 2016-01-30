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

#include "APage.hpp"

#include "../api/ApiHandler.hpp"
#include "../db/Booru.hpp"

#include <Poco/Net/HTTPServerResponse.h>

#include <iostream>

using namespace Poco::Net;
using namespace std;

static void exceptionErrorPage( HTTPServerResponse& response, exception& e ){
	cout << "Exception occurred: " << e.what() << endl;
	string contents = "Exception happened during processing the page: ";
	contents += e.what();
	response.setStatus( HTTPResponse::HTTP_INTERNAL_SERVER_ERROR );
	response.sendBuffer( contents.c_str(), contents.size() );
}

template<typename Func>
void handleRequestExceptions( HTTPServerResponse& response, Func f ){
	try{ f(); }
	//TODO: Handle more specific errors
	catch( exception& e ){
		resetDatabaseConnections();
		exceptionErrorPage( response, e );
	}
}

void StringPage::handleRequest( Arguments args, HTTPServerResponse& response ){
	handleRequestExceptions( response, [&](){
		//Create content
		vector<APage::header> headers;
		string contents = serve( args, headers );
		
		//Add headers
		response.setStatus( HTTPResponse::HTTP_OK );
		for( APage::header h : headers )
			response.add( h.first, h.second );
		
		//Send content
		response.sendBuffer( contents.c_str(), contents.size() );
	} );
	
	try{
		ApiHandler::get_instance()->flush();
	}
	catch( exception& e ){
		resetDatabaseConnections();
		cout << "Flush failed\n";
	}
}

void StreamPage::Reader::writeAll( std::ostream& out ){
	char buffer[4096];
	int available;
	do{
		available = readBuf( buffer, sizeof(buffer) );
		out.write( buffer, available );
	}while( available == sizeof(buffer) );
}

void StreamPage::handleRequest( Arguments args, HTTPServerResponse& response ){
	handleRequestExceptions( response, [&](){
		vector<APage::header> headers;
		auto reader = serve( args, headers );
		
		//Add headers
		response.setStatus( HTTPResponse::HTTP_OK );
		for( APage::header h : headers )
			response.add( h.first, h.second );
		
		//Write everything from the Reader
		reader->writeAll( response.send() );
	} );
}