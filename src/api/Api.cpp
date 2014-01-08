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

//Include Poco before anything else
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

#include "Api.hpp"

#include <iostream>
#include <sstream>

using namespace Poco::Net;
using namespace Poco;

std::string Api::get_from_url( std::string url ) const{
	try{
		// prepare session
		Poco::URI uri(url.c_str());
		//TODO: understand and improve
		const Context::Ptr context = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
		HTTPSClientSession session(uri.getHost(), uri.getPort(), context);

		// prepare path
		std::string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		// send request
		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		session.sendRequest(req);

		// get response
		HTTPResponse res;
		//cout << res.getStatus() << " " << res.getReason() << endl;

		// print response
		std::istream &is = session.receiveResponse(res);
		
		std::string str = std::string(static_cast<std::stringstream const&>(std::stringstream() << is.rdbuf()).str());
		std::cout << "Retrived: " << url << "\n";
		std::cout << str << "\n";
		return str;
	}
	catch( Exception &ex ){
		return "";
	}
}

