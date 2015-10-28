#include "Server.hpp"

#include <poco/Crypto/Crypto.h>
#include <poco/Net/Net.h>
#include <poco/Net/NetSSL.h>

int main( int argc, char *argv[] ){
	Poco::Net::initializeNetwork();
	Poco::Net::initializeSSL();
	Poco::Crypto::initializeCrypto();
	
	auto result = Server().run( argc, argv );
	
	Poco::Net::uninitializeNetwork();
	Poco::Net::uninitializeSSL();
	Poco::Crypto::uninitializeCrypto();
	
	return result;
}