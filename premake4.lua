solution( "BooruSurfer2" )
	configurations{ "debug", "release" }
	
	project( "Server" )
		kind( "ConsoleApp" )
		targetname( "BooruSurfer" )
		language( "C++" )
		location( "build" )
		files( { "./src/**.h", "./src/**.cpp" } )
		
		-- Enable C++14 support
		buildoptions{ "-std=c++14" }
		
		links( {
				"tidy"
			,	"jansson"
			,	"png"
			,	"sqlite3"
			
			-- Poco libraries and dependencies
			,	"PocoNetSSL"
			,	"PocoCrypto"
			,	"PocoNet"
			,	"PocoUtil"
			,	"PocoJSON"
			,	"PocoXML"
			,	"PocoFoundation"
		--	,	"Ws2_32"
		--	,	"wsock32"
			,	"ssl"
			,	"crypto"
		--	,	"iphlpapi"
			,	"nghttp2_asio"
			,	"boost_system"
			} )
		
		defines { "POCO_STATIC" }
		
		configuration( "debug" )
			build_dir = "debug"
			targetdir( "bin/" .. build_dir )
			defines( { "DEBUG" } )
			flags { "Symbols" }
		
		configuration( "release" )
			build_dir = "release"
			targetdir( "bin/" .. build_dir )
			defines( { "RELEASE" } )
			flags { "Optimize" }
		
		
		-- Copy data files
		os.mkdir( "bin/" .. build_dir .. "/data" )
		files = os.matchfiles( "data/*.*" );
		for k, f in pairs( files ) do
			os.copyfile( f, "bin/" .. build_dir .. "/data/" .. path.getname(f) );
		end
		
		-- Copy resource files
		os.mkdir( "bin/" .. build_dir .. "/data" )
		files = os.matchfiles( "resources/*.*" );
		for k, f in pairs( files ) do
			os.copyfile( f, "bin/" .. build_dir .. "/resources/" .. path.getname(f) );
		end
