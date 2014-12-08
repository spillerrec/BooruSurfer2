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

#include <Poco/Timestamp.h>

#include "Tag.hpp"
#include "Image.hpp"
#include "Resource.hpp"
#include "Identity.hpp"
#include "Note.hpp"
#include "Comment.hpp"
#include "Pool.hpp"

#include <string>
#include <vector>

class Post : public Identity<>{
	public:
		enum Rating{
			SAFE,
			QUESTIONABLE,
			EXPLICIT,
			UNRATED = 255
		};
		
		enum Status{
			NORMAL,
			PENDING,
			FLAGGED
		};
		
	public:
		Post(){ id = 0; }
		Post( ID_T id ){ this->id = id; }
		
		std::string hash;
		std::string author;
		//TODO: creation_date
		Poco::Timestamp creation_time{ 0 };
		
		
		Resource<Tag> tags;
		Resource<Post> parents;
		Resource<Post> children;
		Resource<Note> notes;
		Resource<Comment> comments;
		Resource<Pool> pools;
		
		std::string source;
		
		int score = 0;
		Rating rating = UNRATED;
		
		Image full;
		Image reduced;
		Image preview;
		Image thumbnail;
		
		Image get_image_size( Image::Size size ) const;
		
		
	public:
		unsigned get_id() const{ return id; }
		std::string get_hash() const{ return hash; }
		std::string get_author() const{ return author; }
		
};

#endif

