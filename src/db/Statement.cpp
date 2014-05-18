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

#include "Statement.hpp"

#include <sqlite3.h>

#include <utility>
#include <iostream>

using namespace std;

Statement::Statement( Database& db, const char* query ){
	if( sqlite3_prepare_v2( db, query, -1, &stmt, NULL ) != SQLITE_OK )
		cout << "Prepare failed: " << sqlite3_errmsg( db ) << endl; //TODO: correct?
		//TODO: throw exception
}

Statement::~Statement(){ /*sqlite3_finalize( stmt );*/ } //TODO: use shared_ptr

bool Statement::next(){
	return sqlite3_step( stmt ) != SQLITE_DONE;
}
bool Statement::reset(){
	return sqlite3_reset( stmt ) == SQLITE_OK;
}

string Statement::text( unsigned column ){
	return string( (const char*)sqlite3_column_text( stmt, column ) );
}

int Statement::integer( unsigned column ){
	return sqlite3_column_int( stmt, column );
}

int64_t Statement::integer64( unsigned column ){
	return sqlite3_column_int64( stmt, column );
}

double Statement::floating( unsigned column ){
	return sqlite3_column_double( stmt, column );
}

void Statement::bind( string value, unsigned column ){
	sqlite3_bind_text( stmt, column, value.c_str(), value.size(), SQLITE_TRANSIENT );
	//TODO: enable use of SQLITE_STATIC ?
}
void Statement::bind( int value, unsigned column ){
	sqlite3_bind_int( stmt, column, value );
}
void Statement::bind( int64_t value, unsigned column ){
	sqlite3_bind_int64( stmt, column, value );
}
void Statement::bind( double value, unsigned column ){
	sqlite3_bind_double( stmt, column, value );
}


