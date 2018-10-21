#include "server/Server.hpp"

#include <Poco/Crypto/Crypto.h>
#include <Poco/Net/Net.h>
#include <Poco/Net/NetSSL.h>

#ifdef USES_QT
	#include <QGuiApplication>
	#include <thread>
#endif

int main( int argc, char *argv[] ){
	Poco::Net::initializeNetwork();
	Poco::Net::initializeSSL();
	Poco::Crypto::initializeCrypto();
	
	int result = 0;
	std::thread poco_thread( [&](){ result = Server().run( argc, argv ); } );
	
#ifdef USES_QT
	QGuiApplication qt_app( argc, argv );
	//std::thread qt_thread( [&](){ qt_app.exec(); } );
	qt_app.exec();
#endif
	
	poco_thread.join();
	Poco::Net::uninitializeNetwork();
	Poco::Net::uninitializeSSL();
	Poco::Crypto::uninitializeCrypto();
	
#ifdef USES_QT
//	qt_app.exit();
	//qt_thread.join();
#endif
	
	return result;
}
