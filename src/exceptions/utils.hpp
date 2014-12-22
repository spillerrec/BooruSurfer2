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

#ifndef EXCEPTIONS__UTILS_HPP
#define EXCEPTIONS__UTILS_HPP

#include "InvalidInput.hpp"

inline void require( bool valid, const std::string& error ){
	if( !valid )
		throw InvalidInput( error );
}

inline int getInt( const std::string& input, const std::string& error="Not an integer" ){
	try{ return stoi( input ); }
	catch( ... ){ throw InvalidInput( error ); }
}

#endif

