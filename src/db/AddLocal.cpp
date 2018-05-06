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

#include <Poco/DirectoryIterator.h>

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <vector>

using namespace std;
using namespace Poco;

static vector<string> split( string str, string split_on ){
	vector<string> out;
	boost::split( out, str, boost::is_any_of( split_on ) );
	//TODO: split on " -", to avoid removed characters from breaking it up prematurely
	return out;
}

template<typename Predicate>
vector<string> splitAndFilter( string str, string split_on, Predicate pred ){
	auto out = split( str, split_on );
	out.erase( remove_if( out.begin(), out.end(), pred ), out.end() );
	return out;
}

void addFolder( string dir_path ){
	int amount = 0;
	DirectoryIterator it( { dir_path } ), end;
	for( ; it != end; ++it, amount++ )
		try{
			auto parts = splitAndFilter( it.path().getBaseName(), " ", [](string arg){ return arg.empty(); } );
			if( it->isFile() && parts.size() >= 2 ){
				auto& api = ApiHandler::get_instance()->get_by_shorthand( parts[0] );
				
				Post p( stoi( parts[1] ) );
				for( unsigned i=2, type=0; i<parts.size(); i++ ){
					if( parts[i] == "-" )
						type++;
					else{
						Tag t( parts[i] );
						switch( type ){
							case 1: t.type = Tag::ARTIST; break;
							case 2: t.type = Tag::COPYRIGHT; break;
							case 3: t.type = Tag::CHARACTER; break;
							case 4: t.type = Tag::NONE; break;
							default: t.type = Tag::UNKNOWN; break;
						}
						//TODO: type
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
		} catch( ... ) { }
	cout << "Total local files: " << amount << '\n';
}

