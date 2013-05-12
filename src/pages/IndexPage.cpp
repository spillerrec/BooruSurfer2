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

#include "IndexPage.hpp"
#include "../html/HtmlDocument.hpp"

#include <boost/network/protocol/http/client.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <boost/algorithm/string.hpp>


#include "../parsing/JsonDataNode.hpp"
#include "../parsing/NullDataNode.hpp"

using namespace std;
using namespace pugi;
using namespace html;


DataNode get_page( string server ){
	using namespace boost::network;
	http::client client;
	http::client::request request( server + "/post/index.json" );
	request << header( "Connection", "close" );
	
	//http::client::response response = client.get( request );
	return JsonDataNode::from_string( body( client.get( request ) ) );
}

string IndexPage::serve( vector<string> args, vector<header> headers ) const{
	DataNode root = get_page( "https://yande.re" );
	if( root ){
		using namespace html;
		HtmlDocument page( "Index" );
		page.add_stylesheet( "/file/main.css" );
		node container = div( page, "id", "container" );
		element( container, "aside", "class", "post_list_info" ).text().set( " " );
		
		node list_container = element( container, "section", "class", "post_list size_medium" );
		node list = element( container, "ul" );
		
		for( DataNode post : root ){
			DataNode url = post[ "preview_url" ];
			if( url ){
				image( element( element( list, "li" ), "a", "href", post[ "file_url" ] ), url, "preview" );
			}
		}
		
		return page;
	}
	else{
		std::cout << "Failed to parse JSON : \\" << "\n";
		return "fail";
		//TODO: throw exception
	}
}

