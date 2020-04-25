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

#include "TagSearchPage.hpp"

#include "../api/Api.hpp"
#include "../api/ApiHandler.hpp"
#include "../exceptions/utils.hpp"

#include <algorithm>
#include <cctype>

#include <nlohmann/json.hpp>

using namespace std;

TagSearchPage::Parameters TagSearchPage::parseArguments( Arguments args ){
	require( args.size() >= 2, "Incorrect arguments" );
	return { args[1], args.size() == 3 ? args[2] : std::string() };
}

string TagSearchPage::serve( vector<string> args, vector<header> &headers ) const{
	auto input = parseArguments( args );
	Api& api = ApiHandler::get_instance()->get_by_shorthand( input.site );
	
	if (input.search.empty())
		return "[]";
	
	auto tags = api.booru.searchTags( input.search );
	
	nlohmann::json j;
	for(auto tag : tags)
		j.push_back( {
			{"id", tag.id},
			{"count", tag.count},
			{"type", (int)tag.type}
		} );
	
	return j.dump();
}

