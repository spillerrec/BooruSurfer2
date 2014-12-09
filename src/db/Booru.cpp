/*	This file is part of rss2rss.

	rss2rss is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	rss2rss is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with rss2rss.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Booru.hpp"


Booru::Booru( std::string site ) : db( "cache.sqlite" ), site(site){
	std::string query_tags = "CREATE TABLE IF NOT EXISTS " + site + "_tags ( "
			+	"id TEXT PRIMARY KEY, "
			+	"count INTEGER, "
			+	"type INTEGER, "
			+	"ambiguous INTEGER "
			+	")";
	std::string query_index_post = "CREATE TABLE IF NOT EXISTS " + site + "_index_post ( "
			+	"list, offset, post )";
	std::string query_index_list = "CREATE TABLE IF NOT EXISTS " + site + "_index_list ( "
			+	"id INTEGER PRIMARY KEY, "
			+	"search TEXT, "
			+	"count INTEGER, "
			+	"next_update INTEGER, "
			+	"ordered INTEGER, "
			+	"related_tags TEXT, "
			+	"related_counts TEXT, "
			+	"locked INTEGER "
			+	")";
	std::string query_comments = "CREATE TABLE IF NOT EXISTS " + site + "_comments ( "
			+	"id INTEGER PRIMARY KEY, "
			+	"post_id INTEGER, "
			+	"creator TEXT, "
			+	"created_at INTEGER, "
			+	"score INTEGER, "
			+	"body TEXT "
			+	")";
	std::string query_notes = "CREATE TABLE IF NOT EXISTS " + site + "_notes ( "
			+	"id INTEGER PRIMARY KEY, "
			+	"post_id INTEGER, "
			+	"x INTEGER, "
			+	"y INTEGER, "
			+	"width INTEGER, "
			+	"height INTEGER, "
			+	"body TEXT, "
			+	"created_at INTEGER, "
			+	"updated_at INTEGER, "
			+	"version INTEGER "
			+	")";
	std::string query_posts = "CREATE TABLE IF NOT EXISTS " + site + "_posts ( "
			+	"id INTEGER PRIMARY KEY, "
			+	"hash TEXT, "
			+	"created_at INTEGER, "
			+	"uploader TEXT, "
			+	"source TEXT, "
			+	"rating INTEGER, "
			+	"parent_id INTEGER, "
			+	"children_updated INTEGER, "
			+	"pools TEXT, "
			+	"notes_updated INTEGER, "
			+	"comments_updated INTEGER, "
			+	"score NUMERIC, "
			+	"score_count INTEGER, "
			+	"status INTEGER, "
			+	"tags TEXT, "
			+	"post_updated INTEGER, "
			
			+	"url TEXT, "
			+	"width INTEGER, "
			+	"height INTEGER, "
			+	"filesize INTEGER, "
			
			+	"preview_url TEXT, "
			+	"preview_width INTEGER, "
			+	"preview_height INTEGER, "
			+	"preview_filesize INTEGER, "
			
			+	"thumb_url TEXT, "
			+	"thumb_width INTEGER, "
			+	"thumb_height INTEGER, "
			+	"thumb_filesize INTEGER, "
			
			+	"reduced_url TEXT, "
			+	"reduced_width INTEGER, "
			+	"reduced_height INTEGER, "
			+	"reduced_filesize INTEGER, "
			
			+	"local INTEGER "
			+	")";
	Statement( db, query_tags.c_str() ).next();
	Statement( db, query_index_post.c_str() ).next();
	Statement( db, query_index_list.c_str() ).next();
	Statement( db, query_comments.c_str() ).next();
	Statement( db, query_notes.c_str() ).next();
	Statement( db, query_posts.c_str() ).next();
}


Booru::~Booru(){
	delete load_tags;
	delete save_tags;
	delete load_posts;
	delete save_posts;
}

Statement& Booru::loadTags(){
	if( !load_tags )
		load_tags = new Statement( db, ("SELECT * FROM " + site + "_tags WHERE id = ?1").c_str() );
	load_tags->reset();
	return *load_tags;
}

Statement& Booru::saveTags(){
	if( !save_tags ){
		save_tags = new Statement( db, ("INSERT OR REPLACE INTO " + site + "_tags VALUES( ?1, ?2, ?3, ?4 )").c_str() );
	}
	save_tags->reset();
	return *save_tags;
}


Statement& Booru::loadPosts(){
	if( !load_posts ){
		load_posts = new Statement( db, "SELECT * FROM ?2 WHERE id = ?1" );
		load_posts->bind( site + "_posts", 2 );
	}
	return *load_posts;
}

Statement& Booru::savePosts(){
	if( !save_posts ){
		save_posts = new Statement( db, "INSERT INTO ?99 VALUES( "
				" ?1,  ?2,  ?3,  ?4,  ?5,  ?6,  ?7,  ?8,  ?9, ?10,"
				"?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20,"
				"?21, ?22, ?23, ?24, ?25, ?26, ?27, ?28, ?29, ?30,"
				"?31, ?32, ?33 )"
			);
		save_posts->bind( site + "_posts", 99 );
	}
	return *save_posts;
}


#include <iostream>
		
bool Booru::load( Post& p ){
	return false;
/*	auto stmt = loadPosts();
	stmt.reset();
	stmt.bind( p.id, 1 );
	stmt.bind( site + "_posts", 2 );
	
	if( stmt.next() )
		return false;
	
	p.hash = stmt.text( 1 );
	p.creation_time = stmt.integer64( 2 );
	p.author = stmt.text( 3 );
		std::string source;
		Rating rating = UNRATED;
		Resource<Post> parents;
	
		
		
		Resource<Tag> tags;
		Resource<Post> children;
		Resource<Note> notes;
		Resource<Comment> comments;
		Resource<Pool> pools;
		
		
		int score = 0;
		
		Image full;
		Image reduced;
		Image preview;
		Image thumbnail;
	
	return true;*/
}

bool Booru::load( Tag& p ){
	std::cout << "opening tag: " << p.id << std::endl;
	auto stmt = loadTags();
	stmt.bind( p.id, 1 );
	if( stmt.next() ){
		p.count = stmt.integer( 1 );
		p.type = (Tag::Type)stmt.integer( 2 );
		return true;
	}
	else
		return false;
	std::cout << "closing tag: " << p.id << std::endl;
}

void Booru::save( Post& p ){
	
}

void Booru::save( Tag& p ){
	auto stmt = saveTags();
	std::cout << p.id << std::endl;
	stmt.bind( p.id, 1 );
	stmt.bind( (int)p.count, 2 );
	stmt.bind( p.type, 3 );
	stmt.bind( false, 4 );
	stmt.next();
}

