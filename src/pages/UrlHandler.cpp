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

#include "UrlHandler.hpp"

#include "../api/Api.hpp"

using namespace std;

string UrlHandler::post_url( const Post& p ) const{
	return "/post/" + api->get_shorthand() + "/" + to_string( p.id ) + "/";
}
string UrlHandler::index_url( const vector<Tag>& tags, unsigned page, int amount ) const{
	string start( "/index/" + api->get_shorthand() + "/" );
	
	//Page and post-amount
	string offset;
	if( page > 1 )
		offset += to_string( page );
	if( amount > 0 ){
		if( offset.empty() )
			offset = "1";
		offset += "-" + to_string( amount );
	}
	if( !offset.empty() )
		offset += "/";
	
	//Query string
	string search = "";
	for( auto t : tags )
		search += t.name + " ";
	
	return start + offset + search;
}

string image_size_string( Image::Size size ){
	switch( size ){
		case Image::THUMB:      return "thumb";
		case Image::RESIZED:    return "resized";
		case Image::COMPRESSED: return "compressed";
		case Image::ORIGINAL:   return "original";
		default: return "";
	}
}

string tags_to_name( const Post& p, Tag::Type type, int limit ){
	string s;
	//TODO: get tags
//	for( auto tag : p.tags.get() )
//		if( tag.type == type )
//			if( s.size() + tag.name.size() + 1 < limit )
//				s += tag.name + " ";
	return s.size() ? s : " ";
}

string UrlHandler::image_url( const Post& p, Image::Size size ){
	//TODO: change size depending on post
	string size_str = image_size_string( size );
	string path = "/proxy/" + size_str + "/";
	
	string filename_start = api->get_shorthand() + " " + to_string( p.id ) + " - ";
	string filename_end;
	if( size != Image::ORIGINAL )
		filename_end += "." + size_str;
		
	//Add file extension
	string filepath = p.get_image_size( size ).url;
	int pos = filepath.find_last_of( "." );
	if( pos != string::npos )
		filename_end += filepath.substr( pos );
	
	//Add tags
	string tags;
	int left = 128 - filename_start.size() - filename_end.size();
	tags += tags_to_name( p, Tag::ARTIST,    left - tags.size() - 3*2 ) + "- ";
	tags += tags_to_name( p, Tag::COPYRIGHT, left - tags.size() - 2*2 ) + "- ";
	tags += tags_to_name( p, Tag::CHARACTER, left - tags.size() - 1*2 ) + "- ";
	tags += tags_to_name( p, Tag::NONE, left - tags.size() );
	
	return path + filename_start + tags + filename_end;
}