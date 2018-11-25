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

#include "Image.hpp"
#include "../exceptions/utils.hpp"

#include <cstring>

constexpr const char* URL_FILE = "file:///";
constexpr int URL_FILE_LENGHT = std::strlen( URL_FILE );

Image::Size Image::from_string( std::string input ){
	if( input == "thumb"      ) return THUMB;
	if( input == "resized"    ) return RESIZED;
	if( input == "compressed" ) return COMPRESSED;
	return ORIGINAL;
}

bool Image::isLocal() const{
	if( url.empty() )
		return false;
	return url.compare( 0, URL_FILE_LENGHT, URL_FILE ) == 0; //TODO: Use starts_with
}

std::string Image::localPath() const{
	require( isLocal(), "Must be a file url: " + url );
	return url.substr( URL_FILE_LENGHT );
}

