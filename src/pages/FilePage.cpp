/*	This file is part of BooruSurfer2.

	BooruSurfer2 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	BooruSurfer2 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with BooruSurfer2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FilePage.hpp"
#include "../html/HtmlDocument.hpp"

#include <iostream>
#include <fstream>
#include <istream>

using namespace std;
using namespace pugi;
using namespace html;

FilePage::FilePage(){
	//Images
	mimes[ "bmp" ] = "image/x-windows-bmp";
	mimes[ "jpg" ] = "image/jpeg";
	mimes[ "jpeg" ] = "image/jpeg";
	mimes[ "png" ] = "image/png";
	mimes[ "apng" ] = "image/png";
	mimes[ "tif" ] = "image/tiff";
	mimes[ "tiff" ] = "image/tiff";
	mimes[ "gif" ] = "image/gif";
	mimes[ "webp" ] = "image/webp";
	mimes[ "svg" ] = "image/svg+xml";
	
	//Animations/movies
	mimes[ "swf" ] = "application/x-shockwave-flash";
	mimes[ "flv" ] = "video/x-flv";
	mimes[ "mp3" ] = "audio/mpeg";
	mimes[ "mp4" ] = "video/mp4";
	mimes[ "mkv" ] = "video/x-matroska";
	mimes[ "mka" ] = "audio/x-matroska";
	mimes[ "webm" ] = "video/webm";
	
	//Web related
	mimes[ "js" ] = "application/x-javascript";
	mimes[ "html" ] = "text/html";
	mimes[ "xhtml" ] = "application/xhtml+xml";
	mimes[ "css" ] = "text/css";
}

string FilePage::serve( vector<string> args, vector<header> &headers ) const{
	//Rebuild the filepath, as it was split appart
	string filepath = "files";
	for( string arg : args )
		if( arg != ".." )
			filepath += "/" + arg;
	
	//Get file extension, using the solution by graphitemaster:
	// http://stackoverflow.com/questions/51949/how-to-get-file-extension-from-string-in-c
	string ext = "";
	int pos = filepath.find_last_of( "." );
	if( pos != string::npos )
		ext = filepath.substr( pos + 1 );
	
	headers.push_back( header( "Content-Type", get_mime( ext ) ) );
	
	cout << filepath << "\n";
	fstream fs( filepath, fstream::in | fstream::binary );
	
	//Read entire stream into a string, using the solution by Joe Adams:
	// http://stackoverflow.com/questions/3203452/how-to-read-entire-stream-into-a-stdstring
	std::string ret;
	char buffer[4096];
	while( fs.read( buffer, sizeof(buffer) ) )
		ret.append( buffer, sizeof(buffer) );
	ret.append( buffer, fs.gcount() );
	
	return ret;
}

string FilePage::get_mime( string ext ) const{
	unordered_map<string, string>::const_iterator got = mimes.find( ext );
	if( got != mimes.end() )
		return got->second;
	else
		return "text/plain";
}

