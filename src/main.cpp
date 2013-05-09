#include <boost/network/protocol/http/client.hpp>
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

int main(int argc, char *argv[]) {
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