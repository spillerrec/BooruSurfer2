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

#include "../objects/Post.hpp"
#include "../objects/Tag.hpp"

#include <algorithm>
#include <boost/lexical_cast.hpp>


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


Styler::html_node Styler::time( unsigned unix_time ) const{
	
}


Styler::html_node Styler::tag_search( const Styler::html_node& parent ) const{
	html_node input = attr( attr( attr( attr( element( parent, "input" )
		,	"id", "search"
		),	"type", "search"
		),	"name", "tags"
		),	"placeholder", "Tag query"
		);
	
	//TODO: create form
	
}

Styler::html_node Styler::main_navigation( const Styler::html_node& parent, string search ) const{
	
}


Styler::html_node Styler::tag( Styler::html_node parent, const Tag& tag ) const{
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

Styler::html_node Styler::tag_list( Styler::html_node parent, const vector<Tag>& tags, string title ) const{
	if( !title.empty() )
		set_text( element( parent, "h3" ), title );
	
	html_node list = element( parent, "ul" );
	for( Tag tag : tags )
		this->tag( element( list, "li" ), tag );
}


Styler::html_node Styler::note( Styler::html_node parent, const Note& note ) const{
	
}

Styler::html_node Styler::comment( Styler::html_node parent, const Comment& comment ) const{
	
}


Styler::html_node Styler::post_preview( Styler::html_node parent, const Post& post ) const{
	
}



Styler::html_node Styler::post_thumb( Styler::html_node parent, const Post& post ) const{
	string url = "/post/dan/" + boost::lexical_cast<string>( post.id ) + "/"; //TODO: make url class
	html_node a = link( parent, url, "" );
	
	//TODO: set status class on a
	
	return image( a, post.thumbnail.url, "thumbnail" );
}

Styler::html_node Styler::post_info( Styler::html_node parent, const Post& post, bool extended ) const{
	
}

Styler::html_node Styler::post_details( Styler::html_node parent, const Post& post ) const{
	
}

