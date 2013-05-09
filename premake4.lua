solution( "BooruSurfer2" )
	configurations{ "debug", "release" }
	
	project( "Server" )
		kind( "ConsoleApp" )
		language( "C++" )
		location( "build" )
		files( { "./src/**.h", "./src/**.cpp" } )
		
		links( {
				"cppnetlib-client-connections"
			,	"cppnetlib-uri"
			,	"pthread"
			,	"jansson"
			,	"crypto"
			,	"ssl"
			,	"ws2_32"
			,	"wsock32"
			} )
		
		configuration( "debug" )
			targetdir( "bin/debug" )
			defines( { "DEBUG", "BOOST_NETWORK_ENABLE_HTTPS", "_WIN32_WINNT=0x0501" } )
			flags { "Symbols" }
			links( {
					"boost_system-mgw47-mt-d-1_53"
				,	"boost_thread-mgw47-mt-d-1_53"
				} )
		
		configuration( "release" )
			targetdir( "bin/release" )
			defines( { "RELEASE", "BOOST_NETWORK_ENABLE_HTTPS", "_WIN32_WINNT=0x0501" } )
			flags { "Optimize" }
			links( {
					"boost_system-mgw47-mt-1_53"
				,	"boost_thread-mgw47-mt-1_53"
				} )