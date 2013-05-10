#include <boost/network/protocol/http/client.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <iostream>
#include <jansson.h>

using namespace std;

json_t* get_page( string server ){
	using namespace boost::network;
	http::client client;
	http::client::request request( server + "/post/index.json" );
	request << header( "Connection", "close" );
	
	//http::client::response response = client.get( request );
	
	json_error_t error;
	return json_loads( ((std::string)body( client.get( request ) )).c_str(), 0, &error );
}

string parse_post( json_t* post_object ){
	string contents = "<div>";
	
	if( json_is_object( post_object ) ){
		json_t* url = json_object_get( post_object, "preview_url" );
		if( json_is_string( url ) ){
			contents += "<img src=\"";
			contents += json_string_value( url );
			contents += "\"/>";
		}
	}
	
	return contents + "</div>";
}

struct test_server;
typedef boost::network::http::server<test_server> server_t;
struct test_server{
	void operator() ( const server_t::request &request, server_t::response &response ) const{
		string contents = "<html><title>test</title><body>";
		
		contents += "test";
		
		json_t* root = get_page( "https://yande.re" );
		if( root ){
			if( json_is_array( root ) ){
				for( int i=0; i<json_array_size( root ); i++ ){
					json_t* node = json_array_get( root, i );
					contents += parse_post( node );
					cout << "post found\n";
				}
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
	
	//Parse JSON
	json_t *root = get_page( "https://yande.re" );
	if( root ){
		if( json_is_array( root ) ){
			for( int i=0; i<json_array_size( root ); i++ ){
				json_t* node = json_array_get( root, i );
				cout << "post found\n";
			}
		}
	}
	else{
		std::cout << "Failed to parse JSON : \\" << "\n";
	}

	return 0;
}