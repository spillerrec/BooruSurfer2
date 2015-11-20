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

#include <Poco/Timestamp.h>

#include "SavePage.hpp"
#include "ProxyPage.hpp"
#include "../exceptions/utils.hpp"

#include "Styler.hpp"

#include <fstream>

using namespace std;


string SavePage::serve( vector<string> args, vector<header> &headers ) const{
	require( args.size() == 3, "fail" );
	
	vector<header> fake_headers;
	auto result = ProxyPage().getReader( args, true );
	auto& reader = result.first;
	
	//Save file
	ofstream file( "out/" + args[2], ios_base::out | ios_base::binary );
	reader->writeAll( file );
	
	//TODO: detect errors and show them
	BasicStyler s( "Saved" );
	s.body( HTML::p( s.doc )( "Saved" ) );
	return s.doc;
}

