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
using namespace pugi;
using namespace html;

typedef Styler::html_node html_node;

#include "../objects/Post.hpp"
#include "../objects/Tag.hpp"

#include <algorithm>
#include <boost/lexical_cast.hpp>


Styler::Styler( string title ) : doc( title ){
	doc.add_stylesheet( "/file/main.css" );
	
	//Navigation
	nav = element( element( doc.get_body(), "header" ), "nav" );
	//TODO: add stuff here
	
	container = div( doc.get_body(), "id", "container" );
	//element( container, "aside", "class", "post_list_info" ).text().set( " " );
}


string Styler::format_filesize( unsigned filesize ) const{
	string endings[] = { "bytes", "KiB", "MiB", "GiB", "TiB", "PiB" };
	double size = filesize;
	
	for( string ending : endings ){
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


html_node Styler::time( unsigned unix_time ) const{
	
}


html_node Styler::tag_search( const html_node& parent ) const{
	html_node input = attr( attr( attr( attr( element( parent, "input" )
		,	"id", "search"
		),	"type", "search"
		),	"name", "tags"
		),	"placeholder", "Tag query"
		);
	
	//TODO: create form
	
}

html_node Styler::main_navigation( const html_node& parent, string search ) const{
	
}


html_node Styler::tag( html_node parent, const Tag& tag ) const{
	string url = "/index/dan/" + tag.name + "/"; //TODO: make url class
	
	string text = tag.name;
	replace( text.begin(), text.end(), '_', ' ' );
	
	//TODO: use real_count
	if( tag.count )
		text += " (" + boost::lexical_cast<string>( tag.count ) + ")";
	
	html_node a = link( parent, url, text );
	
	if( tag.type )
		attr( a, "class", "tagtype" + boost::lexical_cast<string>( tag.type ) );
	
	return a;
}

html_node Styler::tag_list( html_node parent, const vector<Tag>& tags, string title ) const{
	if( !title.empty() )
		set_text( element( parent, "h3" ), title );
	
	html_node list = element( parent, "ul" );
	for( Tag tag : tags )
		this->tag( element( list, "li" ), tag );
}


html_node Styler::note( html_node parent, const Note& note ) const{
	
}

html_node Styler::comment( html_node parent, const Comment& comment ) const{
	
}


html_node Styler::post_preview( html_node parent, const Post& post ) const{
	
}



html_node Styler::post_thumb( html_node parent, const Post& post ) const{
	string url = "/post/dan/" + boost::lexical_cast<string>( post.id ) + "/"; //TODO: make url class
	html_node a = link( parent, url, "" );
	
	//TODO: set status class on a
	
	return image( a, post.thumbnail.url, "thumbnail" );
}

html_node Styler::post_thumb_info( html_node parent, const Post& post, bool extended ) const{
	
}

html_node Styler::post_details( html_node parent, const Post& post ) const{
	
}


html_node Styler::post( html_node parent, Post post ) const{
	html_node container = element( element( parent, "section", "class", "post" ), "div", "class", "container" );
	
	string url = "/proxy/dan/original/" + boost::lexical_cast<string>( post.id );
	image( link( container, url, "" ), post.preview.url, "preview" ); //TODO: url
}

html_node Styler::post_list( html_node parent, std::vector<Post> posts ) const{
	html_node list_container = element( parent, "section", "class", "post_list size_medium" );
	html_node list = element( list_container, "ul" );
	
	for( Post post : posts )
		post_thumb( element( list, "li" ), post );
		
	return list_container;
}

