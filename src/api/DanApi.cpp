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

#include "DanApi.hpp"

#include "../parsing/JsonDataNode.hpp"
#include "../parsing/NullDataNode.hpp"

#include <fstream>
using namespace std;

const char* const DanApi::post_strings[] = {
	"id",
	"md5",
	"author",
	"created_at",
	"parent_id",
	"has_children",
	"has_notes", //notes
	"has_comments",
	NULL,
	"source",
	"tags",
	"score",
	"rating",
	"status",
	
	"file_url",
	"width",
	"height",
	"file_size",
	
	"preview_url",
	"preview_width",
	"preview_height",
	NULL,
	
	"sample_url",
	"sample_width",
	"sample_height",
	NULL,
	
	NULL,
	NULL,
	NULL,
	NULL
};

void DanApi::load_tag_file(){
	DataNode data = JsonDataNode::from_file( "tags/" + get_shorthand() + ".json" );
	if( data )
		tag_handler.addAll( data.begin(), data.end(), [&](DataNode n){ return parse_tag(n); } );
	else
		cout << "Could not load tags for " << get_shorthand() << '\n';
}

Image DanApi::get_image( DataNode parent, DanApi::PostItem url, DanApi::PostItem width, DanApi::PostItem height, DanApi::PostItem size ) const{
	auto asInt = [&]( PostItem item )
		{ return post_table()[ item ] ? parent[ post_table()[ item ] ].as_int() : 0; };
	
	Image img;
	img.url = post_table()[ url ] ? parent[ post_table()[ url ] ].as_string() : "";
	img.width  = asInt( width );
	img.height = asInt( height );
	img.size   = asInt( size );
	
	if( img.url.size() > 0 && img.url[0] == '/' )
		img.url.replace( 0, 1, get_url() );
	
	return img;
}

Post DanApi::parse_post( DataNode p ) const{
	Post post;
	post.id = int( p[ post_table()[ POST_ID ] ] );
	post.author = p[ post_table()[ AUTHOR ] ].as_string();
	post.hash = p[ post_table()[ HASH ] ].as_string();
	post.score = p[ post_table()[ SCORE ] ];
	post.source = p[ post_table()[ SOURCE ]].as_string();
	
	post.creation_time = Time::FromUnixTime( p[ post_table()[ CREATED ] ].as_int() );
	
	post.tags = get_resource<Tag>( p, TAGS );
	post.parents = get_resource<Post>( p, PARENT_ID );
	post.children = get_resource<Post>( p, CHILDREN );
	post.notes = get_resource<Note>( p, NOTES );
	post.comments = get_resource<Comment>( p, COMMENTS );
	post.pools = get_resource<Pool>( p, POOLS );
	
	post.full = get_image( p, URL, WIDTH, HEIGHT, SIZE );
	post.thumbnail = get_image( p, THUMB_URL, THUMB_WIDTH, THUMB_HEIGHT, THUMB_SIZE );
	post.preview = get_image( p, PREVIEW_URL, PREVIEW_WIDTH, PREVIEW_HEIGHT, PREVIEW_SIZE );
	post.reduced = get_image( p, REDUCED_URL, REDUCED_WIDTH, REDUCED_HEIGHT, REDUCED_SIZE );
	
	return post;
}

Tag::Type DanApi::tag_type_id( int id ) const{
	switch( id ){
		case 0: return Tag::NONE;
		case 1: return Tag::ARTIST;
		case 3: return Tag::COPYRIGHT;
		case 4: return Tag::CHARACTER;
		default: return Tag::UNKNOWN;
	}
}

Tag DanApi::parse_tag( DataNode node ) const{
	Tag t;
	t.id = node[ "name" ].as_string();
	t.type = tag_type_id( node[ "type" ].as_int() );
	t.count = node[ "count" ].as_int();
	return t;
}

Post DanApi::get_post( unsigned id, Image::Size level ){
	Post post;
	if( post_handler.get_checked( id, post, level ) )
		return post;
	
	string url = get_url() + "post/index.json?tags=id:" + to_string( id );
	DataNode data = JsonDataNode::from_string( get_from_url( url ) );
	
	if( data.size() > 0 ){
		post = parse_post( data[0] );
		post_handler.add( post );
		return post;
	}
	else
		return Post();
}

Index DanApi::get_index( string search, int page, int limit ){
	Index index({ search, page, limit });
	string url = get_url() + "post/index.json?tags=" + Server::remove_reserved( Server::encode_str( search ) );
	if( page > 1 )
		url += "&page=" + to_string( page );
	
	if( limit < 1 )
		limit = 25;
	url += "&limit=" + to_string( limit );
	
	DataNode data = JsonDataNode::from_string( get_from_url( url ) );
	
	for( DataNode node : data ){
		auto post = parse_post( node );
		post_handler.add( post );
		index.posts.push_back( post );
	}
	
	
	//Related tags
	if( search.empty() ){
		string related_url = get_url() + "tag/index.json?order=count";
		DataNode related_data = JsonDataNode::from_string( get_from_url( related_url ) );
		
		for( auto related : related_data ){
			Tag t = parse_tag( related );
			tag_handler.add( t );
			index.related_tags.add( t.id );
		}
	}
	else{
		string related_url = get_url() + "tag/related.json?tags=" + Server::remove_reserved( Server::encode_str( search ) );
		DataNode related_data = JsonDataNode::from_string( get_from_url( related_url ) );
		
		//Determine first tag
		string first_tag = search;
		auto pos = first_tag.find( ' ' );
		if( pos != string::npos )
			first_tag = first_tag.substr( 0, pos );
		
		//Get data
		//TODO: related[1] contains the count, support?
		for( auto related : related_data[first_tag] )
			index.related_tags.add( related[0].as_string() );
	}
	
	return index;
}

