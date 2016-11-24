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


#include <Poco/URI.h>

#include <nghttp2/asio_http2_server.h>

#include "Server.hpp"
#include "api/ApiHandler.hpp"
#include "db/Booru.hpp"
#include "pages/APage.hpp"
#include "pages/PageHandler.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/system/error_code.hpp>

#include <iostream>
#include <algorithm>

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
	Poco::URI::encode( input, reserved, output );
	return output;
}
string Server::unencode_str( const string& input ){
	string output;
	string temp = input;
	boost::replace_all( temp, "+", "%20" );
	Poco::URI::decode( temp, output );
	return output;
}



static void handle_request( const nghttp2::asio_http2::server::request& req, const nghttp2::asio_http2::server::response& res ){
	PageHandler pages;
	
	//Split on '/' and remove empty parts
	vector<string> args;
	boost::split( args, req.uri().path, boost::is_any_of( "/" ) ); //TODO: avoid is_any_of() ?
	args.erase( remove_if( args.begin(), args.end(), [](string arg){ return arg.empty(); } ), args.end() );
	
	for( auto& arg : args )
		arg = Server::unencode_str( arg );
	
	//Process page
	if( args.size() == 0 )
		pages.get_root()->handleRequest( args, res );
	else
		pages.get( args[0] )->handleRequest( args, res );
}

int Server::main(){
	
	boost::system::error_code ec;
	boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);

	tls.use_private_key_file("key.pem", boost::asio::ssl::context::pem);
	tls.use_certificate_chain_file("cert.pem");

	nghttp2::asio_http2::server::configure_tls_context_easy(ec, tls);
	
	
	nghttp2::asio_http2::server::http2 server;
	server.num_threads( 4 );
	
	server.handle("/", &handle_request );
  
	if( server.listen_and_serve( ec, tls, "localhost", "8000" ) ){
		std::cout << "Server error: " << ec.message() << std::endl;
		return -1;
	}
	
	return 0;
}




