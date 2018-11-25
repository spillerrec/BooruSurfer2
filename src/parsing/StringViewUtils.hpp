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

#ifndef STRING_VIEW_UTILS_HPP
#define STRING_VIEW_UTILS_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <charconv>
#include <stdexcept>
#include <string_view>


inline bool starts_with( std::string_view input, std::string_view match ){
	if( input.length() >= match.length() )
		return input.substr( 0, match.length() ) == match;
	return false;
}
inline bool ends_with( std::string_view input, std::string_view match ){
	if( input.length() >= match.length() )
		return input.substr( input.size() - match.length() ) == match;
	return false;
}

inline std::pair<std::string_view, std::string_view> splitAt( std::string_view str, char splitter ){
	auto pos = str.find( splitter );
	if( pos == std::string_view::npos )
		return { str, {} };
	else
		return { str.substr( 0, pos ), str.substr( pos+1 ) };
}

inline std::vector<std::string_view> splitAllOn( std::string_view str, char splitter ){
	std::vector<std::string_view> splits;
	
	size_t last=0, pos = 0;
	while( (pos = str.find(splitter, last)) != std::string_view::npos ){
		splits.push_back( str.substr( last, pos-last ) );
		last = pos+1;
	}
	splits.push_back( str.substr( last ) );
	
	return splits;
}
inline void removeEmpty( std::vector<std::string_view>& list ){
	list.erase( std::remove_if( list.begin(), list.end(), [](auto& item){ return item.empty(); } ), list.end() );
}


inline int parseInt( std::string_view input, int fallback ){
	int value = 0;
	auto result = std::from_chars( input.begin(), input.end(), value, 10 );
	return result.ptr == input.begin() ? fallback : value;
}

inline double parseDouble( std::string_view input, double fallback ){
	//TODO: Avoid string conversion
	//TODO: 4.7 got parsed as "4" !
	try
		{ return std::stod(std::string(input)); }
	catch(...)
		{ return fallback; }
}



inline std::string_view removePrefix( std::string_view str, std::string_view prefix ){
	if( !starts_with( str, prefix ) )
		throw std::logic_error( std::string(str) + " does not start with " + std::string(prefix) );
	return str.substr( prefix.size() );;
}
inline std::string_view removePostfix( std::string_view str, std::string_view postfix ){
	if( !ends_with( str, postfix ) )
		throw std::logic_error( std::string(str) + " does not end with " + std::string(postfix) );
	return str.substr( str.size() - postfix.size() );
}
inline std::string_view trim( std::string_view str ){
	auto first = str.find_first_not_of( ' ' );
	auto last  = str.find_last_not_of(  ' ' );
	auto chars_removed = first + (str.size()-last-1);
	return str.substr( first, str.size() - chars_removed );
}



#endif

