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

#include "FilePage.hpp"
#include "../html/HtmlDocument.hpp"

#include <iostream>
#include <fstream>
#include <istream>

using namespace std;
using namespace pugi;
using namespace html;

string FilePage::serve( vector<string> args, vector<header> headers ) const{
	string filepath = "files";
	for( string arg : args )
		if( arg != ".." )
			filepath += "/" + arg;
	
	cout << filepath << "\n";
	fstream fs( filepath, fstream::in );
	
	return string( istreambuf_iterator<char>( fs ), istreambuf_iterator<char>() );
}

