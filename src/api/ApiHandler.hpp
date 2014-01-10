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

#ifndef API_HANDLER_HPP
#define API_HANDLER_HPP

#include <string>
#include <vector>

class Api;

class ApiHandler{
	private:
		std::vector<const Api*> apis;
		ApiHandler();
		static ApiHandler* instance;
		
	public:
		static ApiHandler* get_instance();
		
		std::vector<const Api*> get_apis(){ return apis; }
		const Api* get_by_shorthand( std::string shorthand ) const;
};

#endif

