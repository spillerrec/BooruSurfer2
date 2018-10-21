cmake_minimum_required(VERSION 3.0)
project(BooruSurfer2)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# Turn on warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


set(SOURCES
	src/api/Api.cpp
	src/api/ApiHandler.cpp
	src/api/DanApi.cpp
	src/api/KonachanApi.cpp
	src/api/LocalApi.cpp
	src/api/SanApi.cpp
	
	src/db/AddLocal.cpp
	src/db/Booru.cpp
	src/db/Database.cpp
	src/db/LocalDb.cpp
	src/db/Statement.cpp
	
	src/objects/Comment.cpp
	src/objects/Image.cpp
	src/objects/Index.cpp
	src/objects/Note.cpp
	src/objects/Pool.cpp
	src/objects/Post.cpp
	src/objects/Tag.cpp
	
	src/pages/APage.cpp
	src/pages/FaviconPage.cpp
	src/pages/FilePage.cpp
	src/pages/HomePage.cpp
	src/pages/IndexPage.cpp
	src/pages/IndexStubPage.cpp
	src/pages/NotFoundPage.cpp
	src/pages/OpenSearchPage.cpp
	src/pages/PageHandler.cpp
	src/pages/PostPage.cpp
	src/pages/ProxyPage.cpp
	src/pages/RssPage.cpp
	src/pages/SavePage.cpp
	src/pages/Styler.cpp
	src/pages/UrlHandler.cpp
	
	src/parsing/OpenSearchDescription.cpp
	src/parsing/pugixml.cpp #Remove from project?
	src/parsing/Rss.cpp
	
	#src/server/ArgsParsing.cpp
	src/server/PocoServer.cpp
	
	src/main.cpp
	)
	
find_package(Poco REQUIRED NetSSL Net JSON XML Crypto) 
include_directories( Poco_INCLUDE_DIRS )
set( POCO_LINKS Poco_NetSSL_LIBRARY Poco_Net_LIBRARY Poco_JSON_LIBRARY Poco_XML_LIBRARY Poco_Crypto_LIBRARY )

if( NOT Poco_FOUND )
	message( FATAL_ERROR "Could not find Poco" )
endif()
	
if( UNIX )
	set(THUMB_SOURCES src/system/Thumbnailer-kde.cpp src/system/ThumbGetter.cpp)
	find_package(Qt5Core REQUIRED)
	find_package(Qt5Gui REQUIRED)
	set(CMAKE_AUTOMOC ON)
	
	find_package(KF5KIO REQUIRED)
	set(SYSTEM_LINKS Qt5::Core Qt5::Gui KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)
	message( WARNING "Using KDE for thumbnailing" )
	add_definitions( -DLINUX )
	add_definitions( -DUSES_QT )
	
	#TODO: Find this path
	include_directories( /usr/include/KF5/KIOWidgets/kio/ )
elseif( WIN32 )
	set(THUMB_SOURCES src/system/Thumbnailer-windows.cpp)
	set(SYSTEM_LINKS -lShell32 -lUuid -lGdi32 -lOle32 -lWindowscodecs -lboost_system-mgw52-mt-1_61 -lboost_filesystem-mgw52-mt-1_61.dll)
	#TODO: find boost
else()
	set(THUMB_SOURCES src/system/Thumbnailer-fallback.cpp)
	message( WARNING "Using null implementation of thumbnail" )
endif()
	


add_executable(BooruSurfer2 ${SOURCES} ${THUMB_SOURCES} )
target_link_libraries(BooruSurfer2 -ltidy -ljansson -lpng -lsqlite3 -llzma -lpugixml ${Poco_LIBRARIES} ${SYSTEM_LINKS})

# C++14
set_property(TARGET BooruSurfer2 PROPERTY CXX_STANDARD 14)
set_property(TARGET BooruSurfer2 PROPERTY CXX_STANDARD_REQUIRED ON)
