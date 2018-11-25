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

#include "AddLocal.hpp"

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "../objects/Post.hpp"
#include "../parsing/StringViewUtils.hpp"

#include <Poco/DirectoryIterator.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace Poco;

void addFolder( string dir_path ){
	int amount = 0;
	DirectoryIterator it( { dir_path } ), end;
	for( ; it != end; ++it, amount++ )
		try{
			auto parts = splitAllOn( it.path().getBaseName(), ' ' );
			removeEmpty( parts );
			if( it->isFile() && parts.size() >= 2 ){
				auto& api = ApiHandler::get_instance()->get_by_shorthand( std::string(parts[0]) ); //TODO: Avoid string conversion
				
				Post p( stoi( std::string(parts[1]) ) );
				for( unsigned i=2, type=0; i<parts.size(); i++ ){
					if( parts[i] == "-" )
						type++;
					else{
						Tag t( std::string( parts[i] ) );
						switch( type ){
							case 1:  t.type = Tag::ARTIST   ; break;
							case 2:  t.type = Tag::COPYRIGHT; break;
							case 3:  t.type = Tag::CHARACTER; break;
							case 4:  t.type = Tag::NONE     ; break;
							default: t.type = Tag::UNKNOWN  ; break;
						}
						api.tag_handler.add( t );
						p.tags.add( t.id );
					}
				}
				
				//Filepath
				p.saved = true;
				p.creation_time = it->created();
				auto current = it.path();
				p.full.url = "file:///" + current.makeAbsolute().toString();
				
				api.post_handler.add( p );
			}
			else if( it->isDirectory() ){
				auto current = it.path();
				addFolder( current.makeAbsolute().toString() );
			}
		} catch( ... ) { }
	cout << "Total local files: " << amount << '\n';
}

