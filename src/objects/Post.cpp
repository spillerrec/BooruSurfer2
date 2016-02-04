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

bool Post::isAvailable( Image::Size size ) const{
	return !get_image_size( size ).url.empty();
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

static std::string urlCombine( const Image& a, const Image& b ){
	if( a.isLocal() ) return a.url;
	if( b.isLocal() ) return b.url;
	return a.url.empty() ? b.url : a.url;
}

Image imageCombine( const Image& a, const Image& b ){
	Image out;
	out.url = urlCombine( a, b );
	out.width = std::max( a.width, b.width );
	out.height = std::max( a.height, b.height );
	out.size = std::max( a.size, b.size );
	return out;
}

static Poco::Timestamp timeCombine( Poco::Timestamp time1, Poco::Timestamp time2 ){
	return ( time1 == 0 || time2 == 0 )
		?	std::max( time1, time2 ) //If one of them is unset, use the other one
		:	std::min( time1, time2 ) //Else use the earliest time
		;
}

Post Post::combine( const Post& other ) const{
	auto p = *this;
	
	p.id = eqAssert<std::invalid_argument>( id, other.id, "post id" );
	p.hash   = fillMissing( hash,   other.hash,   "hash" );
	p.author = fillMissing( author, other.author, "author" );
	p.source = fillMissing( source, other.source, "source" );
	p.creation_time = timeCombine( creation_time, other.creation_time );
	
	p.tags     = resourceCombine( tags,     other.tags     );
	p.parents  = resourceCombine( parents,  other.parents  );
	p.children = resourceCombine( children, other.children );
	p.notes    = resourceCombine( notes,    other.notes    );
	p.comments = resourceCombine( comments, other.comments );
	p.pools    = resourceCombine( pools,    other.pools    );
	
	p.score = std::max( score, other.score );
	p.saved = saved || other.saved;
	
	//Pick the highest rating, but ignore UNRATED
	auto max_rating = std::max( rating, other.rating );
	p.rating = ( max_rating == UNRATED ) ? std::min( rating, other.rating ) : max_rating;
	
	p.full      = imageCombine( full,      other.full      );
	p.reduced   = imageCombine( reduced,   other.reduced   );
	p.preview   = imageCombine( preview,   other.preview   );
	p.thumbnail = imageCombine( thumbnail, other.thumbnail );
	
	return p;
}

