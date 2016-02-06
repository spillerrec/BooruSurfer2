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

#include <iostream>

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

class PreparedStatement{
	public:
		std::string query;
		Statement stmt;
		
	public:
		PreparedStatement( Database& db, std::string query )
			:	query(query), stmt( db, this->query.c_str() ) { }
		PreparedStatement( const PreparedStatement& ) = delete;
		PreparedStatement( PreparedStatement&& other )
			:	query( std::move(other.query) ), stmt( std::move(other.stmt) ) { }
		
		Statement& operator()(){ return stmt; }
};

class SiteQueries{
	public:
		std::string site;
		
		PreparedStatement loadTags;
		PreparedStatement loadNotes;
		PreparedStatement loadPosts;
		
		PreparedStatement saveTags;
		PreparedStatement saveNotes;
		PreparedStatement savePosts;
		
		PreparedStatement iteratePosts;
	
	public:
		SiteQueries( Database& db, std::string site )
			:	site(site)
			,	loadTags ( db, "SELECT * FROM " + site + "_tags  WHERE id = ?1" )
			,	loadNotes( db, "SELECT * FROM " + site + "_notes WHERE id = ?1" )
			,	loadPosts( db, "SELECT * FROM " + site + "_posts WHERE id = ?1" )
			,	saveTags ( db, "INSERT OR REPLACE INTO " + site + "_tags VALUES( ?1, ?2, ?3, ?4 )" )
			,	saveNotes( db, "INSERT OR REPLACE INTO " + site + "_notes VALUES( ?1, ?2, ?3, ?4, ?5, ?6, ?7 )" )
			,	savePosts( db, "INSERT OR REPLACE INTO " + site + "_posts VALUES( "
					" ?1,  ?2,  ?3,  ?4,  ?5,  ?6,  ?7,  ?8,  ?9, ?10,"
					"?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20,"
					"?21, ?22, ?23, ?24, ?25, ?26, ?27, ?28, ?29, ?30,"
					"?31, ?32, ?33 )"
				)
			,	iteratePosts( db, "SELECT * FROM " + site + "_posts ORDER BY created_at DESC LIMIT ?1 OFFSET ?2" )
			{ }
		
		bool isSite( std::string wanted_site ) const { return site == wanted_site; }
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
		void reset(){
			Poco::ScopedLock<Poco::Mutex> locker( site_lock );
			sites.clear();
			db.reset();
		}
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

void resetDatabaseConnections(){ connection.reset(); }

Booru::Booru( std::string site ) : site(site) {
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
			"x REAL, "
			"y REAL, "
			"width REAL, "
			"height REAL, "
			"body TEXT)"
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

void Booru::saveToDb( const Note& item ){
	auto& stmt = connection.getSite(site).saveNotes();
	std::cout << "Saving Note: " << item.id << std::endl;
	
	stmt.bind( static_cast<int>(item.id), 1 );
	stmt.bind( item.post_id, 2 );
	stmt.bind( item.x, 3 );
	stmt.bind( item.y, 4 );
	stmt.bind( item.width, 5 );
	stmt.bind( item.height, 6 );
	stmt.bind( item.content, 7 );
	
	stmt.next();
	stmt.reset();
}

void Booru::saveToDb( const Post& item ){
	auto& stmt = connection.getSite(site).savePosts();
	std::cout << "Saving Post: " << item.id << std::endl;
	
	stmt.bind( static_cast<int>(item.id), 1 );
	stmt.bind( item.hash, 2 );
	stmt.bind( item.creation_time.epochTime(), 3 );
	stmt.bind( item.author, 4 );
	stmt.bind( item.source, 5 );
	stmt.bind( item.rating, 6 );
	stmt.bind( -1, 7 ); //TODO: parent_id
	stmt.bind( 0, 8 ); //TODO: children_updated
	stmt.bind( "", 9 ); //TODO: pools
	stmt.bind( 0, 10 ); //TODO: notes updated
	stmt.bind( 0, 11 ); //TODO: comments updated
	stmt.bind( item.score, 12 );
	stmt.bind( 1, 13 ); //TODO: score_count
	stmt.bind( -1, 14 ); //TODO: status
	stmt.bind( combineIds( item.tags.list ), 15 );
	stmt.bind( 0, 16 ); //TODO: post_updated
	
	auto bindImage = [&]( Image img, int offset ){
			stmt.bind( img.url,    offset + 0 );
			stmt.bind( img.width,  offset + 1 );
			stmt.bind( img.height, offset + 2 );
			stmt.bind( img.size,   offset + 3 );
		};
	bindImage( item.full,      17 );
	bindImage( item.preview,   21 );
	bindImage( item.thumbnail, 25 );
	bindImage( item.reduced,   29 );
	
	stmt.bind( item.saved, 33 );
	
	stmt.next();
	stmt.reset();
}

void Booru::saveToDb( const Tag& tag ){
	auto& stmt = connection.getSite(site).saveTags();
	std::cout << "Saving Tag: " << tag.id << std::endl;
	
	stmt.bind( tag.id, 1 );
	stmt.bind( (int)tag.count, 2 );
	stmt.bind( tag.type, 3 );
	stmt.bind( false, 4 );
	
	stmt.next();
	stmt.reset();
}

static Post readPostFromStmt( Statement& stmt ){
	if( stmt.next() ){
		Post p( stmt.integer( 0 ) );
		p.hash = stmt.text( 1 );
		p.creation_time = Poco::Timestamp::fromEpochTime( stmt.integer64( 2 ) );
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
		
		p.saved = stmt.boolean( 32 );
		
		stmt.reset();
		return p;
	}
	stmt.reset();
	return {};
}

bool Booru::load( Post& p, Image::Size level ){
	auto copy = p;
	if( posts.get( copy ) ){
		p = p.combine( copy );
		if( p.isAvailable(level) )
			return true;
	}
	
	auto& stmt = connection.getSite(site).loadPosts();
	stmt.bind( static_cast<int>(p.id), 1 );
	std::cout << "Loading post: " << p.id << std::endl;
	
	auto p2 = readPostFromStmt( stmt );
	if( p2.id != 0 ){
		p = p2;
		posts.insert( p, true );
		return p.isAvailable( level );
	}
	return false;
}

Index Booru::iteratePosts( IndexId id ){
	Index index( id );
	
	auto& stmt = connection.getSite(site).iteratePosts();
	id.limit = id.limit<0 ? 24 : id.limit;
	stmt.bind( id.limit, 1 );
	stmt.bind( id.page * id.limit, 2 );
	
	for( auto p=readPostFromStmt(stmt); p.id != 0; p=readPostFromStmt(stmt) ){
		index.posts.push_back( p );
	}
	
	stmt.reset();
	return index;
}

bool Booru::load( Note& p ){
	if( notes.get( p ) )
		return true;
	
	auto& stmt = connection.getSite(site).loadNotes();
	std::cout << "Loading note: " << p.id << std::endl;
	stmt.bind( static_cast<int>(p.id), 1 );
	if( stmt.next() ){
		p.x       = stmt.floating( 2 );
		p.y       = stmt.floating( 3 );
		p.width   = stmt.floating( 4 );
		p.height  = stmt.floating( 5 );
	//	p.content = stmt.text( 6 );
		notes.insert( p, true );
		stmt.reset();
		return true;
	}
	else{
		notes.insert( p, true );
		stmt.reset();
		return false;
	}
}

bool Booru::load( Tag& p ){
	if( tags.get( p ) )
		return true;
	
	auto& stmt = connection.getSite(site).loadTags();
	std::cout << "Loading tag: " << p.id << std::endl;
	stmt.bind( p.id, 1 );
	if( stmt.next() ){
		p.count = stmt.integer( 1 );
		p.type = (Tag::Type)stmt.integer( 2 );
		tags.insert( p, true );
		stmt.reset();
		return true;
	}
	else{
		tags.insert( p, true );
		stmt.reset();
		return false;
	}
}

bool Booru::load( Index& index, IndexId id ){
	return false;
}

void Booru::save( Post& p ){
	auto copy = p;
	if( posts.get( copy ) ){
	//	p = copy.saved || p.saved;
	//	if( copy.tags.list != p.tags.list || p.saved != copy.saved )
	//		posts.replace( p );
		p = p.combine( copy );
		posts.replace( p );
	}
	else
		posts.insert( p, false );
}

void Booru::save( Note& n ){
	auto copy = n;
	if( notes.get( copy ) ){
		if( copy.content.empty() ){
			notes.replace( n );
		}
	}
	else
		notes.insert( n, false );
}

void Booru::save( Tag& t ){
	auto copy = t;
	if( tags.get( copy ) ){
		if( unsigned(copy.count * 1.01) < t.count ){
			std::cout << "Tag count is: " << copy.count << " versus " << t.count << std::endl;
			tags.replace( t );
		}
	}
	else
		tags.insert( t, t.count == 0 );
}

