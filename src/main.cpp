#include "Server.hpp"

#include <Poco/Crypto/Crypto.h>
#include <Poco/Net/Net.h>
#include <Poco/Net/NetSSL.h>

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