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

#ifndef FILE_PAGE_H
#define FILE_PAGE_H

#include "APage.hpp"

#include <istream>
#include <fstream>
#include <unordered_map>

class FilePage : public StreamPage{
	private:
		std::unordered_map<std::string, std::string> mimes;
		
	public:
		class StreamReader : public Reader{
			private:
				std::istream& stream;
				
			public:
				StreamReader( std::istream& stream ) : stream(stream) { }
				unsigned readBuf( char* buf, unsigned size ) override{
					stream.read( buf, size );
					return stream.gcount();
				}
		};

		struct FileReader : public StreamReader{
			std::fstream fs;
			FileReader( std::string path )
				:	fs( path, std::fstream::in | std::fstream::binary ), StreamReader( fs ) { }
		};
		
	public:
		using Result = std::pair<std::unique_ptr<Reader>, std::string>;
		FilePage();
		virtual Result getReader( Arguments args, bool save=false ) const;
		virtual std::unique_ptr<Reader> serve( Arguments args, std::vector<header>& headers ) const final;
		std::string get_mime( std::string ext ) const;
};

#endif

