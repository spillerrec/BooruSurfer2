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
#include "../html/HtmlDocument.hpp"

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


Post DanApi::get_post( unsigned id ) const{
	DataNode data = JsonDataNode::from_string( get_from_url( "https://yande.re/post/index.json?tags=touhou" ) );
	
	
	if( data.size() > 0 ){
		DataNode p = data[0];
		
		Post post;
		post.id = int( p[ post_table()[ POST_ID ] ] );
		post.author = p[ post_table()[ AUTHOR ] ].as_string();
		post.hash = p[ post_table()[ HASH ] ].as_string();
		post.score = p[ post_table()[ SCORE ] ];
		
		post.url = p[ post_table()[ URL ] ].as_string();
		
		
		post.tags = get_resource<Tag, std::string>( p, TAGS );
		post.parents = get_resource<Post>( p, PARENT_ID );
		post.children = get_resource<Post>( p, CHILDREN );
		post.notes = get_resource<Note>( p, NOTES );
		post.comments = get_resource<Comment>( p, COMMENTS );
		post.pools = get_resource<Pool>( p, POOLS );
		
		return post;
	}
	
	return Post();
}

