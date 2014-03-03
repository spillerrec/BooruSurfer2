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

Image DanApi::get_image( DataNode parent, DanApi::PostItem url, DanApi::PostItem width, DanApi::PostItem height, DanApi::PostItem size ) const{
	Image img;
	img.url = "";
	img.width = 0;
	img.height = 0;
	img.size = 0;
	
	if( post_table()[ url ] )
		img.url = parent[ post_table()[ url ] ].as_string();
	if( post_table()[ width ] )
		img.width = parent[ post_table()[ width ] ].as_int();
	if( post_table()[ height ] )
		img.height = parent[ post_table()[ height ] ].as_int();
	if( post_table()[ size ] )
		img.size = parent[ post_table()[ size ] ].as_int();
	
	return img;
}


Post DanApi::parse_post( DataNode p ) const{
	Post post;
	post.id = int( p[ post_table()[ POST_ID ] ] );
	post.author = p[ post_table()[ AUTHOR ] ].as_string();
	post.hash = p[ post_table()[ HASH ] ].as_string();
	post.score = p[ post_table()[ SCORE ] ];
	post.source = p[ post_table()[ SOURCE ]].as_string();
	
	post.tags = get_resource<Tag, std::string>( p, TAGS );
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


Post DanApi::get_post( unsigned id ){
	std::string url = get_url() + "post/index.json?tags=id:" + std::to_string( id );
	DataNode data = JsonDataNode::from_string( get_from_url( url ) );
	
	if( data.size() > 0 )
		return parse_post( data[0] );
	else
		return Post();
}

std::vector<Post> DanApi::get_index( std::string search, int page, int limit ){
	std::string url = get_url() + "post/index.json?tags=" + search;
	if( page > 1 )
		url += "&page=" + std::to_string( page );
	
	if( limit < 1 )
		limit = 25;
	url += "&limit=" + std::to_string( limit );
	
	DataNode data = JsonDataNode::from_string( get_from_url( url ) );
	
	std::vector<Post> list;
	
	for( DataNode node : data )
		list.push_back( parse_post( node ) );
	
	return list;
}

