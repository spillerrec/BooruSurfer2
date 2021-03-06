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

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"

#include <algorithm>

::string redirect_page( std::string url ){
	HTML::Document doc;
	
	doc.html()(
		head(doc)(
				title(doc)( "Redirecting" )
			,	link( doc, REL( "stylesheet" ), HREF( "/file/main.css" ) )
		//	,	"<!--[if lt IE 9]><script src=\"/style/ie8.js\"></script><![endif]-->" //TODO:
			,	meta( doc, HTTP_EQUIV("refresh"), CONTENT("0; " + url) )
			)
	,	body(doc)(
				p(doc)( "Redirecting to: " + url )
			)
	);
	
	return doc;
}


//TODO: make url as input to the functions


BasicStyler::BasicStyler( string page_title ){
	doc.html()(
			head(
					title(doc)( page_title )
				,	link( doc, REL( "stylesheet" ), HREF( "/file/main.css" ) )
			//	,	"<!--[if lt IE 9]><script src=\"/style/ie8.js\"></script><![endif]-->" //TODO:
				,	meta( doc, NAME("viewport"), CONTENT("width=device-width, initial-scale=1.0") )
				)
		,	body(
					header(doc)(
							nav
						)
				//,	container
				)
		);
}
Styler::Styler( Api* api, string page_title )
	:	BasicStyler(page_title), api(api), url( api ) { }


string BasicStyler::format_filesize( unsigned filesize ) const{
	string endings[] = { "bytes", "KiB", "MiB", "GiB", "TiB", "PiB" };
	double size = filesize;
	
	for( auto ending : endings ){
		if( size > 1024 )
			size /= 1024;
		else{
			char buf[255];
			snprintf( buf, 255, "%.2f", size );
			return string(buf) + " " + ending;
		}
	}
	
	return "will crash your computer";
}

string BasicStyler::format_date( Time timestamp ) const{
	//TODO: write how long time ago it was
	if( timestamp.isValid() )
		return timestamp.formatSimple();
	else
		return "Unknown";
}


Node BasicStyler::time( unsigned unix_time ){
	return p(doc)( "TODO: BasicStyler::time" ); //TODO:
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
	vector<Api*> apis = ApiHandler::get_instance()->get_apis();
	
	auto list = ul(doc);
	for( auto api : apis ){
		if( api == this->api ) //TODO:
			continue;
		
		string href = UrlHandler(api).index_url( {{search}} ); //TODO: fix
		list( li(doc)( a(doc, HREF( href ) )( api->get_name() ) ) );
	}
	
//	other_actions( li(doc)( a(doc, HREF( "/manage/" ) )( "Settings" ) ) );
	
	return other_actions(//ul(doc)(
			li(doc)(
					api->get_name()
				,	list
				)
		,	li(doc)( a(doc, HREF(url.index_url()) )( "Index" ) )
		,	li(doc)( tag_search() ) //TODO: search
	, li(doc)( a(doc, HREF( "/manage/" ) )( "Settings" ) )//	,	li(doc)( "Other", other_actions )
		);
}


Node Styler::tag( const Tag& tag ){
	string url = this->url.index_url( vector<Tag>{ tag } );
	
	string text = tag.id;
	replace( text.begin(), text.end(), '_', ' ' );
	auto node = a( doc, HREF( url ) )( text );
	
	//TODO: use real_count
	if( tag.count )
		node( span( doc )( "(" + std::to_string( tag.count ) + ")" ) );
	
	if( tag.type )
		node.add_attr( CLASS( "tagtype" + to_string( tag.type ) ) );
	
	return node;
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
	auto x      = note.x      * 100;
	auto y      = note.y      * 100;
	auto width  = note.width  * 100;
	auto height = note.height * 100;
	
	string style = "";
	style += "left:"   + to_string( x      ) + "%;";
	style += "top:"    + to_string( y      ) + "%;";
	style += "width:"  + to_string( width  ) + "%;";
	style += "height:" + to_string( height ) + "%;";
	
	return div( doc, STYLE( style ) )( div( doc )( note.content ) );
}

Node Styler::comment( const Comment& comment ){
	return p(doc)( "unimplemented" );
}


Node Styler::post_preview( const Post& post ){
	return p(doc)( "unimplemented" );
}



Node Styler::post_thumb( const Post& post ){
	string url = this->url.post_url( post );
	string classes;
	if( post.saved )
		classes += "saved";
	auto link = a( doc, HREF(url), CLASS( classes ) )(
		img( doc, SRC( this->url.image_url( post, Image::THUMB ) ), ALT( "thumbnail" ) )
		);
	
	return link;
	//TODO: set status class on a
}

Node Styler::post_thumb_info( const Post& post, bool extended ){
	auto node = div( doc, CLASS("details") );
	post_info( node, post, extended );
	
	auto tags = p(doc);
	tags( em(doc)( "Tags:" ) );
	
	string tag_text;
	for( auto t : post.tags.list ){
		Tag tag = api->tag_handler.get( t );
		if( tag.type != Tag::NONE )
			tags( span(doc, CLASS("tagtype" + to_string(tag.type) ))( t, " " ) );
		else
			tags( t + " " );
	}
	
	return node( tags );
}

Node Styler::post_details( const Post& post ){
	return p(doc)( "unimplemented" );
}

Node postInternal( Document& doc, Post& p, UrlHandler& url ){
	string url_org  = url.image_url( p, Image::ORIGINAL );
	string url_show = url.image_url( p, Image::RESIZED );
	if( p.full.isLocal() )
		url_show = url_org;
	
	//Determine file extension from url, which may contain arguments
	auto end = url_org.find( "?" );
	auto start = url_org.rfind( ".", end );
	auto ext = url_org.substr( start+1, end - start );
	
	if( ext == "webm" || ext == "flv" || ext == "mp4" || ext == "mkv" ) //NOTE: perhaps a bit too much...
		return video( doc, SRC(url_org), LOOP("loop"), CONTROLS("controls"), AUTOPLAY("autoplay") );
	
	if( ext == "svg" )
		url_show = url_org; //Just use img tag, but do use the SVG, as it is usually smaller
	
	if( ext == "swf" )
		return object( doc, TYPE( "application/x-shockwave-flash" ), DATA( url_org )
				, WIDTH( to_string( p.full.width ) ), HEIGHT( to_string( p.full.height ) )
				)(	param( doc, NAME( "movie" ), VALUE( url_org ) ) );
	
	return a( doc, HREF(url_org) )(
			img( doc, SRC(url_show), ALT("preview") )
		);
}

Node Styler::post( Post post ){
	auto container = div( doc, CLASS("container") )(
			postInternal( doc, post, url )
		);
	
	for( auto n : post.notes.list ){
		auto note = api->note_handler.get( n );
		container( this->note( note ) );
	}
	
	return section( doc, CLASS("post") )( container );
}

string to_string( Post::Rating rating ){
	switch( rating ){
		case Post::SAFE: return "Safe";
		case Post::QUESTIONABLE: return "Questionable";
		case Post::EXPLICIT: return "Explicit";
		default: return "Unknown";
	}
}

Node Styler::post_info( Node& parent, const Post& post, bool extended ){
	auto add = [=]( string title, string content ){
			return p(doc)(
						em(doc)( title )
					,	content
				);
		};
	
	parent( add( "Posted:", format_date( post.creation_time ) ) );
	if( extended && !post.author.empty() )
		parent( add( "By:", post.author ) );
	if( post.full.width && post.full.height )
		parent( add( "Dimensions:", to_string(post.full.width) + "x" + to_string(post.full.height) ) );
	if( post.full.size )
		parent( add( "Size:", format_filesize(post.full.size) ) );
	if( post.rating != Post::UNRATED )
		parent( add( "Rating:", to_string( post.rating ) ) );
	if( extended && !post.source.empty() )
		parent( add( "Source:", post.source ) );
	if( extended )
		parent( p(doc)( a(doc, HREF(api->original_post_url(post.id)))( "Site link" ) ) );
	
	return parent; //TODO: How is this used?
}

Node Styler::post_list( std::vector<Post> posts ){
	auto list = ul(doc);
	for( Post post : posts )
		list( li(doc)( post_thumb( post ), post_thumb_info( post, false ) ) );
	
	return list;
}

Node Styler::post_list_section( std::vector<Post> posts )
	{ return section( doc, CLASS("post_list size_medium") )( post_list( posts ) ); }

Node Styler::index_navigation( string search, int page, int limit, int amount ){
	int min_page = max( page - 3, 1 );
	int max_page = min( page + 3, amount );
	
	auto link = [=]( int page ){
			return li(doc)(
					a(doc, HREF( url.index_url( {{search}}, page, limit ) ))(
							to_string(page)
						)
				);
		};
	
	auto list = ul(doc);
	
	// 1, ..., min, min+1
	if( min_page > 1 ){
		list( link( 1 ) );
		list( li(doc)( "..." ) );
	}
	
	// min, min+1 ... max-1, max
	for( int i=min_page; i<=max_page; i++ )
		if( i == page )
			list( li(doc)( to_string(page) ) );
		else
			list( link( i ) );
	
	// max, ..., amount
	if( max_page < amount ){
		list( li(doc)( "..." ) );
		list( link( amount ) );
	}
	
	return HTML::nav( doc, CLASS("page_nav") )( list );
}

