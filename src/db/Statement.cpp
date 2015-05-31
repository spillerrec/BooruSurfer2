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
#include <Poco/Thread.h>

#include <utility>
#include <iostream>
#include <exception>

using namespace std;

struct SQLiteError : runtime_error {
	static string message( Database& db, const char* query, string error ){
		return "SQLiteError:\n\tWhat: " + error
			+	"\n\tQuery: " + query
			+	"\n\tLast SQLite error message: " + sqlite3_errmsg( db ) + "\n"
			;
	}
	SQLiteError( Database& db, const char* query, string error )
		:	runtime_error( message( db, query, error ) )
		{ cout << what() << endl; }
};

void Statement::throwError( string error ){ throw SQLiteError( db, query, error ); }

template<typename T>
int validateError( Statement& stmt, int result, T checker ){
	if( !checker( result ) )
		stmt.throwError( "Unexpected sqlite error code: " + to_string( result ) );
	else
		return result;
}

static int validateError( Statement& stmt, int result, int code=SQLITE_OK ){
	return validateError( stmt, result, [code]( int v ){ return v == code; } );
}

Statement::Statement( Database& db, const char* query ) : db(db), query(query){
	if( sqlite3_prepare_v2( db, query, -1, &stmt, NULL ) != SQLITE_OK )
		throwError( "prepare" );
}

Statement::~Statement(){ sqlite3_finalize( stmt ); } //TODO: use shared_ptr

static void check_type( Statement& stmt, int column, int type ){
	auto actual = stmt.columnType( column );
	if( actual != type )
		stmt.throwError( "Unexpected type: " + to_string( actual )
			+	", expected: " + to_string( type ) + "\n"
			);
}

bool Statement::next(){
	int result, i=1;
	while( (result = sqlite3_step( stmt )) == SQLITE_BUSY ){
		if( ++i >= 30 )
			throwError( "Time out during execution of statement" );
		Poco::Thread::sleep( 10 );
	}
	
	return validateError( *this, result
		,	[](int i){ return i == SQLITE_ROW || i == SQLITE_DONE; }
		) == SQLITE_ROW;
}
bool Statement::reset(){
	cout << "Resetting: " << query << endl;
	return validateError( *this, sqlite3_reset( stmt ) );
}

int Statement::columnType( unsigned column ){ return sqlite3_column_type( stmt, column ); }

string Statement::text( unsigned column ){
	return string( (const char*)sqlite3_column_text( stmt, column ) );
}

int Statement::integer( unsigned column ){
	check_type( *this, column, SQLITE_INTEGER );
	return sqlite3_column_int( stmt, column );
}

int64_t Statement::integer64( unsigned column ){
	check_type( *this, column, SQLITE_INTEGER );
	return sqlite3_column_int64( stmt, column );
}

double Statement::floating( unsigned column ){
	check_type( *this, column, SQLITE_FLOAT );
	return sqlite3_column_double( stmt, column );
}

void Statement::bind( string value, unsigned column ){
	validateError( *this, sqlite3_bind_text( stmt, column, value.c_str(), value.size(), SQLITE_TRANSIENT ) );
	//TODO: enable use of SQLITE_STATIC ?
}
void Statement::bind( int value, unsigned column ){
	validateError( *this, sqlite3_bind_int( stmt, column, value ) );
}
void Statement::bind( int64_t value, unsigned column ){
	validateError( *this, sqlite3_bind_int64( stmt, column, value ) );
}
void Statement::bind( double value, unsigned column ){
	validateError( *this, sqlite3_bind_double( stmt, column, value ) );;
}

void Transaction::close(){
	if( commit )
		try{ Statement( *db, "COMMIT" ).next(); }
		catch(...){ Statement( *db, "ROLLBACK" ).next(); }
	commit = false;
}

