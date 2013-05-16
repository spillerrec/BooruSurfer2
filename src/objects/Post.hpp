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

#ifndef POST_H
#define POST_H

#include "Tag.hpp"
#include "Image.hpp"
#include "Resource.hpp"
#include "Note.hpp"
#include "Comment.hpp"
#include "Pool.hpp"

#include <string>

class Post{
	typedef unsigned ID_T;
	
	public:
		enum Rating{
			SAFE,
			QUESTIONABLE,
			EXPLICT,
			UNRATED = 255
		};
		
		enum Status{
			NORMAL,
			PENDING,
			FLAGGED
		};
		
	public:
		Post() : tags( false ), parents( false ), children( false ), notes( false ), comments( false ), pools( false ){ }
		
		//Id<Site,unsigned> ?
		unsigned id;
		std::string hash;
		std::string author;
		//TODO: creation_date
		
		std::string url; //temp
		
		
		Resource<Tag, std::string> tags;
		Resource<Post> parents;
		Resource<Post> children;
		Resource<Note> notes;
		Resource<Comment> comments;
		Resource<Pool> pools;
		
		std::string source;
		
		int score;
		Rating rating;
		
		Image full;
		Image compressed;
		Image prevew;
		Image thumbnail;
		
		
	public:
		unsigned get_id() const{ return id; }
		std::string get_hash() const{ return hash; }
		std::string get_author() const{ return author; }
		
};

#endif

