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

#include "NotFoundPage.hpp"

#include "Styler.hpp"

#include <algorithm>

using namespace std;
using namespace HTML;

string NotFoundPage::serve( vector<string> args, vector<header> &headers ) const{
	//Rebuild the query, as it was split apart
	auto add_dir = [](string sum, string add){ return ( add != ".." ) ? sum + "/" + add : ""; };
	string query = accumulate( args.begin(), args.end(), string(""), add_dir );
	
	//TODO: set error code
	Styler s( nullptr, "Could not find \'" + query + "'" );
	headers.push_back( content_type() );
	
	s.container( h3( s.doc )( "404: Page not found" ) );
	s.container( p( s.doc )( query ) );
	
	return s.doc;
}

