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

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>

struct sqlite3;
struct sqlite3_stmt;

class Database{
	private:
		sqlite3 *db;
	
	public:
		Database( std::string file_path );
		~Database();
		
		operator sqlite3*() const{ return db; }
};

#endif

