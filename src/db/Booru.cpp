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

#include <boost/algorithm/string.hpp>

#include <Poco/Mutex.h>
#include <iostream>

//Singleton initialization of prepared statement, with automatic reset
static Statement& prepareInstance( Database& db, Statement* stmt, std::string query ){
	if( !stmt ){
		//NOTE: Leak!!
		auto copy = new char[query.size()+1];
		stmt = new Statement( db, strcpy( copy, query.c_str() ) );
	}
	else
		stmt->reset();
	return *stmt;
}

class SiteQueries{
	private:
		Database& db;
		std::string site;
		Statement* load_tags{ nullptr };
		Statement* save_tags{ nullptr };
		
		Statement* load_posts{ nullptr };
		Statement* save_posts{ nullptr };
	
	public:
		SiteQueries( Database& db, std::string site ) : db(db), site(site) { }
		bool isSite( std::string wanted_site ) const { return site == wanted_site; }
		
		Statement& loadTags()
			{ return prepareInstance( db, load_tags, "SELECT * FROM " + site + "_tags WHERE id = ?1" ); }
			
		Statement& saveTags()
			{ return prepareInstance( db, save_tags, "INSERT OR REPLACE INTO " + site + "_tags VALUES( ?1, ?2, ?3, ?4 )" ); }
			
		Statement& loadPosts()
			{ return prepareInstance( db, load_posts, "SELECT * FROM " + site + "_posts WHERE id = ?1" ); }
			
		Statement& savePosts(){
			return prepareInstance( db, save_posts,
					"INSERT OR REPLACE INTO " + site + "_posts VALUES( "
					" ?1,  ?2,  ?3,  ?4,  ?5,  ?6,  ?7,  ?8,  ?9, ?10,"
					"?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20,"
					"?21, ?22, ?23, ?24, ?25, ?26, ?27, ?28, ?29, ?30,"
					"?31, ?32, ?33 )"
				);
		}
};

class DbConnection{
	private:
		Database db;
		std::vector<SiteQueries> sites;
		Poco::Mutex site_lock;
	
	public:
		DbConnection() : db( "cache.sqlite" ) { }
		SiteQueries& getSite( std::string site );
		Database& getDb(){ return db; }
};

SiteQueries& DbConnection::getSite( std::string site ){
	Poco::ScopedLock<Poco::Mutex> locker( site_lock );
	for( auto& query : sites )
		if( query.isSite( site ) )
			return query;
	
	//Construct a new one
	sites.emplace_back( db, site );
	return sites.back();
}

thread_local DbConnection connection;

Booru::Booru( std::string site ) : site(site){
	Database db( "cache.sqlite" );
	std::cout << "Initializing booru: " << site << std::endl;
	auto create = [&](const char* values)
		{ Statement( db, ("CREATE TABLE IF NOT EXISTS " + site + values).c_str() ).next(); };
	
	create( "_tags ("
			"id TEXT PRIMARY KEY, "
			"count INTEGER, "
			"type INTEGER, "
			"ambiguous INTEGER)"
		);
	create( "_index_post ( list, offset, post )" );
	create( "_index_list ("
			"id INTEGER PRIMARY KEY, "
			"search TEXT, "
			"count INTEGER, "
			"next_update INTEGER, "
			"ordered INTEGER, "
			"related_tags TEXT, "
			"related_counts TEXT, "
			"locked INTEGER)"
		);
	create( "_comments ("
			"id INTEGER PRIMARY KEY, "
			"post_id INTEGER, "
			"creator TEXT, "
			"created_at INTEGER, "
			"score INTEGER, "
			"body TEXT)"
		);
	create( "_notes ("
			"id INTEGER PRIMARY KEY, "
			"post_id INTEGER, "
			"x INTEGER, "
			"y INTEGER, "
			"width INTEGER, "
			"height INTEGER, "
			"body TEXT, "
			"created_at INTEGER, "
			"updated_at INTEGER, "
			"version INTEGER)"
		);
	create( "_posts ("
			"id INTEGER PRIMARY KEY, "
			"hash TEXT, "
			"created_at INTEGER, "
			"uploader TEXT, "
			"source TEXT, "
			"rating INTEGER, "
			"parent_id INTEGER, "
			"children_updated INTEGER, "
			"pools TEXT, "
			"notes_updated INTEGER, "
			"comments_updated INTEGER, "
			"score NUMERIC, "
			"score_count INTEGER, "
			"status INTEGER, "
			"tags TEXT, "
			"post_updated INTEGER, "
			
			"url TEXT, "
			"width INTEGER, "
			"height INTEGER, "
			"filesize INTEGER, "
			
			"preview_url TEXT, "
			"preview_width INTEGER, "
			"preview_height INTEGER, "
			"preview_filesize INTEGER, "
			
			"thumb_url TEXT, "
			"thumb_width INTEGER, "
			"thumb_height INTEGER, "
			"thumb_filesize INTEGER, "
			
			"reduced_url TEXT, "
			"reduced_width INTEGER, "
			"reduced_height INTEGER, "
			"reduced_filesize INTEGER, "
			
			"local INTEGER)"
		);
}

Transaction Booru::beginBatch(){ return connection.getDb(); }


std::vector<std::string> splitIds( std::string str ){
	std::vector<std::string> ids;
	boost::split( ids, str, boost::is_any_of( " " ) ); //TODO: avoid is_any_of() ?
	
	std::vector<std::string> output;
	output.reserve( ids.size() );
	for( auto id : ids )
		if( !id.empty() )
			output.emplace_back( id );
	return output;
}

template<typename In>
std::string combineIds( const std::vector<In>& input ){
	std::string output;
	for( auto in : input )
		output += std::to_string( in ) + " ";
	return output;
}
std::string combineIds( const std::vector<std::string>& input ){
	std::string output;
	for( auto in : input )
		output += in + " ";
	return output;
}

bool Booru::load( Post& p ){
	auto& stmt = connection.getSite(site).loadPosts();
	stmt.bind( static_cast<int>(p.id), 1 );
	
	if( stmt.next() ){
		p.hash = stmt.text( 1 );
	//	p.creation_time = Poco::Timestamp::fromEpochTime( stmt.integer64( 2 ) ); //TODO: why linker error?
		p.author = stmt.text( 3 );
		p.source = stmt.text( 4 );
		p.rating = static_cast<Post::Rating>( stmt.integer( 5 ) );
		//TODO: ...
		p.score = stmt.integer( 11 );
		//TODO: ...
		p.tags.list = splitIds( stmt.text( 14 ) );
		
		auto loadImage = [&]( Image& img, int offset ){
				img.url    = stmt.text   ( offset + 0 );
				img.width  = stmt.integer( offset + 1 );
				img.height = stmt.integer( offset + 2 );
				img.size   = stmt.integer( offset + 3 );
			};
		loadImage( p.full,      16 );
		loadImage( p.preview,   20 );
		loadImage( p.thumbnail, 24 );
		loadImage( p.reduced,   28 );
		
		return true;
	}
	
	return false;
}

bool Booru::load( Tag& p ){
//	std::cout << "opening tag: " << p.id << std::endl;
	auto& stmt = connection.getSite(site).loadTags();
	stmt.bind( p.id, 1 );
	if( stmt.next() ){
		p.count = stmt.integer( 1 );
		p.type = (Tag::Type)stmt.integer( 2 );
		return true;
	}
	else
		return false;
}

void Booru::save( Post& p ){
	auto& stmt = connection.getSite(site).savePosts();
	
	stmt.bind( static_cast<int>(p.id), 1 );
	stmt.bind( p.hash, 2 );
	stmt.bind( p.creation_time.epochMicroseconds(), 3 );
	stmt.bind( p.author, 4 );
	stmt.bind( p.source, 5 );
	stmt.bind( p.rating, 6 );
	stmt.bind( -1, 7 ); //TODO: parent_id
	stmt.bind( 0, 8 ); //TODO: children_updated
	stmt.bind( "", 9 ); //TODO: pools
	stmt.bind( 0, 10 ); //TODO: notes updated
	stmt.bind( 0, 11 ); //TODO: comments updated
	stmt.bind( p.score, 12 );
	stmt.bind( 1, 13 ); //TODO: score_count
	stmt.bind( -1, 14 ); //TODO: status
	stmt.bind( combineIds( p.tags.list ), 15 );
	stmt.bind( 0, 16 ); //TODO: post_updated
	
	auto bindImage = [&]( Image img, int offset ){
			stmt.bind( img.url,    offset + 0 );
			stmt.bind( img.width,  offset + 1 );
			stmt.bind( img.height, offset + 2 );
			stmt.bind( img.size,   offset + 3 );
		};
	bindImage( p.full,      17 );
	bindImage( p.preview,   21 );
	bindImage( p.thumbnail, 25 );
	bindImage( p.reduced,   29 );
	
	stmt.bind( 0, 33 ); //TODO: local
	
	stmt.next();
}

void Booru::save( Tag& p ){
	auto& stmt = connection.getSite(site).saveTags();
//	std::cout << p.id << std::endl;
	
	stmt.bind( p.id, 1 );
	stmt.bind( (int)p.count, 2 );
	stmt.bind( p.type, 3 );
	stmt.bind( false, 4 );
	stmt.next();
}

