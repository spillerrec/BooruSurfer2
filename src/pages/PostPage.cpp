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
#include "../html/HtmlDocument.hpp"
#include "../api/YandereApi.hpp"

using namespace std;
using namespace pugi;
using namespace html;

string PostPage::serve( vector<string> args, vector<header> &headers ) const{
	YandereApi api;
	
	Post post = api.get_post( 0 );
	
	
	HtmlDocument doc( "Post test" );
	
	p( doc.get_body(), "url: " + post.url );
	
	for( int i=0; i<post.tags.get().size(); i++ )
		p( doc.get_body(), "tags: " + post.tags.get()[i] );
//	for( string s : post.tags.get() )
//		p( doc, "tags2: " + s );
	
	return doc;
}

