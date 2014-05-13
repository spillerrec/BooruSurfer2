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

#include <fstream>

using namespace std;


string SavePage::serve( vector<string> args, vector<header> &headers ) const{
	if( args.size() != 3 )
		return "fail";
	
	ProxyPage p;
	vector<header> fake_headers;
	string content = p.serve( args, fake_headers );
	//TODO: thow exceptions on errors
	
	if(	content == "fail"
		||	content == "parsing failed"
		||	content == "Not a valid id"
		||	content == "Not a site"
		||	content == "Could not retrive post"
		)
		return content;
	
	ofstream file( "out/" + args[2], ios_base::out | ios_base::binary );
	file << content;
	
	return "<html><head><title>Saved</title></head><body>Saved</body></html>";
}

