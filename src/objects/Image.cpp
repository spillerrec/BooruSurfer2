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

#include <map>

using namespace std;


Image::Size Image::from_string( std::string input ){
	static const map<string,Image::Size> function{
			{"thumb", THUMB}
		,	{"resized", RESIZED}
		,	{"compressed", COMPRESSED}
		};
	auto it = function.find( input );
	return ( it != function.end() ) ? it->second : ORIGINAL;
}

bool Image::isLocal() const{
	if( url.empty() )
		return false;
	return url.compare( 0, 4, "http" ) != 0;
}

