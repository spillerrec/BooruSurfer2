#include "Server.hpp"

#include "db/Booru.hpp"

int main( int argc, char *argv[] ){
	Booru( "test2" );
	return Server().run( argc, argv );
}