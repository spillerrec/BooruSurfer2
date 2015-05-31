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

#include "Database.hpp"

#include "Statement.hpp"

#include <sqlite3.h>

#include <utility>
#include <iostream>

using namespace std;

Database::Database( string file_path ){
	if( sqlite3_open_v2( file_path.c_str(), &db, SQLITE_OPEN_READWRITE, nullptr ) != SQLITE_OK ){
		//TODO: throw exception
		cout << "Couldn't open db!" << endl;
		sqlite3_close( db );
		exit( -1 );
	}
//	sqlite3_busy_timeout( db, 100 );
//	Statement( *this, "PRAGMA journal_mode=WAL;" ).next();
}

Database::~Database(){
//	cout << "Destroying" << endl;
	sqlite3_close( db );
}
