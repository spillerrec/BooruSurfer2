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

#include "LocalDb.hpp"

#include <Poco/DirectoryIterator.h>

#include <string>
#include <iostream>
#include <stdexcept>


LocalDb::LocalDb() : db( "cache_debug.sqlite" ) {
	std::cout << "Initializing local booru\n";
	auto create = [&](const char* table, const char* values)
		{ Statement( db, ("CREATE TABLE IF NOT EXISTS " + std::string(table) + '(' + values + ')').c_str() ).next(); };
	//TODO: Make something smarter, constexp string combiner to avoid ", "?
	
	//TODO: Autoincrement
	create( "configuration",
			"setting TEXT PRIMARY KEY, "
			"conf_group TEXT, "
			"value TEXT" //TODO: any type
		); //Generic config storage
	
	create( "plugins",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, " //Human readable
			"type INTEGER, " //Site handler, tag category handler, etc.
			"handler TEXT" //Handler indification @plugin(path), @internal(class name)?
		); //TODO:
	
	create( "site_configuration",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, "
			"plugin INTEGER NOT NULL, "
			"FOREIGN KEY(plugin) REFERENCES plugins(id)"
		); //TODO:
	
	create( "tag_category",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, "
			"color INTEGER, "
			"plugin INTEGER, "
			"FOREIGN KEY(plugin) REFERENCES plugins(id)"
		);
		
	create( "tags",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, "
			"parent_id INTEGER NOT NULL, "
			"category INTEGER, "
			"FOREIGN KEY(parent_id) REFERENCES tags(id), "
			"FOREIGN KEY(category) REFERENCES tag_category(id)"
		);
		
	
	create( "meta_types",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT"
		);
	create( "tag_meta",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"tag_id INTEGER NOT NULL, "
			"meta_id INTEGER NOT NULL, "
			"value TEXT, " //TODO: any type
			"FOREIGN KEY(tag_id) REFERENCES tags(id), "
			"FOREIGN KEY(meta_id) REFERENCES meta_types(id) "
		);
		
	
	create( "tag_legacy_names",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name TEXT, "
			"tag_id INTEGER NOT NULL, "
			//TODO: Source system?
			"FOREIGN KEY(tag_id) REFERENCES tags(id) "
		);
	
	create( "mime",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"mime TEXT "
			//TODO: What else?
			//
		);
		
		//file extension to mime lookup?
	
	//Contains prefixes so we can more easily fix paths, and to save space
	create( "media_path",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"path TEXT"
		);
	
	create( "media",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"date INTEGER, " //TODO: Time
			"check_sum INTEGER, " //TODO: 64bit?
			"path_prefix INTEGER, "
			"path TEXT, "
			"mime_id INTEGER, "
			"file_size INTEGER, " //TODO: 64bit?
			"FOREIGN KEY(path_prefix) REFERENCES media_path(id), "
			"FOREIGN KEY(mime_id) REFERENCES mime(id) "
		);
	
	create( "position",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"shape INTEGER, "
			"x INTEGER, " //Refers to the center
			"y INTEGER, " //Refers to the center
			"w INTEGER, "
			"h INTEGER "
		);
	
	create( "media_tags",
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"media_id INTEGER NOT NULL, "
			"tag_id INTEGER NOT NULL, "
			"position_id INTEGER, "
			"FOREIGN KEY(media_id) REFERENCES media(id), "
			"FOREIGN KEY(tag_id)   REFERENCES tag(id) "
			"FOREIGN KEY(position_id) REFERENCES position(id) "
		);
	
	create( "media_links", //TODO: Think through
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"media_id_from INTEGER, "
			"media_id_to INTEGER, "
			"directed INTEGER, " //Is one-way or both directions
			"FOREIGN KEY(media_id_from) REFERENCES media(id), "
			"FOREIGN KEY(media_id_to)   REFERENCES media(id) "
		);
		
	//TODO: Queries as folders
	//Empty query for grouping other queries?
	
	//Some way of referencing its source, likely multiple sources
}


std::string LocalDb::sanitizeFolderPath( std::string path ){
	//NOTE: Paths must always be UTF-8
	for( auto& c : path )
		if( c == '\\' )
			c = '/';
	
	if( path.back() != '/' )
		path += '/';
	
	return path;
}

DbIndex LocalDb::getFolderId( const std::string& folder_path ){
	
	//TODO: Check path. Must end on '/' and must not use '\\'!
	
	Statement read( db, "SELECT * FROM media_path WHERE path = ?1 " );
	read.bind( folder_path, 1 );
	if( read.next() )
		return { read.integer64( 0 ) };
	
	//Failed to get an folder, insert a new one
	Statement write( db, "INSERT INTO media_path VALUES( null, ?1 ) " );
	write.bind( folder_path, 1 );
	write.next(); //TODO: have a write specialization?
	return { db.lastRowId() };
}

DbIndex LocalDb::addMedia( DbIndex path_id, const std::string& file_name ){
	/*		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"date INTEGER, " //TODO: Time
			"check_sum INTEGER, " //TODO: 64bit?
			"path_prefix INTEGER, "
			"path TEXT, "
			"mime_id INTEGER, "
			"file_size INTEGER, " //TODO: 64bit?
			"FOREIGN KEY(path_prefix) REFERENCES media_path(id), "
			"FOREIGN KEY(mime_id) REFERENCES mime(id) "*/
	if( !path_id )
		throw std::runtime_error( "Missing media ID" );
	
	Statement write( db, "INSERT INTO media VALUES( null, null, null, ?1, ?2, null, null ) " );
	write.bind( path_id.get(), 1 );
	write.bind( file_name, 2 );
	write.next();
	return { db.lastRowId() };
}


void LocalDb::addFolderToDbInternal( const std::string& folder_path, int64_t& counter ){
	//Get folder path ID for insertion
	auto folder = Poco::Path( folder_path ).makeAbsolute().toString();
	auto db_path = sanitizeFolderPath( folder );
	auto path_id = getFolderId( db_path );
	
	//Iterate over all files
	Poco::DirectoryIterator it( { folder_path } ), end;
	for( ; it != end; ++it, counter++ )
		try{
			if( it->isFile() ){
				//TODO: Fill all the details
				addMedia( path_id, Poco::Path( it->path() ).getFileName() );
				
			}
			else if( it->isDirectory() ){
				//Add folders recursively
				addFolderToDbInternal( Poco::Path( it->path() ).makeAbsolute().toString(), counter );
			}
		} catch( ... ) { }
}

void LocalDb::addFolderToDb( const std::string& folder_path ){
	int64_t amount = 0;
	//Do it as a single transacction to speed it up very very much
	{	Transaction trans( db );
		addFolderToDbInternal( folder_path, amount );
	}
	std::cout << "Total files added to local database: " << amount << '\n';
}
