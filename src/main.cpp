#include "Server.hpp"

int main( int argc, char *argv[] ){
/* 	test_server handler;
	handler.add( "manage", &serve_manage );
	handler.add( "post", &serve_post );
	handler.add( "index", &serve_index );
	handler.add( "api", &serve_api );
	handler.add( "favicon", &serve_favicon );
	handler.add( "md5", &serve_md5 );
	handler.add( "tool", &serve_tool );
	handler.add( "file", &serve_file );
	
	server_t my_server( server_t::options(handler).address( "localhost" ).port( "8000" ) );
	my_server.run(); */
	
	Server server;
	server.run();

	
	return 0;
}