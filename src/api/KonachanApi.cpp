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

#include "KonachanApi.hpp"

const char* const KonachanApi::kona_post_strings[] = {
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
	NULL,
};

