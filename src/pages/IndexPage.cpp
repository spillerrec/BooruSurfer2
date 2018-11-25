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

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "../exceptions/utils.hpp"
#include "Styler.hpp"

#include <algorithm>
#include <cctype>

using namespace std;
using namespace HTML;

IndexPage::Parameters IndexPage::parseArguments( Arguments args ){
	require( args.size() >= 2, "Too few arguments" );
	
	string search;
	int page = 1;
	int limit = -1;
	if( args.size() == 3 ){
		try{
			auto page_str = args[2];
			if( !all_of( page_str.begin(), page_str.end(), [](char c){ return isdigit(c); } ) )
				throw false;
			page = stoi( args[2] );
		}
		catch(...){
			search = args[2];
		}
	}
	else if( args.size() == 4 ){
		//TODO: page 99-99
		page = getInt( args[2], "fail parsing page number" );
		search = args[3];
	}
	
	return { args[1], search, page, limit };
}

string IndexPage::serve( vector<string> args, vector<header> &headers ) const{
	auto input = parseArguments( args );
	Api& api = ApiHandler::get_instance()->get_by_shorthand( input.site );
	
	Index index = api.get_index( input.search, input.page, input.limit );
	
	//TODO: page amount;
	int page_amount = index.amount != -1 ? index.amount*index.id.limit : index.id.page+1;
	Styler styler( &api, "Index: " + input.search );
	
	//Add navigation
	if( input.page < page_amount )
		styler.head( link(styler.doc, REL("next"), HREF( UrlHandler(&api).index_url( {{input.search}}, input.page+1, input.limit ) )) );
	if( input.page-1 > 0 )
		styler.head( link(styler.doc, REL("prev"), HREF( UrlHandler(&api).index_url( {{input.search}}, input.page-1, input.limit ) )) );
	
	
	styler.head( link(styler.doc, REL("shortcut icon"), HREF( "/favicon/" + api.get_shorthand() + "/index" ) ) );
	//Rss meta tag
	styler.head( link(styler.doc
		,	REL("alternate"), TYPE("application/rss+xml")
		,	HREF( UrlHandler(&api).rss_url( {{input.search}}, 1, input.limit ) )
		,	TITLE( "Feed: " + input.search ) ) );
	//OpenSearch meta tag
	styler.head( link(styler.doc
		,	REL("search"), TYPE("application/opensearchdescription+xml")
		,	HREF( "/search/" + api.get_shorthand() )
		,	TITLE( api.get_name() + " search" ) ) );
	//<link rel="search" type="application/opensearchdescription+xml" href="/opensearch.xml" title="Twitter Search">
	styler.head( script( styler.doc, SRC( "/file/jquery2.min.js" ) )("") );
	styler.head( script( styler.doc, SRC( "/file/infinity.js" ) )("") );
	
	styler.nav( styler.main_navigation( input.search ) );
	
//TODO:	element( styler.container, "aside", "class", "post_list_info" ).text().set( " " );
	vector<Tag> tags;
	for( auto t : index.related_tags.list )
		tags.push_back( api.tag_handler.get( t ) );
	
	auto sidebar = aside(styler.doc, CLASS("post_list_info"));
	styler.tag_list( sidebar, tags, "Related tags" );
	styler.body( div(styler.doc, ID("container"))(
			styler.post_list_section( index.posts )(
					styler.index_navigation( input.search, input.page, input.limit, page_amount )
				)
		,	sidebar
		) );
	cout << "Post amount: " << index.posts.size() << '\n';
	
	headers.push_back( content_type() );
	return styler.doc;
}

