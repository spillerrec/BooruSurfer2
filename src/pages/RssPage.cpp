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
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>

#include "RssPage.hpp"

#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "../parsing/Rss.hpp"
#include "UrlHandler.hpp"

using namespace std;
using namespace pugi;

string RssPage::serve( vector<string> args, vector<header> &headers ) const{
	if( args.size() < 2 )
		return "fail";
	
	Api *api = ApiHandler::get_instance()->get_by_shorthand( args[1] );
	if( !api )
		return "no such site!";
	UrlHandler url( api );
	
	string search = "";
	if( args.size() == 3 )
		search = args[2];
	
	Index index = api->get_index( search, 1 );
	vector<Post>& posts = index.posts;
	
	Rss rss;
	rss.title.value = api->get_name() + " - " + search;
	rss.link.value = url.index_url( {{search}} );
	
	Poco::Timestamp current;
	for( auto post : posts ){
		Rss::Item item{ "item" };
		item.title.value = url.image_tags( post, 128 );
		item.link.value = url.post_url( post.id );
		item.description.value = "<img src=\"" + post.thumbnail.url + "\"/>";
		item.media_thumbnail.url.value = post.thumbnail.url;
		item.media_content.url.value = post.full.url;
		item.guid.value = item.link.value;
		item.guid_isPermaLink = true;
		
		if( post.creation_time.epochMicroseconds() != 0 )
			current = post.creation_time;
		
		item.pubDate.value = Poco::DateTimeFormatter::format( current, Poco::DateTimeFormat::RFC1123_FORMAT );
		current -= 1000000; //Decrease it with a second
		
		rss.items.push_back( item );
	}
	
	headers.push_back( header( "Content-Type", "application/rss+xml" ) );
	return rss.save();
}

