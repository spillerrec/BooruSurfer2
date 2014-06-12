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

#include "../parsing/pugixml.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <algorithm>
#include <iostream>
#include <map>

using namespace std;
using namespace pugi;

#include <tidy/tidy.h>
#include <tidy/buffio.h>

//Returns an empty string on error
static string CleanHTML(const string &html){
	TidyBuffer output;
	TidyDoc doc = tidyCreate();
	tidyBufInit( &output );
	
	bool ok = tidyOptSetBool( doc, TidyXmlOut, yes );
	ok &= tidyOptSetBool( doc, TidyQuiet, yes );
	ok &= tidyOptSetBool( doc, TidyShowWarnings, no );
	ok &= tidyOptSetValue( doc, TidyCharEncoding, "utf8" );
	
	int result = -1;
	if( ok ){
		result = tidyParseString( doc, html.c_str() );
		
		if( result >= 0 )
			result = tidyCleanAndRepair( doc );
		
		if( result > 1 )	// If error, force output.
			result = ( tidyOptSetBool( doc, TidyForceOutput, yes ) ? result : -1 );
		
		if( result >= 0 )
			result = tidySaveBuffer( doc, &output );
	}
	
	
	string xml = "";
	if( result >= 0 ){
		if( output.bp )
			xml = (char*)output.bp;
	}
	
	tidyBufFree( &output );
	tidyRelease( doc );
	
	return xml;
}

int parseInt( const string& input, int fallback ){
	try{
		return stoi( input );
	}
	catch( ... ){
		return fallback;
	}
}

double parseDouble( const string& input, double fallback ){
	try{
		return stod( input );
	}
	catch( ... ){
		return fallback;
	}
}


Post::Rating parseAgeRating( const string& input ){
	static const map<string,Post::Rating> function{
			{"Safe", Post::SAFE}
		,	{"Questionable", Post::QUESTIONABLE}
		,	{"Explicit", Post::EXPLICIT}
		};
	auto it = function.find( input );
	return ( it != function.end() ) ? it->second : Post::UNRATED;
};

string hash_from_url( string url ){
	size_t start = url.find_last_of( '/' ) + 1;
	size_t end = url.find_last_of( '.' );
	
	//Validate results
	if( start == string::npos || end == string::npos || start >= url.size() )
		return "";
	else
		return url.substr( start, end-start );
}

void extract_meta( Post &post, string meta ){
	//Split on ' ' and remove empty parts
	vector<string> data;
	boost::split( data, meta, boost::is_any_of( " " ) ); //TODO: avoid is_any_of() ?
	data.erase( remove_if( data.begin(), data.end(), [](string arg){ return arg.empty(); } ), data.end() );
	
	for( string s : data ){
		if( boost::starts_with( s, "Rating:" ) )
			post.rating = parseAgeRating( s.substr( 7 ) );
		else if( boost::starts_with( s, "Score:" ) )
			post.score = parseDouble( s.substr( 6 ), -1 );
		else if( boost::starts_with( s, "User:" ) )
			post.author = s.substr( 5 );
		else if( boost::starts_with( s, "Size:" ) )
			s.substr( 5 ); //TODO:
		else
			post.tags.add( s );
	}
}


Post parse_preview( const xml_node &span ){
	Post post;
	string id_string = span.attribute( "id" ).value();
	post.id = parseInt( id_string.substr( 1 ), 0 ); //Remove initial 'p'
	
	const xml_node &img = span.child( "a" ).child( "img" );
	
	post.thumbnail.url = img.attribute( "src" ).value();
	post.thumbnail.width = parseInt( img.attribute( "width" ).value(), 0 );
	post.thumbnail.height = parseInt( img.attribute( "height" ).value(), 0 );
	
	//TODO: size of original image: "Size:999x999"
	//TODO: parent: class="... has-parent"
	//TODO: children: class="... has-children"
	//TODO: flagged: class="... flagged"
	
	cout << "\t" << "Hash: " << hash_from_url( post.thumbnail.url ) << "\n";
	//TODO: fill the other urls from this
	
	string meta = img.attribute( "title" ).value();
	extract_meta( post, meta );
	
	return post;
}


Tag::Type parseTagType( const string& input ){
	static const map<string,Tag::Type> function{
			{"tag-type-general", Tag::NONE}
			
			//Sankaku
		,	{"tag-type-character", Tag::CHARACTER}
		,	{"tag-type-copyright", Tag::COPYRIGHT}
		,	{"tag-type-artist", Tag::ARTIST}
			//Idol
		,	{"tag-type-idol", Tag::ARTIST}
		,	{"tag-type-photo_set", Tag::COPYRIGHT}
		,	{"tag-type-medium", Tag::SPECIAL}
		,	{"tag-type-meta", Tag::SPECIAL}
		};
	auto it = function.find( input );
	return ( it != function.end() ) ? it->second : Tag::UNKNOWN;
};

Tag parse_tag( const xml_node &node ){
	Tag tag;
	tag.id = node.select_nodes( "./a" ).first().node().child_value();
	//TODO: Japanese name?
	tag.count = parseInt( node.select_nodes( ".//span[@class='post-count']" ).first().node().child_value(), 0 );
	tag.type = parseTagType( node.attribute( "class" ).value() );
	
	//Remove spaces in tag name
	replace( tag.id.begin(), tag.id.end(), ' ', '_' );
	replace( tag.id.begin(), tag.id.end(), '\n', '_' );
	
	return tag;
}

Poco::Timestamp parse_date_time( string time ){
	Poco::DateTime datetime;
	int timezone;
	
	Poco::DateTimeParser::parse( Poco::DateTimeFormat::SORTABLE_FORMAT, time, datetime, timezone );
	return datetime.timestamp();
}

bool get_image( xml_document& doc, Post& p ){
	//Original/resized width+height+url
	Image resized, original;
	xml_node link = doc.select_nodes( "//a[@id='image-link']" ).first().node();
	xml_node image = link.child( "img" );
	
	if( !image )
		return false;
	
	//Get image link, but overwrite it with highres to work with svg.
	original.url = link.attribute( "href" ).value();
	string highres = doc.select_nodes( "//a[@id='highres']" ).first().node().attribute( "href" ).value();
	if( highres.size() != 0 )
		original.url = highres;
	cout << "org url: " << original.url << endl;
	
	original.width = image.attribute( "orig_width" ).as_int();
	original.height = image.attribute( "orig_height" ).as_int();
	
	resized.width = image.attribute( "width" ).as_int();
	resized.height = image.attribute( "height" ).as_int();
	resized.url = image.attribute( "src" ).value();
	
	//It might not be resized, in that case the image is the original
	if( original.url.empty() )
		p.full = resized;
	else{
		p.preview = resized;
		p.full = original;
	}
	
	return true;
}

bool get_video( xml_document& doc, Post& p ){
	xml_node video = doc.select_nodes( "//video[@id='image']" ).first().node();
	if( !video )
		return false;
	
	p.full.width = video.attribute( "width" ).as_int();
	p.full.height = video.attribute( "height" ).as_int();
	p.full.url = video.attribute( "src" ).value();
	
	return true;
}

Post SanApi::get_post( unsigned id ){
	Post post;
	if( post_handler.get_checked( id, post ) )
		return post;
	
	string url = get_url() + "post/show/" + to_string( id );
	string html = CleanHTML( get_from_url( url ) );
	xml_document doc;
	xml_parse_result result = doc.load( html.c_str() );
	
	
	//TODO: notes
	//TODO: comments
	
	if( result.status == status_ok ){
		//Get ID
		string id = doc.select_nodes( "//meta[@property='og:title']" ).first().node().attribute("content").value();
		post.id = parseInt( id.substr( 5 ), 0 );
		
		if( !get_image( doc, post ) )
			if( !get_video( doc, post ) )
				return Post(); //TODO: throw error
		
		//Parent
		xml_node parent = doc.select_nodes( "//div[@id='parent-preview']/span" ).first().node();
	//	if( parent )
	//		post.set_parent( parse_preview( parent ) );
		
		//Children
	//	for( auto it : doc.select_nodes( "//div[@id='child-preview']/span" ) )
	//		post.children.push_back( parse_preview( it.node() ) );
		
		//Score
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
		
		//Tags
		for( auto tag : doc.select_nodes( "//ul[@id='tag-sidebar']/li" ) ){
			Tag t = parse_tag( tag.node() );
			post.tags.add( t.id );
			tag_handler.add( t );
		}
		
		//Favorites
	//	for( auto it : doc.select_nodes( "//span[@id='favorited-by']/a" ) )
	//		post.fans.push_back( it.node().child_value() );
	//	for( auto it : doc.select_nodes( "//span[@id='remaining-favs']/a" ) )
	//		post.fans.push_back( it.node().child_value() );
	//	cout << "Amount of fans: " << post.fans.size() << "\n";
	}
	else{
		cout << "XHTML parsing failed!\n";
		cout << "Error: " << result.description();
	}
	
	post_handler.add( post );
	return post;
}

Index SanApi::get_index( string search, int page, int limit ){
	Index index({ search, page, limit });
	string url = get_url() + "?tags=" + Server::remove_reserved( Server::encode_str( search ) );
	if( page > 1 )
		url += "&page=" + to_string( page );
	string html = CleanHTML( get_from_url( url ) );
	
	xml_document doc;
	xml_parse_result result = doc.load( html.c_str() );
	
	if( result.status == status_ok ){
		
		//div.content div span.thumb
		
		xpath_node_set spans = doc.select_nodes( "//div[@class='content']/div/span[contains(concat(' ', normalize-space(@class), ' '), ' thumb ')]" );
		cout << "Spans: " << spans.size() << "\n";
		for( xpath_node f : spans ){
			index.posts.push_back( parse_preview( f.node() ) );
		}
		
		
		xpath_node_set tags = doc.select_nodes( "//ul[@id='tag-sidebar']/li" );
		for( xpath_node tag : tags ){
			Tag t = parse_tag( tag.node() );
			tag_handler.add( t );
			index.related_tags.add( t.id );
		}

		index.amount = -1;
	}
	else{
		cout << "XHTML parsing failed!\n";
		cout << "Error: " << result.description();
	}
	
	return index;
}

