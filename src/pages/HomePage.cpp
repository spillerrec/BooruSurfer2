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

#include "HomePage.hpp"

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "Styler.hpp"

using namespace std;
using namespace HTML;

string HomePage::serve( vector<string> args, vector<header> &headers ) const{
	vector<Api*> apis = ApiHandler::get_instance()->get_apis();
	
	Styler s( nullptr, "BooruSurfer2" );
	headers.push_back( content_type() );
	
	s.body( h1(s.doc)( "Available sites" ) );
	
	auto list = ul(s.doc);
	for( auto api : apis )
		list( li(s.doc)( a(s.doc, HREF( UrlHandler(api).index_url() ) )( api->get_name() ) ) );
	s.body( list );
	
	return s.doc;
}

