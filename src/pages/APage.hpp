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

#ifndef A_PAGE_H
#define A_PAGE_H

#include <memory>
#include <vector>
#include <string>
#include <utility>

namespace Poco{
	namespace Net{
		class HTTPServerResponse;
	}
}

class APage{
	public:
		using header = std::pair<std::string,std::string>;
		using Arguments = std::vector<std::string>;
		static header content_type( std::string mime="text/html; charset=utf-8" ){ return header( "Content-Type", mime ); }
		virtual void handleRequest( Arguments args, Poco::Net::HTTPServerResponse& response ) = 0;
		virtual ~APage(){ }
};

class StringPage : public APage{
	public:
		virtual std::string serve( Arguments args, std::vector<header> &headers ) const = 0;
		virtual void handleRequest( Arguments args, Poco::Net::HTTPServerResponse& response ) override;
};

class StreamPage: public APage{
	public:
		struct Reader{
			virtual unsigned readBuf( char* buf, unsigned size ) = 0;
			virtual ~Reader(){ }
			
			void writeAll( std::ostream& out );
		};
		
		virtual std::unique_ptr<Reader> serve( Arguments args, std::vector<header>& headers ) const = 0;
		virtual void handleRequest( Arguments args, Poco::Net::HTTPServerResponse& response ) final;
};

#endif

