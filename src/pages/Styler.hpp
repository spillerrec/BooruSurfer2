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

#include "../html/HtmlDocument.hpp"

#include <vector>

class Post;
class Tag;
class Comment;
class Pool;
class Note;
class Image;

class Styler{
	public:
		typedef pugi::xml_node html_node;
	
	public:
		html::HtmlDocument doc;
		html_node nav;
		html_node container;
	
	public:
		Styler( std::string title );
		
		std::string format_filesize( unsigned filesize ) const;
		std::string format_date( unsigned unix_time ) const;
		
		html_node time( unsigned unix_time ) const;
		
		html_node tag_search( const html_node& parent ) const;
		html_node main_navigation( const html_node& parent, std::string search ) const;
		
		html_node tag( html_node parent, const Tag& tag ) const;
		html_node tag_list( html_node parent, const std::vector<Tag>& tags, std::string title="" ) const;
		
		html_node note( html_node parent, const Note& note ) const;
		html_node comment( html_node parent, const Comment& comment ) const;
		
		html_node post_preview( html_node parent, const Post& post ) const;
		
		
		html_node post_thumb( html_node parent, const Post& post ) const;
		html_node post_thumb_info( html_node parent, const Post& post, bool extended=false ) const;
		html_node post_details( html_node parent, const Post& post ) const;
		
		html_node post( html_node parent, Post post ) const;
		html_node post_list( html_node parent, std::vector<Post> list ) const;
		
		//html_node page_index_nav( html_node parent, $index, $page ) const;
};

#endif

