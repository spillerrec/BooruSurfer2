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

#include "Server.hpp"

#include "pages/PageHandler.hpp"

#include <boost/network/protocol/http/client.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

using namespace std;

struct server_implementation;
typedef boost::network::http::server<server_implementation> server_t;
struct server_implementation{
	public:
		
		void operator() ( const server_t::request &request, server_t::response &response ) const{
		PageHandler pages = PageHandler();
			string contents;
			vector<string> raw_args;
			string query = destination( request );
			boost::split( raw_args, query, boost::is_any_of( "/" ) ); //TODO: avoid is_any_of() ?
			
			vector<string> args;
			for( string arg : raw_args )
				if( !arg.empty() )
					args.push_back( arg );
			
			vector<APage::header> headers;
			if( args.size() == 0 )
				contents = pages.get_root()->serve( args, headers );
			else
				contents = pages.get( args[0] )->serve( args, headers );
			
			//TODO: apply headers
			
			response = server_t::response::stock_reply( server_t::response::ok, contents );
			//response << boost::network::body( contents );
		//	response.headers.push_back( boost::network::header( "Content-Type", "image/png" ) );
			cout << "response served\n";
		}
		
		
		void log( server_t::string_type const &info ){
			std::cerr << "ERROR: " << info << '\n';
		}
};

Server::Server(){
	//TODO: load settings
}

void Server::run(){
	server_implementation handler;
	server_t my_server( server_t::options(handler).address( "localhost" ).port( "8000" ) );
	my_server.run();
}



