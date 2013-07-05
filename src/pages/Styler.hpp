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

#include <vector>

class Post;
class Tag;
class Comment;
class Pool;
class Note;
class Image;

class Styler{
	public:
		html::Document doc;
		html::Node nav;
		html::Node container;
	
	public:
		Styler( std::string title );
		
		std::string format_filesize( unsigned filesize ) const;
		std::string format_date( unsigned unix_time ) const;
		
		html::Node time( unsigned unix_time ) const;
		
		html::Node tag_search() const;
		html::Node main_navigation( std::string search ) const;
		
		html::Node tag( const Tag& tag ) const;
		html::Node tag_list( const std::vector<Tag>& tags, std::string title="" ) const;
		
		html::Node note( const Note& note ) const;
		html::Node comment( const Comment& comment ) const;
		
		html::Node post_preview( const Post& post ) const;
		
		
		html::Node post_thumb( const Post& post ) const;
		html::Node post_thumb_info( const Post& post, bool extended=false ) const;
		html::Node post_details( const Post& post ) const;
		
		html::Node post( Post post ) const;
		html::Node post_list( std::vector<Post> list ) const;
		
		//html::Node page_index_nav( $index, $page ) const;
};

#endif

