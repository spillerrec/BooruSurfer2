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

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <cctype>
#include <charconv>

using namespace std;
using namespace pugi;

using str_view = std::string_view;


bool starts_with( str_view input, str_view match ){
	if( input.length() >= match.length() )
		return input.substr( 0, match.length() ) == match;
	return false;
}

std::pair<str_view, str_view> splitAt( str_view str, char splitter ){
	auto pos = str.find( splitter );
	if( pos == str_view::npos )
		return { str, {} };
	else
		return { str.substr( 0, pos ), str.substr( pos+1 ) };
}

std::vector<str_view> splitAllOn( str_view str, char splitter ){
	std::vector<str_view> splits;
	
	size_t last=0, pos = 0;
	while( (pos = str.find(splitter, last)) != str_view::npos ){
		splits.push_back( str.substr( last, pos-last ) );
		last = pos+1;
	}
	splits.push_back( str.substr( last ) );
	
	return splits;
}


static int parseInt( str_view input, int fallback ){
	int value = 0;
	auto result = std::from_chars( input.begin(), input.end(), value, 10 );
	return result.ptr == input.begin() ? fallback : value;
}

static double parseDouble( str_view input, double fallback ){
	//TODO: Avoid string conversion
	try
		{ return std::stod(std::string(input)); }
	catch(...)
		{ return fallback; }
}

static std::string parse_url( str_view url ){
	std::string url_s( url );
	if( starts_with( url, "//" ) )
		return "https:" + url_s;
	if( starts_with( url, "/" ) )
		return "https://chan.sankakucomplex.com" + url_s;
	return url_s;
}

static Post::Rating parseAgeRating( str_view input ){
	     if( input == "Safe"         ) return Post::SAFE;
	else if( input == "Questionable" ) return Post::QUESTIONABLE;
	else if( input == "Explicit"     ) return Post::EXPLICIT;
	else                               return Post::UNRATED;
}

static string hash_from_url( string url ){
	size_t start = url.find_last_of( '/' ) + 1;
	size_t end = url.find_last_of( '.' );
	
	//Validate results
	if( start == string::npos || end == string::npos || start >= url.size() )
		return "";
	else
		return url.substr( start, end-start );
}


static Tag::Type parseTagType( str_view input ){
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
	
	tag.id = std::string( node.getChildWith( tree.tag("a") ).child().text() );
	//TODO: Japanese name?
	
	tag.count = parseInt( node.getChildWith( tree.tag("span") ).getChildWith( tree.tag("span") ).child().text(), 0 );
	tag.type = parseTagType( node.attributes().valueOf( "class" ) );
	
	//Remove spaces in tag name
	std::replace( tag.id.begin(), tag.id.end(), ' ', '_' );
	std::replace( tag.id.begin(), tag.id.end(), '\n', '_' );
	
	return tag;
}

static string removePrefix( const string& str, const string& prefix ){
	if( str.compare( 0, prefix.size(), prefix ) != 0 )
		throw logic_error( str + " does not start with " + prefix );
	return { str.c_str() + prefix.size() };
}

static Note parse_note( const xml_node& node ){
	auto body = node.next_sibling("div");
	
	//Get ids
	const string box_prefix  = "note-box-";
	const string body_prefix = "note-body-";
	auto      id = parseInt( removePrefix( node.attribute( "id" ).value(), box_prefix  ), 0 );
	auto body_id = parseInt( removePrefix( body.attribute( "id" ).value(), body_prefix ), 0 );
	if( id != body_id )
		throw logic_error( "Note id and body id did not match" );
	
	
	Note note( id );
	
	auto parameters = cStrView( node.attribute( "style" ).value() );
	
	map<StringView, StringView> arguments;
	for( auto part : StringView(parameters).split(';') ){
		//Convert "key: 123px" to insert { key, 123 }
		auto items = part.split(':');
		if( items.size() != 2 )
			throw logic_error( "Not a valid style: " + part.toString() );
		
		auto trimmer = [](char c){ return isspace(c);};
		arguments.emplace(
				items[0].trim( trimmer )
			,	items[1].trim( trimmer ).removePostfix( cStrView("px") )
			);
	}
	
	auto convert = [arguments]( StringView key ){
			return parseInt( arguments.at( key ).toString(), 0 );
		};
	note.width  = convert( cStrView("width" ) );
	note.height = convert( cStrView("height") );
	note.x      = convert( cStrView("left"  ) );
	note.y      = convert( cStrView("top"   ) );
	
	note.content = body.text().get();
	
	return note;
}

static Poco::Timestamp parse_date_time( string time ){
	Poco::DateTime datetime;
	int timezone;
	
	Poco::DateTimeParser::parse( Poco::DateTimeFormat::SORTABLE_FORMAT, time, datetime, timezone );
	return datetime.timestamp();
}

static bool get_image( MyHtml::Tree& tree, Post& p ){
	//Original/resized width+height+url
	Image resized, original;
	
	auto link = MyHtml::Search::byAttrValue( tree, "id", "image-link" ).first();
	auto image = link.getChildWith( tree.tag("img") );
	if( !image )
		return false;
	
	original.url = std::string( link.attributes().valueOf( "href" ) );
	auto highres_link = MyHtml::Search::byAttrValue(tree, "id", "highres").first();
	string highes = parse_url( highres_link.attributes().valueOf("href") );
	if( highes.size() > 0 )
		original.url = highes;
	std::cout << "org url: " << original.url << '\n';
	
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
	auto flash = flash_container.getChildWith(tree.tag("object")).getChildWith(tree.tag("embed"));
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
	
//	cout << "\t" << "Hash: " << hash_from_url( post.thumbnail.url ) << "\n";
	//TODO: fill the other urls from this
	
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
	
	//TODO: notes
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
				/*
	string score_query( "//span[@id='post-score-" + id + "']" );
	string count_query( "//span[@id='post-vote-count-" + id + "']" );
	xml_node avg_span = doc.select_nodes( score_query.c_str() ).first().node();
	xml_node count_span = doc.select_nodes( count_query.c_str() ).first().node();
	post.score = parseDouble( avg_span.child_value(), -1.0 ) * parseDouble( count_span.child_value(), 0.0 );
	
	
	//Parsing of the detail section of the side-bar
	for( auto tag : doc.select_nodes( "//div[@id='stats']/ul/li" ) ){
		string val = tag.node().child_value();
		string start_rating = "Rating: ";
		string start_source = "Source: ";
		//TODO: file-sizes of images?
		
		if( boost::starts_with( val, start_rating ) )
			//Rating
			post.rating = parseAgeRating( val.substr( start_rating.size() ) );
		else if( boost::starts_with( val, start_source ) )
			//Source
			post.source = val.substr( start_source.size() );
		else if( boost::starts_with( val, "Posted: " ) ){
			//Date
			auto links = tag.node().child( "a" );
			
			string date = links.attribute( "title" ).value();
			post.creation_time = parse_date_time( date );
			
			//User-name
			if( (links = links.next_sibling( "a" )) )
				post.author = links.child_value();
			else
				post.author = "System";
		}
	}
	*/
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
		//TODO: Convert parse_note
		/*auto note = parse_note( notebox );
		note.post_id = post.id;
		
		//Scale to range 0.0-1.0
		note.x      /= double(post.full.width );
		note.y      /= double(post.full.height);
		note.width  /= double(post.full.width );
		note.height /= double(post.full.height);
		
		post.notes.add( note.id );
		note_handler.add( note );*/
	}
	
	//Favorites
//	for( auto it : doc.select_nodes( "//span[@id='favorited-by']/a" ) )
//		post.fans.push_back( it.node().child_value() );
//	for( auto it : doc.select_nodes( "//span[@id='remaining-favs']/a" ) )
//		post.fans.push_back( it.node().child_value() );
//	cout << "Amount of fans: " << post.fans.size() << "\n";
	
	//post_handler.add( post );
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

