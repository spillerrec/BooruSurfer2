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

#include "Styler.hpp"

using namespace std;
using namespace HTML;

#include "../objects/Post.hpp"
#include "../objects/Tag.hpp"

#include <algorithm>
#include <boost/lexical_cast.hpp>


Styler::Styler( string page_title ){
	doc.html()(
			head(
					title(doc)( page_title )
				,	link( doc, REL( "stylesheet" ), HREF( "/file/main.css" ) )
			//	,	"<!--[if lt IE 9]><script src=\"/style/ie8.js\"></script><![endif]-->" //TODO:
				)
		,	body(
					header(doc)(
							nav
						)
				,	container
				)
		);
}


string Styler::format_filesize( unsigned filesize ) const{
	string endings[] = { "bytes", "KiB", "MiB", "GiB", "TiB", "PiB" };
	double size = filesize;
	
	for( auto ending : endings ){
		if( size > 1024 )
			size /= 1024;
		else{
			//TODO: decimals
			return boost::lexical_cast<string>( size ) + " " + ending;
		}
	}
	
	return "will crash your computer";
}

string Styler::format_date( unsigned unix_time ) const{
	return "unimplemented";
}


Node Styler::time( unsigned unix_time ){
	
}


Node Styler::tag_search(){
	return input( doc ,
			ID( "search" )
		,	TYPE( "search" )
		,	NAME( "tags" )
		,	PLACEHOLDER( "Tag query" )
		);
	
	//TODO: create form
	
}

Node Styler::main_navigation( string search ){
	
}


Node Styler::tag( const Tag& tag ){
	string url = "/index/dan/" + tag.name + "/"; //TODO: make url class
	
	string text = tag.name;
	replace( text.begin(), text.end(), '_', ' ' );
	
	//TODO: use real_count
	if( tag.count )
		text += " (" + boost::lexical_cast<string>( tag.count ) + ")";
	
	if( tag.type )
		return a( doc, HREF( url ), CLASS( "tagtype" + to_string( tag.type ) ) )( text );
	else
		return a( doc, HREF( url ) )( text );
}

void Styler::tag_list( Node& parent, const vector<Tag>& tags, string title ){
	if( !title.empty() )
		parent( h3(doc)( title ) );
	
	auto list = ul( doc );
	for( Tag tag : tags )
		list( li(doc)( this->tag( tag ) ) );
	
	parent( list );
}


Node Styler::note( const Note& note ){
	return p(doc)( "unimplemented" );
}

Node Styler::comment( const Comment& comment ){
	return p(doc)( "unimplemented" );
}


Node Styler::post_preview( const Post& post ){
	return p(doc)( "unimplemented" );
}



Node Styler::post_thumb( const Post& post ){
	string url = "/post/dan/" + boost::lexical_cast<string>( post.id ) + "/"; //TODO: make url class
	auto link = a( doc, HREF(url) )(
		img( doc, SRC( post.thumbnail.url ), ALT( "thumbnail" ) )
		);
	
	return link;
	//TODO: set status class on a
}

Node Styler::post_thumb_info( const Post& post, bool extended ){
	return p(doc)( "unimplemented" );
}

Node Styler::post_details( const Post& post ){
	return p(doc)( "unimplemented" );
}


Node Styler::post( Post post ){
	string url_org = "/proxy/dan/original/" + boost::lexical_cast<string>( post.id );
	string url_show = "/proxy/dan/sample/" + boost::lexical_cast<string>( post.id );
	
	return section( doc, CLASS("post") )(
			div( doc, CLASS("container") )(
					a( doc, HREF(url_org) )(
						img( doc, SRC(url_show), ALT("preview") )
					)
				)
		);
}

Node Styler::post_list( std::vector<Post> posts ){
	return p(doc)( "unimplemented" );	
	/*
	html_node list_container = element( parent, "section", "class", "post_list size_medium" );
	html_node list = element( list_container, "ul" );
	
	for( Post post : posts )
		post_thumb( element( list, "li" ), post );
		
	return list_container;
	*/
}

