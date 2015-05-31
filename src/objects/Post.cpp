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

#include "Post.hpp"

#include <stdexcept>
#include <iostream>
#include <string>

template<typename Error=std::domain_error, typename T, typename Msg>
T eqAssert( T a, T b, Msg msg ){
	if( !( a == b ) )
		throw Error( std::to_string( a ) + " != " + std::to_string( b ) + " in " + msg );
	return a;
}

std::string fillMissing( std::string a, std::string b, const char* msg ){
//	if( !a.empty() && !b.empty() )
//		return eqAssert<std::domain_error>( a, b, msg );
	return a.empty() ? b : a;
}

Image Post::get_image_size( Image::Size size ) const{
	switch( size ){
		case Image::THUMB:      return thumbnail;
		case Image::RESIZED:    return preview;
		case Image::COMPRESSED: return reduced;
		case Image::ORIGINAL:   return full;
		default: return full;
	}
}

Image::Size Post::available() const{
	if( !full   .url.empty() ) return Image::ORIGINAL;
	if( !reduced.url.empty() ) return Image::COMPRESSED;
	if( !preview.url.empty() ) return Image::RESIZED;
	
	return Image::THUMB;
}

template<typename Res>
Resource<Res> resourceCombine( const Resource<Res>& a, const Resource<Res>& b ){
	Resource<Res> out;
	out.list = a.list;
	for( auto& item : b.list )
		if( !a.contains( item ) )
			out.add( item );
	return out;
}

Image imageCombine( const Image& a, const Image& b ){
	Image out;
	out.url = a.url.empty() ? b.url : a.url; //TODO: prefer local files
	out.width = std::max( a.width, b.width );
	out.height = std::max( a.height, b.height );
	out.size = std::max( a.size, b.size );
	return out;
}

Post Post::combine( const Post& other ) const{
	auto p = *this;
	
	p.id = eqAssert<std::invalid_argument>( id, other.id, "post id" );
	p.hash   = fillMissing( hash,   other.hash,   "hash" );
	p.author = fillMissing( author, other.author, "author" );
	p.source = fillMissing( source, other.source, "source" );
	//creation_time
	
	p.tags     = resourceCombine( tags,     other.tags     );
	p.parents  = resourceCombine( parents,  other.parents  );
	p.children = resourceCombine( children, other.children );
	p.notes    = resourceCombine( notes,    other.notes    );
	p.comments = resourceCombine( comments, other.comments );
	p.pools    = resourceCombine( pools,    other.pools    );
	
	p.score = std::max( score, other.score );
	//rating
	p.saved = saved || other.saved;
	
	p.full      = imageCombine( full,      other.full      );
	p.reduced   = imageCombine( reduced,   other.reduced   );
	p.preview   = imageCombine( preview,   other.preview   );
	p.thumbnail = imageCombine( thumbnail, other.thumbnail );
	
	return p;
}

