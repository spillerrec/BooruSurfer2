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

#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>

#include "SanApi.hpp"

#include "MyHtml++.hpp"
#include "../parsing/pugixml.hpp"
#include "../parsing/StringView.hpp"
#include "../parsing/StringViewUtils.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <cctype>

using namespace std;
using namespace pugi;

static std::string parse_url( std::string_view url ){
	std::string url_s( url );
	if( starts_with( url, "//" ) )
		return "https:" + url_s;
	if( starts_with( url, "/" ) )
		return "https://chan.sankakucomplex.com" + url_s;
	return url_s;
}

static Post::Rating parseAgeRating( std::string_view input ){
	     if( input == "Safe"         ) return Post::SAFE;
	else if( input == "Questionable" ) return Post::QUESTIONABLE;
	else if( input == "Explicit"     ) return Post::EXPLICIT;
	else                               return Post::UNRATED;
}

static std::string_view hash_from_url( std::string_view url ){
	size_t start = url.find_last_of( '/' ) + 1;
	size_t end = url.find_last_of( '.' );
	
	//Validate results
	if( start == string::npos || end == string::npos || start >= url.size() )
		return "";
	else
		return url.substr( start, end-start );
}


static Tag::Type parseTagType( std::string_view input ){
	static const map<string,Tag::Type> function{
			{"tag-type-general"  , Tag::NONE     }
			
			//Sankaku
		,	{"tag-type-character", Tag::CHARACTER}
		,	{"tag-type-copyright", Tag::COPYRIGHT}
		,	{"tag-type-artist"   , Tag::ARTIST   }
			//Idol
		,	{"tag-type-idol"     , Tag::ARTIST   }
		,	{"tag-type-photo_set", Tag::COPYRIGHT}
		,	{"tag-type-medium"   , Tag::SPECIAL  }
		,	{"tag-type-meta"     , Tag::SPECIAL  }
		};
	auto it = function.find( std::string(input) );
	return ( it != function.end() ) ? it->second : Tag::UNKNOWN;
};


static Tag parse_tag( MyHtml::Node node, MyHtml::Tree& tree ){
	Tag tag;
	
	tag.id = std::string( node.child( tree.tag("a") ).child().text() );
	//TODO: Japanese name?
	
	tag.count = parseInt( node.child( tree.tag("span") ).child( tree.tag("span") ).child().text(), 0 );
	tag.type = parseTagType( node.attributes().valueOf( "class" ) );
	
	//Remove spaces in tag name
	std::replace( tag.id.begin(), tag.id.end(), ' ', '_' );
	std::replace( tag.id.begin(), tag.id.end(), '\n', '_' );
	
	return tag;
}

static Note parse_note( MyHtml::Node node, MyHtml::Tree& tree ){
	auto body = node.next(tree.tag("div"));
	
	//Get ids
	auto      id = parseInt( removePrefix( node["id"sv], "note-box-"  ), -1 );
	auto body_id = parseInt( removePrefix( body["id"sv], "note-body-" ), -2 );
	if( id != body_id ) //NOTE: The fallbacks differs to avoid matching in that case
		throw logic_error( "Note id and body id did not match" );
	
	
	Note note( id );
	
	map<std::string_view, std::string_view> arguments;
	for( auto part : splitAllOn( node["style"sv], ';') ){
		if( part.empty() )
			continue;
		
		//Convert "key: 123px" to insert { key, 123 }
		auto [key, value] = splitAt( part, ':' );
		if( value.empty() )
			throw logic_error( "Not a valid style: " + std::string(part) );
		
		arguments.emplace( trim( key ), removePostfix( trim( value ), "px" ) );
	}
	
	note.width  = parseInt( arguments.at( "width"  ), 0 );
	note.height = parseInt( arguments.at( "height" ), 0 );
	note.x      = parseInt( arguments.at( "left"   ), 0 );
	note.y      = parseInt( arguments.at( "top"    ), 0 );
	
	note.content = "";
	for( auto child : body )
		note.content += std::string( child.text() );
	//TODO: Parse the whole body recursively and handle note types. Also introduce newline
	
	return note;
}

static Poco::Timestamp parse_date_time( std::string_view time ){
	Poco::DateTime datetime;
	int timezone;
	
	//TODO: Remove dependency on Poco
	Poco::DateTimeParser::parse( Poco::DateTimeFormat::SORTABLE_FORMAT, std::string(time), datetime, timezone );
	return datetime.timestamp();
}

static bool get_image( MyHtml::Tree& tree, Post& p ){
	//Original/resized width+height+url
	Image resized, original;
	
	auto link = MyHtml::Search::byAttrValue( tree, "id", "image-link" ).first();
	auto image = link.child( tree.tag("img") );
	if( !image )
		return false;
	
	original.url = std::string( link.attributes().valueOf( "href" ) );
	auto highres_link = MyHtml::Search::byAttrValue(tree, "id", "highres").first();
	string highes = parse_url( highres_link.attributes().valueOf("href") );
	if( highes.size() > 0 )
		original.url = highes;
	
	auto attr = image.attributes();
	original.width  = parseInt(attr.valueOf("orig_width" ), 0);
	original.height = parseInt(attr.valueOf("orig_height"), 0);
	
	resized.width  = parseInt(  attr.valueOf("width" ), 0 );
	resized.height = parseInt(  attr.valueOf("height"), 0 );
	resized.url    = parse_url( attr.valueOf("src") );
	
	//It might not be resized, in that case the image is the original
	if( original.url.empty() )
		p.full = resized;
	else{
		p.preview = resized;
		p.full = original;
	}
	
	return true;
}

static bool get_video( MyHtml::Tree& tree, Post& p ){
	auto video = MyHtml::Search::byId( tree, "image" );
	if( !video )
		return false;
	
	auto attr = video.attributes();
	p.full.width  = parseInt(attr.valueOf("width" ), 0);
	p.full.height = parseInt(attr.valueOf("height"), 0);
	p.full.url    = parse_url( attr.valueOf("src") );
	
	return true;
}

static bool get_flash( MyHtml::Tree& tree, Post& p ){
	auto flash_container = MyHtml::Search::byId( tree, "non-image-content" );
	auto flash = flash_container.child(tree.tag("object")).child(tree.tag("embed"));
	if( !flash )
		return false;
	
	auto attr = flash.attributes();
	p.full.width  = parseInt(attr.valueOf("width" ), 0);
	p.full.height = parseInt(attr.valueOf("height"), 0);
	p.full.url    = parse_url( attr.valueOf("src") );
	
	return true;
}


static Post parse_preview( MyHtml::Node span ){
	Post post;
	post.id = parseInt( span.attributes().valueOf( "id" ).substr( 1 ), 0 ); //Remove initial 'p'
	
	auto img = span.child().child().attributes(); // a > img
	
	post.thumbnail.url    = parse_url( img.valueOf( "src" )  );
	post.thumbnail.width  = parseInt( img.valueOf( "width"  ), 0 );
	post.thumbnail.height = parseInt( img.valueOf( "height" ), 0 );
	
	//TODO: parent: class="... has-parent"
	//TODO: children: class="... has-children"
	//TODO: flagged: class="... flagged"
	
	auto hash = hash_from_url( post.thumbnail.url );
	if( hash.size() == 32 )
		post.hash = std::string( hash );
	else
		std::cout << "Warning: Could not read hash from preview with id: " << post.id << '\n';
	
	for( auto property : splitAllOn( img.valueOf( "title" ), ' ' ) ){
		if( property.empty() )
			continue; //Ignore empty properties
		
		auto [name, value] = splitAt( property, ':' );
		if( name == "Rating" )
			post.rating = parseAgeRating( value );
		else if( name == "Score" )
			post.score = parseDouble( value, -1 );
		else if( name == "User" )
			post.author = std::string( value );
		else if( name == "Size" ){
			auto [width, height] = splitAt(value, 'x');
			post.full.width  = parseInt(width , 0);
			post.full.height = parseInt(height, 0);
		}
		else
			post.tags.add( std::string( property ) );
	}
	
	return post;
}


Post SanApi::get_post( unsigned post_id, Image::Size level ){
	Post post;
	if( post_handler.get_checked( post_id, post, level ) )
		return post;
	
	
	MyHtml::Html myHtml;
	MyHtml::Tree tree( myHtml );
	auto data = get_from_url( get_url() + "post/show/" + to_string( post_id ) );
	tree.parse( data.c_str(), data.size() );
	
	//TODO: comments
	post.id = post_id;
	
	if( !get_image( tree, post ) )
		if( !get_video( tree, post ) )
			if( !get_flash( tree, post ) )
				throw logic_error( "No media found in HTML post" );
	
	//Parent
	//xml_node parent = doc.select_nodes( "//div[@id='parent-preview']/span" ).first().node();
//	if( parent )
//		post.set_parent( parse_preview( parent ) );
	
	//Children
//	for( auto it : doc.select_nodes( "//div[@id='child-preview']/span" ) )
//		post.children.push_back( parse_preview( it.node() ) );
	
	//Score
	auto s_id = std::to_string( post.id );
	auto score      = parseDouble( MyHtml::Search::byId( tree, "post-score-"      + s_id ).child().text(), -1.0 );
	auto vote_count = parseDouble( MyHtml::Search::byId( tree, "post-vote-count-" + s_id ).child().text(),  0.0 );
	post.score = score * vote_count;
	
	//Parsing of the detail section of the side-bar
	auto stats = MyHtml::Search::byId( tree, "stats" ).child( tree.tag("ul") );
	for( auto li : stats ){
		auto [info, value] = splitAt( li.child().text(), ' ' );
		
		if( info == "Rating:" )
			post.rating = parseAgeRating( value );
		else if( info == "Source:" )
			post.source = std::string(value);
		else if( info == "\nPosted:" ){
			auto links = li.child( tree.tag("a") );
			
			post.creation_time = parse_date_time( links.attributes().valueOf( "title" ) );
			
			links = links.next( tree.tag("a") );
			if( links )
				post.author = std::string( links.child().text() );
			else
				post.author = "System";
		}
	}
	
	//Tags
	for( auto ul : MyHtml::Search::byAttrValue( tree, "id", "tag-sidebar" ) )
		for( auto li : ul )
			if( !li.getTag().isText() ){
				Tag t = parse_tag( li, tree );
				if( !post.tags.contains( t.id ) )
					post.tags.add( t.id );
				tag_handler.add( t );
			}
	
	//Notes:
	for( auto notebox : MyHtml::Search::byAttrValue(tree, "class", "note-box")){
		auto note = parse_note( notebox, tree );
		note.post_id = post.id;
		
		//Scale to range 0.0-1.0
		note.x      /= double(post.full.width );
		note.y      /= double(post.full.height);
		note.width  /= double(post.full.width );
		note.height /= double(post.full.height);
		
		post.notes.add( note.id );
		note_handler.add( note );
	}
	
	//Favorites
//	for( auto it : doc.select_nodes( "//span[@id='favorited-by']/a" ) )
//		post.fans.push_back( it.node().child_value() );
//	for( auto it : doc.select_nodes( "//span[@id='remaining-favs']/a" ) )
//		post.fans.push_back( it.node().child_value() );
//	cout << "Amount of fans: " << post.fans.size() << "\n";
	
	post_handler.add( post ); //TODO: Removing this causes very repeatable "database is locked" errors
	return post;
}

Index SanApi::get_index( string search, int page, int limit ){
	Index index({ search, page, limit });
	string url = get_url() + "?tags=" + Server::remove_reserved( Server::encode_str( search ) );
	if( page > 1 )
		url += "&page=" + to_string( page );
	
	MyHtml::Html myHtml;
	MyHtml::Tree tree( myHtml );
	auto data = get_from_url( url );
	tree.parse( data.c_str(), data.size() );

	for( auto node : MyHtml::Search::byAttrValueSpaceSeperated( tree, "class", "thumb", false ) ){
		auto post = parse_preview( node );
		post_handler.add( post );
		index.posts.push_back( post );
	}
	
	for( auto node : MyHtml::Search::byAttrValue( tree, "id", "tag-sidebar", false ).first() )
		if( !node.getTag().isText() ){
			Tag t = parse_tag( node, tree );
			tag_handler.add( t );
			index.related_tags.add( t.id );
		}

	index.amount = -1;
	return index;
}

