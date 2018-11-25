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

#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Database.hpp"

#include <string>
#include <cstdint>

struct sqlite3;
struct sqlite3_stmt;

class Statement{
	private:
		sqlite3_stmt *stmt{ nullptr };
		Database& db;
		const char* query;
	
	public:
		Statement( Database& db, const char* query );
		Statement( const Statement& ) = delete;
		Statement( Statement&& other )
			:	stmt( other.stmt ), db( other.db ), query( other.query ) {
				other.stmt = nullptr;
				other.query = nullptr;
			}
		~Statement();
		
		[[ noreturn ]] void throwError( std::string error );
		
		bool next();
		bool reset();
		
		int columnType( unsigned column );
		
		std::string text( unsigned column );
		bool boolean( unsigned column );
		int integer( unsigned column );
		int64_t integer64( unsigned column );
		double floating( unsigned column );
		
		void bind( std::string value, unsigned column );
		void bind( bool value, unsigned column );
		void bind( int value, unsigned column );
		void bind( int64_t value, unsigned column );
		void bind( double value, unsigned column );
		
};

class Transaction{
	private:
		Database* db;
		bool commit{ true };
		
	public:
		Transaction( Database& db ) : db(&db) { Statement( db, "BEGIN TRANSACTION" ).next(); }
		Transaction( const Transaction& ) = delete;
		Transaction( Transaction&& other ) : db( other.db ){ other.commit = false; }
		~Transaction(){ close(); }
		
		Transaction& operator=( const Transaction& other ) = delete;
		Transaction& operator=( Transaction&& other ){
			db = other.db;
			commit = other.commit;
			other.commit = false;
			return *this;
		}
		
		void close();
};

#endif

