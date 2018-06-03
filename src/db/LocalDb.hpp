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

#ifndef LOCAL_DB_HPP
#define LOCAL_DB_HPP

#include "Database.hpp"
#include "Statement.hpp"

#include <string>

class DbIndex{
	private:
		int64_t id{ -1 }; //Negative values means invalid index
		
	public:
		DbIndex( int64_t id=-1 ) : id(id) {}
		
		int64_t get() const{ return id; }
		//TODO: debug exception on invalid index?
		//Or use the maybe scheme instead? We may want to pass an invalid index though
		
		operator bool() const{ return id >= 0; }
		operator int64_t() const{ return get(); }
};

class LocalDb{
	private:
		Database db;
		
		void addFolderToDbInternal( const std::string& folder_path, int64_t& counter );
		
	public:
		LocalDb();
		
		static std::string sanitizeFolderPath( std::string path );
		DbIndex getFolderId( const std::string& folder_path );
		
		DbIndex addMedia( DbIndex path_id, const std::string& file_name ); //TODO: Much more
		
		void addFolderToDb( const std::string& folder_path );
};

#endif

