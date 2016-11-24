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

#include <nghttp2/asio_http2_server.h>

#include <iostream>

using namespace std;
using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

static void exceptionErrorPage( const response& res, exception& e ){
	cout << "Exception occurred: " << e.what() << endl;
	string contents = "Exception happened during processing the page: ";
	contents += e.what();
	res.write_head( 500 ); //HTTP_INTERNAL_SERVER_ERROR
	res.end( contents );
}

template<typename Func>
void handleRequestExceptions( const response& res, Func f ){
	try{ f(); }
	//TODO: Handle more specific errors
	catch( exception& e ){
		resetDatabaseConnections();
		exceptionErrorPage( res, e );
	}
}

void StringPage::handleRequest( Arguments args, const response& res ){
	handleRequestExceptions( res, [&](){
		//Create content
		vector<APage::header> headers;
		string contents = serve( args, headers );
		
		//Add headers
		header_map header;
		for( APage::header h : headers )
			header.emplace( h.first, header_value{h.second, false} );
		res.write_head( 200, header );
		
		//Send content
		res.end( contents );
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

void StreamPage::handleRequest( Arguments args, const response& res ){
	handleRequestExceptions( res, [&](){
		vector<APage::header> headers;
		auto reader = serve( args, headers );
		
		//Add headers
		header_map header;
		for( APage::header h : headers )
			header.emplace( h.first, header_value{h.second, false} );
		res.write_head( 200, header );
		
		//Write everything from the Reader
	//	reader->writeAll( res.send() );
	} );
}
