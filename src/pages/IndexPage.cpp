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

#include <Poco/Timestamp.h>

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
	
	Api *api = ApiHandler::get_instance()->get_by_shorthand( args[1] );
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
	
	Index index = api->get_index( search, page );
	vector<Post>& posts = index.posts;
	
	if( posts.size() ){
		//TODO: page amount;
		int page_amount = index.amount != -1 ? index.amount*index.id.limit : index.id.page+1;
		Styler styler( api, "Index: " + search );
		
		//Add navigation
		if( page < page_amount )
			styler.head( link(styler.doc, REL("next"), HREF( UrlHandler(api).index_url( {{search}}, page+1, limit ) )) );
		if( page-1 > 0 )
			styler.head( link(styler.doc, REL("prev"), HREF( UrlHandler(api).index_url( {{search}}, page-1, limit ) )) );
		if( !search.empty() )
			styler.head( link(styler.doc, REL("up"), HREF( UrlHandler(api).index_url( {{""}}, 1, limit ) )) );
		
		
		styler.head( link(styler.doc, REL("shortcut icon"), HREF( "/favicon/" + api->get_shorthand() + "/index" ) ) );
		styler.nav( styler.main_navigation( search ) );
		
	//TODO:	element( styler.container, "aside", "class", "post_list_info" ).text().set( " " );
		vector<Tag> tags;
		for( auto t : index.related_tags.list )
			tags.push_back( api->tag_handler.get( t ) );
		
		auto sidebar = aside(styler.doc, CLASS("post_list_info"));
		styler.tag_list( sidebar, tags, "Related tags" );
		styler.body( div(styler.doc, ID("container"))(
				styler.post_list( posts )(
						styler.index_navigation( search, page, limit, page_amount )
					)
			,	sidebar
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

