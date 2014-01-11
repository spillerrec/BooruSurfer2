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

#include <boost/algorithm/string.hpp>

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "Styler.hpp"

using namespace std;
using namespace HTML;

string IndexPage::serve( vector<string> args, vector<header> &headers ) const{
	if( args.size() < 2 )
		return "fail";
	
	const Api *api = ApiHandler::get_instance()->get_by_shorthand( args[1] );
	if( !api )
		return "no such site!";
	
	string search;
	int page = 1;
	int limit = -1;
	if( args.size() == 3 ){
		try{
			page = stoi( args[2] );
		}
		catch(...){
			search = args[2];
		}
	}
	else if( args.size() == 4 ){
		//TODO: page 99-99
		try{
			page = stoi( args[2] );
		}
		catch(...){
			return "fail parsing page number";
		}
		
		search = args[3];
	}
	vector<Post> posts = api->get_index( search, page );
	
	if( posts.size() ){
		Styler styler( api, "Index: " + search );
		
		styler.nav( styler.main_navigation( search ) );
		
	//TODO:	element( styler.container, "aside", "class", "post_list_info" ).text().set( " " );
		
		styler.body( div(styler.doc, ID("container"))(
				styler.post_list( posts )
				//TODO: don't include index_navigation if only one page?
			,	styler.index_navigation( search, page, limit, 9999 ) //TODO:
			) );
		cout << "Post amount: " << posts.size() << endl;
		
		headers.push_back( content_type() );
		return styler.doc;
	}
	else{
		std::cout << "Failed to parse JSON : \\" << "\n";
		return "fail";
		//TODO: throw exception
	}
}

