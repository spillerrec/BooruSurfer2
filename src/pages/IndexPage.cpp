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

#include <boost/algorithm/string.hpp>

#include "../api/YandereApi.hpp"
#include "Styler.hpp"

using namespace std;
using namespace pugi;
using namespace html;

string IndexPage::serve( vector<string> args, vector<header> &headers ) const{
	YandereApi api;
	
	Styler styler;
	
	vector<Post> posts = api.get_index( "", 1 );
	
	if( posts.size() ){
		using namespace html;
		HtmlDocument page( "Index" );
		page.add_stylesheet( "/file/main.css" );
		node container = div( page, "id", "container" );
		element( container, "aside", "class", "post_list_info" ).text().set( " " );
		
		node list_container = element( container, "section", "class", "post_list size_medium" );
		node list = element( container, "ul" );
		
		for( Post post : posts )
			styler.post_thumb( element( list, "li" ), post );
		
		return page;
	}
	else{
		std::cout << "Failed to parse JSON : \\" << "\n";
		return "fail";
		//TODO: throw exception
	}
}

