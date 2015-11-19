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
#include "../Server.hpp"

#include <algorithm>

using namespace std;

string UrlHandler::post_url( const Post& p ) const{
	return "/post/" + api->get_shorthand() + "/" + to_string( p.id ) + "/";
}
string UrlHandler::search_url( string type, const vector<Tag>& tags, unsigned page, int amount ) const{
	string start( "/" + type + "/" + api->get_shorthand() + "/" );
	
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
		search += Server::remove_reserved( t.id ) + " ";
	if( tags.size() > 0 )
		search.pop_back();
	
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

string tags_to_name( Api* api, const Post& p, Tag::Type type, int limit ){
	string s;
	for( auto raw : p.tags.list ){
		Tag tag = api->tag_handler.get( raw );
		if( tag.type == type )
			if( s.size() + tag.id.size() + 1 < limit )
				s += tag.id + " ";
	}
	return s.size() ? s : " ";
}

string UrlHandler::image_tags( const Post& p, int lenght ){
	//Add tags
	string tags;
	tags += tags_to_name( api, p, Tag::ARTIST,    lenght - tags.size() - 3*2 ) + "- ";
	tags += tags_to_name( api, p, Tag::COPYRIGHT, lenght - tags.size() - 2*2 ) + "- ";
	tags += tags_to_name( api, p, Tag::CHARACTER, lenght - tags.size() - 1*2 ) + "- ";
	tags += tags_to_name( api, p, Tag::NONE, lenght - tags.size() );
	
	//Remove last space, unless ending on "- "
	if( tags.size() > 2 )
		if( tags[tags.size()-1] == ' ' && tags[tags.size()-2] != '-' )
			tags = tags.substr( 0, tags.size()-1 );
	
	return tags;
}

string UrlHandler::image_url( const Post& p, Image::Size size, bool save ){
	if( size == Image::RESIZED && p.get_image_size( size ).url.empty() )
		size = Image::COMPRESSED;
	if( size == Image::COMPRESSED && p.get_image_size( size ).url.empty() )
		size = Image::ORIGINAL;
	
	//TODO: change size depending on post
	string size_str = image_size_string( size );
	string path = (save ? "/save/" : "/proxy/") + size_str + "/";
	
	string filename_end;
	if( size != Image::ORIGINAL )
		filename_end += "." + size_str;
	
	string filepath = p.get_image_size( size ).url;
	string filename_start = api->get_shorthand() + " " + to_string( p.id ) + " - ";
	
	//Ignore ?xxx parameters
	int pos = filepath.find_last_of( "?" );
	if( pos != string::npos )
		filepath = filepath.substr( 0, pos );
	
	//Add file extension
	pos = filepath.find_last_of( "." );
	if( pos != string::npos )
		filename_end += filepath.substr( pos );
	
	//Create tags and remove special characters
	string tags = image_tags( p, 128-filename_start.size()-filename_end.size() );
	replace( tags.begin(), tags.end(), '/', '-' );
	
	//Windows reserved characters:
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx#naming_conventions
	replace( tags.begin(), tags.end(), '<', '-' );
	replace( tags.begin(), tags.end(), '>', '-' );
	replace( tags.begin(), tags.end(), ':', '-' );
	replace( tags.begin(), tags.end(), '"', '-' );
	replace( tags.begin(), tags.end(), '\\', '-' );
	replace( tags.begin(), tags.end(), '|', '-' );
	replace( tags.begin(), tags.end(), '?', '-' );
	replace( tags.begin(), tags.end(), '*', '-' );
	
	return path + filename_start + tags + filename_end;
}
