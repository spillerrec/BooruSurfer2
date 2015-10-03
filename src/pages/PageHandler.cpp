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

#include "PageHandler.hpp"

#include "APage.hpp"
#include "HomePage.hpp"
#include "PostPage.hpp"
#include "IndexPage.hpp"
#include "FilePage.hpp"
#include "OpenSearchPage.hpp"
#include "ProxyPage.hpp"
#include "SavePage.hpp"
#include "NotFoundPage.hpp"
#include "FaviconPage.hpp"
#include "RssPage.hpp"

PageHandler::PageHandler()
		:	page_root( new HomePage() )
		,	page_404( new NotFoundPage() )
	{
	add<IndexPage     >( "index"   );
	add<PostPage      >( "post"    );
	add<FilePage      >( "file"    );
	add<ProxyPage     >( "proxy"   );
	add<SavePage      >( "save"    );
	add<FaviconPage   >( "favicon" );
	add<RssPage       >( "rss"     );
	add<OpenSearchPage>( "search"  );
}


PageHandler::~PageHandler(){
	
}


APage* PageHandler::get( std::string page ){
	auto got = pages.find( page );
	if( got != pages.end() )
		return got->second.get();
	else
		return page_404.get();
}

