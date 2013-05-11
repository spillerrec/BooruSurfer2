#include <boost/network/protocol/http/client.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <iostream>
#include <jansson.h>

#include "parsing/JsonDataNode.hpp"
#include "parsing/NullDataNode.hpp"

using namespace std;

DataNode get_page( string server ){
	using namespace boost::network;
	http::client client;
	http::client::request request( server + "/post/index.json" );
	request << header( "Connection", "close" );
	
	//http::client::response response = client.get( request );
	return JsonDataNode::from_string( body( client.get( request ) ) );
}

string parse_post( DataNode post_object ){
	string contents = "<div>";
	
	
	DataNode url = post_object[ "preview_url" ];
	if( url.is_valid() ){
		contents += "<img src=\"" + url.as_string() + "\"/>";
	}
	
	return contents + "</div>";
}

struct test_server;
typedef boost::network::http::server<test_server> server_t;
struct test_server{
	void operator() ( const server_t::request &request, server_t::response &response ) const{
		string contents = "<html><title>test</title><body>";
		
		DataNode root = get_page( "https://yande.re" );
		if( root.is_valid() ){
			for( int i=0; i<root.size(); i++ ){
				contents += parse_post( root[ i ] );
				cout << "post found\n";
			}
		}
		else{
			std::cout << "Failed to parse JSON : \\" << "\n";
		}
		
		contents += "</html>";
		response = server_t::response::stock_reply( server_t::response::ok, contents );
		cout << "response served\n";
	}
	

    void log(server_t::string_type const &info) {
        std::cerr << "ERROR: " << info << '\n';
    }
};


int main(int argc, char *argv[]) {
	test_server handler;
	server_t my_server( server_t::options(handler).address( "localhost" ).port( "8000" ) );
	my_server.run();

	return 0;
}