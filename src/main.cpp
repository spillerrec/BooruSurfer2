#include <boost/network/protocol/http/client.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>
#include <jansson.h>

#include <unordered_map>
#include <utility>

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
	if( url ){
		contents += "<img src=\"" + (string)url + "\"/>";
	}
	
	return contents + "</div>";
}


string serve_root(){
	return "<html><head><title>BooruSurfer</title></head><body>Welcome to BooruSurfer!</body></html>";
}

string serve_404(){
	return "<html><head><title>BooruSurfer</title></head><body>Page could not be found</body></html>";
}

string serve_error(){
	return "<html><head><title>BooruSurfer</title></head><body>Page could not be found</body></html>";
}

string serve_not_implemented(){
	return "<html><head><title>BooruSurfer</title></head><body>Page has not yet been implemented</body></html>";
}

string serve_manage( vector<string> args ){
	return serve_not_implemented();
}

string serve_post( vector<string> args ){
	return serve_not_implemented();
}

string serve_index( vector<string> args ){
	string contents = "<html><title>test</title><body>";
	
	DataNode root = get_page( "https://yande.re" );
	if( root ){
		for( DataNode post : root )
			contents += parse_post( post );
	}
	else{
		std::cout << "Failed to parse JSON : \\" << "\n";
	}
	
	contents += "</html>";
	return contents;
}

string serve_api( vector<string> args ){
	return serve_not_implemented();
}

string serve_favicon( vector<string> args ){
	return serve_not_implemented();
}

string serve_md5( vector<string> args ){
	return serve_not_implemented();
}

string serve_tool( vector<string> args ){
	return serve_not_implemented();
}

struct test_server;
typedef boost::network::http::server<test_server> server_t;
struct test_server{
	typedef string (*page_handler)( vector<string> );
	unordered_map<string, page_handler> pages;
	
	void add( string name, page_handler handler ){
		pages[ name ] = handler;
	}
	
	void operator() ( const server_t::request &request, server_t::response &response ) const{
		string contents;
		vector<string> raw_args;
		string query = destination( request );
		boost::split( raw_args, query, boost::is_any_of( "/" ) ); //TODO: avoid is_any_of() ?
		
		vector<string> args;
		for( string arg : raw_args )
			if( !arg.empty() )
				args.push_back( arg );
		
		if( args.size() == 0 )
			contents = serve_root();
		else{
			unordered_map<string, page_handler>::const_iterator got = pages.find( args[0] );
			if( got != pages.end() )
				contents = (got->second)( args ); //This 
			else
				contents = serve_404();
		}
		
		response = server_t::response::stock_reply( server_t::response::ok, contents );
		cout << "response served\n";
	}
	

    void log(server_t::string_type const &info) {
        std::cerr << "ERROR: " << info << '\n';
    }
};


int main(int argc, char *argv[]) {
	test_server handler;
	handler.add( "manage", &serve_manage );
	handler.add( "post", &serve_post );
	handler.add( "index", &serve_index );
	handler.add( "api", &serve_api );
	handler.add( "favicon", &serve_favicon );
	handler.add( "md5", &serve_md5 );
	handler.add( "tool", &serve_tool );
	
	server_t my_server( server_t::options(handler).address( "localhost" ).port( "8000" ) );
	my_server.run();

	return 0;
}