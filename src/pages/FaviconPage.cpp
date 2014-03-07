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

#include "FaviconPage.hpp"
#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"

#include "png++/png.hpp"

#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;
using namespace png;


rgb_pixel colorize( Api::ThemeColor c1, Api::ThemeColor c2, gray_pixel from ){
	return rgb_pixel(
			(c2.red   * from  +  c1.red   * (255-from)) / 255
		,	(c2.green * from  +  c1.green * (255-from)) / 255
		,	(c2.blue  * from  +  c1.blue  * (255-from)) / 255
		);
}

static const unordered_map<string, string> images{
		{"index", "i"}
	,	{"post", "p"}
};
string image_path( string wanted ){
	auto pos = images.find( wanted );
	wanted = pos != images.end() ? pos->second : "missing";
	return "data/favicon_" + wanted + ".png";
}

string to_string( image<rgb_pixel> img ){
	ostringstream stream;
	img.write_stream( stream );
	return stream.str();
}

string FaviconPage::serve( vector<string> args, vector<header> &headers ) const{
	headers.push_back( header( "Content-Type", "image/png" ) );
	
	image<rgb_pixel> img_fail( image_path( "" ) );
	if( args.size() < 3 )
		return to_string( img_fail );
	
	Api *api = ApiHandler::get_instance()->get_by_shorthand( args[1] );
	if( !api )
		return to_string( img_fail );
	
	try{
		auto main = api->main_color(), secondary = api->secondary_color();
		image<gray_pixel> img_template( image_path( args[2] ) );
		image<rgb_pixel> img_out( 16, 16 );
		if( img_template.get_width() != img_out.get_width()
			&& img_template.get_height() != img_out.get_height() )
			return to_string( img_fail );
		
		for( int iy=0; iy<img_out.get_height(); iy++ )
			for( int ix=0; ix<img_out.get_width(); ix++ )
				img_out.set_pixel( ix,iy, colorize( main,secondary, img_template.get_pixel( ix, iy ) ) );
		
		headers.push_back( header( "Cache-Control", "max-age=31536000" ) );
		return to_string( img_out );
	}
	catch(...){ return to_string( img_fail ); }
}

