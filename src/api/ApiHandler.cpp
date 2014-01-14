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


#include "ApiHandler.hpp"

#include "SanApi.hpp"
#include "YandereApi.hpp"

ApiHandler* ApiHandler::instance{ nullptr };

ApiHandler::ApiHandler() : apis{
	//TODO: add all the apis
	//TODO: cleanup
		new DanApi()
	,	new SanApi()
	,	new KonachanApi()
	,	new YandereApi()
}{ }

ApiHandler* ApiHandler::get_instance(){
	//TODO: make atomic
	if( !instance )
		instance = new ApiHandler;
	return instance;
}

const Api* ApiHandler::get_by_shorthand( std::string shorthand ) const{
	for( auto api : apis )
		if( api->get_shorthand() == shorthand )
			return api;
	return nullptr;
}