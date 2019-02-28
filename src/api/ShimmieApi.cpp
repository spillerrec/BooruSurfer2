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

#include "ShimmieApi.hpp"

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

/*
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
}*/

static Post parse_preview( MyHtml::Tree& tree, MyHtml::Node div ){
	auto img = div.child( tree.tag("a") ).child( tree.tag("img") ).attributes();
	
	//Get post ID
	Post post;
	auto [id_prefix, value] = splitAt( img.valueOf("id"), '_' );
	if( id_prefix != "thumb" )
		throw std::runtime_error("Could not get ID");
	post.id = parseInt( value, 0 );
	
	//Get post thumbnail
	post.thumbnail.url = std::string( img.valueOf("src") );
	post.thumbnail.width  = parseInt( img.valueOf( "width"  ), 0 );
	post.thumbnail.height = parseInt( img.valueOf( "height" ), 0 );
	
	//Add tags
	for( auto property : splitAllOn( div.attributes().valueOf( "data-tags" ), ' ' ) )
		if( !property.empty() )
			post.tags.add( std::string( property ) );
	
	return post;
}

static std::vector<Tag> parse_tags( MyHtml::Tree& tree, MyHtml::Node table ){
	std::vector<Tag> tags;
	
	for( auto tr : table.child(tree.tag("tbody")) ){
		auto info = tr.child();
		auto name = info.next();
		auto count = name.next();
		
		Tag t;
		t.id = std::string( name.child(tree.tag("a")).child().text() );
		for( auto& c : t.id )
			c = (c == ' ') ? '_' : tolower( c );
		t.count = parseInt( count.child(tree.tag("span")).child().text(), 0 );
		
		tags.push_back( t );
	}
	
	return tags;
}


Post ShimmieApi::get_post( unsigned post_id, Image::Size level ){
	Post post;
	if( post_handler.get_checked( post_id, post, level ) )
		return post;
	//main_image
	MyHtml::Html myHtml;
	MyHtml::Tree tree( myHtml );
	auto data = get_from_url( get_url() + "post/view/" + to_string( post_id ) );
	tree.parse( data.c_str(), data.size() );
	
	post.id = post_id;
	
	
	auto img = MyHtml::Search::byAttrValue( tree, "id", "main_image", false ).first();
	post.full.url = std::string( img["src"sv] );
	if( post.full.url.empty() )
		throw logic_error( "No media found in HTML post" );
	
	//TODO: Parse info
	//TODO: comments
	
	//Tags
	for( auto node : MyHtml::Search::byAttrValueSpaceSeperated( tree, "class", "tag_list", false ) ){
		auto tags = parse_tags( tree, node );
		for( auto tag : tags )
		{
			if( !post.tags.contains( tag.id ) )
				post.tags.add( tag.id );
			tag_handler.add( tag );
		}
	}
	
	post_handler.add( post );
	return post;
}

Index ShimmieApi::get_index( string search, int page, int limit ){
	Index index({ search, page, limit });
	string url = get_url() + "post/list/" + Server::remove_reserved( Server::encode_str( search ) );
	url += "/" + to_string( page );
	
	MyHtml::Html myHtml;
	MyHtml::Tree tree( myHtml );
	auto data = get_from_url( url );
	tree.parse( data.c_str(), data.size() );

	for( auto node : MyHtml::Search::byAttrValueSpaceSeperated( tree, "class", "shm-thumb", false ) ){
		auto post = parse_preview( tree, node );
		post_handler.add( post );
		index.posts.push_back( post );
	}
	
	
	for( auto node : MyHtml::Search::byAttrValueSpaceSeperated( tree, "class", "tag_list", false ) ){
		auto tags = parse_tags( tree, node );
		for( auto tag : tags )
		{
			tag_handler.add( tag );
			index.related_tags.add( tag.id );
		}
	}

	index.amount = -1;
	return index;
}

