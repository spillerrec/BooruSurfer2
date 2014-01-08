solution( "BooruSurfer2" )
	configurations{ "debug", "release" }
	
	project( "Server" )
		kind( "ConsoleApp" )
		language( "C++" )
		location( "build" )
		files( { "./src/**.h", "./src/**.cpp" } )
		
		buildoptions{ "-std=c++11" }
		
		links( {
				"tidy"
			,	"jansson"
			
			,	"PocoNetSSL"
			,	"PocoCrypto"
			,	"PocoNet"
			,	"PocoUtil"
			,	"PocoXML"
			,	"Ws2_32"
			,	"wsock32"
			,	"ssl"
			,	"crypto"
			,	"PocoFoundation"
			,	"iphlpapi"
			} )
		
		configuration( "debug" )
			targetdir( "bin/debug" )
			defines( { "DEBUG" } )
			flags { "Symbols" }
		
		configuration( "release" )
			targetdir( "bin/release" )
			defines( { "RELEASE" } )
			flags { "Optimize" }