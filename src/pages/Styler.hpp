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

#ifndef STYLER_H
#define STYLER_H

#include "Styler.hpp"

#include "../parsing/html.hpp"
#include "UrlHandler.hpp"

#include <vector>

class Post;
class Tag;
class Comment;
class Pool;
class Note;
class Image;

class Styler{
	public:
		HTML::Document doc;
		HTML::Node head{ HTML::head(doc) };
		HTML::Node body{ HTML::body(doc) };
		HTML::Node nav{ HTML::nav(doc) };
		HTML::Node container{ HTML::div( doc, HTML::ID("container") ) };
		
		UrlHandler url;
	
	public:
		Styler( std::string site, std::string title );
		
		std::string format_filesize( unsigned filesize ) const;
		std::string format_date( unsigned unix_time ) const;
		
		HTML::Node time( unsigned unix_time );
		
		HTML::Node tag_search();
		HTML::Node main_navigation( std::string search );
		
		HTML::Node tag( const Tag& tag );
		void tag_list( HTML::Node& parent, const std::vector<Tag>& tags, std::string title="" );
		
		HTML::Node note( const Note& note );
		HTML::Node comment( const Comment& comment );
		
		HTML::Node post_preview( const Post& post );
		
		
		HTML::Node post_thumb( const Post& post );
		HTML::Node post_thumb_info( const Post& post, bool extended=false );
		HTML::Node post_details( const Post& post );
		
		HTML::Node post( Post post );
		HTML::Node post_list( std::vector<Post> list );
		
		//HTML::Node page_index_nav( $index, $page );
};

#endif

