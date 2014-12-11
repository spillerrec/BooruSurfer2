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
#include <exception>

using namespace std;

struct SQLiteError : std::exception {
	string error;
	SQLiteError( string error ) : error(error) { }
	const char* what() const noexcept {
		return ("SQLiteError: " + error).c_str();
	}
};

template<typename T>
int validateError( int result, const char* query, T checker ){
	if( !checker( result ) )
		throw SQLiteError(
				"Unexpected sqlite error code: " + to_string( result )
			+	" during: " + query
			);
	else
		return result;
}

static int validateError( int result, const char* query, int code=SQLITE_OK ){
	return validateError( result, query, [code]( int v ){ return v == code; } );
}

Statement::Statement( Database& db, const char* query ) : db(db), query(query){
	if( sqlite3_prepare_v2( db, query, -1, &stmt, NULL ) != SQLITE_OK )
		cout << "Prepare failed: " << sqlite3_errmsg( db ) << endl; //TODO: correct?
		//TODO: throw exception
}

Statement::~Statement(){ sqlite3_finalize( stmt ); } //TODO: use shared_ptr

static void check_type( sqlite3_stmt* stmt, int column, int type ){
	if( sqlite3_column_type( stmt, column ) != type )
		throw SQLiteError( "Unexpected type: "
			+	std::to_string( sqlite3_column_type( stmt, column ) )
			+	", expected: " + std::to_string( type )
			+	"\n"
			);
}

bool Statement::next(){
	auto result = sqlite3_step( stmt );
	while( result == SQLITE_BUSY ){
		std::cout << "DB busy, retrying\n";
		result = sqlite3_step( stmt );
	}
	
	return validateError( result, query
		,	[](int i){ return i == SQLITE_ROW || i == SQLITE_DONE; }
		) == SQLITE_ROW;
}
bool Statement::reset(){
	return validateError( sqlite3_reset( stmt ), query );
}

string Statement::text( unsigned column ){
	return string( (const char*)sqlite3_column_text( stmt, column ) );
}

int Statement::integer( unsigned column ){
	check_type( stmt, column, SQLITE_INTEGER );
	return sqlite3_column_int( stmt, column );
}

int64_t Statement::integer64( unsigned column ){
	check_type( stmt, column, SQLITE_INTEGER );
	return sqlite3_column_int64( stmt, column );
}

double Statement::floating( unsigned column ){
	check_type( stmt, column, SQLITE_FLOAT );
	return sqlite3_column_double( stmt, column );
}

void Statement::bind( string value, unsigned column ){
	validateError( sqlite3_bind_text( stmt, column, value.c_str(), value.size(), SQLITE_TRANSIENT ), "binding text" );
	//TODO: enable use of SQLITE_STATIC ?
}
void Statement::bind( int value, unsigned column ){
	validateError( sqlite3_bind_int( stmt, column, value ), "binding int32" );
}
void Statement::bind( int64_t value, unsigned column ){
	validateError( sqlite3_bind_int64( stmt, column, value ), "binding int64" );
}
void Statement::bind( double value, unsigned column ){
	validateError( sqlite3_bind_double( stmt, column, value ), "binding double" );;
}


