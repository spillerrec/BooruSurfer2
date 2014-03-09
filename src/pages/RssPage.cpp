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

#include "RssPage.hpp"

#include <boost/algorithm/string.hpp>

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "../parsing/Rss.hpp"

using namespace std;
using namespace pugi;

string RssPage::serve( vector<string> args, vector<header> &headers ) const{
	if( args.size() < 2 )
		return "fail";
	
	Api *api = ApiHandler::get_instance()->get_by_shorthand( args[1] );
	if( !api )
		return "no such site!";
	/*
	string search = "";
	if( args.size() == 3 )
		search = args[2];
	
	Index index = api->get_index( search, page );
	vector<Post>& posts = index.posts;
	*/
	
	headers.push_back( header( "Content-Type", "application/rss+xml" ) );
	
	xml_document doc_in, doc_out;
	doc_in.load( api->get_from_url( "http://konachan.com/post/piclens?page=1&tags=" ).c_str() );
	Rss rss( doc_in );
	return rss.save();
}

