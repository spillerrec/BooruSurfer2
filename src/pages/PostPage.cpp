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

#include "PostPage.hpp"
#include "../api/YandereApi.hpp"

#include "Styler.hpp"

using namespace std;
using namespace HTML;

string PostPage::serve( vector<string> args, vector<header> &headers ) const{
	YandereApi api;
	Post post = api.get_post( 277759 );
	
	Styler s( "Post: TODO: add tags here" );
	headers.push_back( content_type() );
	
	s.container( s.post( post ) );
	
	//Post info
	auto info = aside( s.doc, CLASS( "post_info" ) );
	
	
	//Temp
	vector<Tag> tags;
	vector<string> tag_string = post.tags.get();
	for( string s : tag_string ){
		Tag t;
		t.name = s;
		tags.push_back( t );
	}
	s.tag_list( info, tags, "Tags:" );
	
	s.container( info );
	
	return s.doc;
}

