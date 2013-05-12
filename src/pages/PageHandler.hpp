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

#ifndef PAGE_HANDLER_H
#define PAGE_HANDLER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

#include "APage.hpp"

class PageHandler{
	private:
		std::unordered_map<std::string, APage*> pages;
		APage* page_root;
		APage* page_404;
		
	public:
		PageHandler();
		~PageHandler();
		
		void add( std::string name, APage* handler ){
			pages[ name ] = handler;
		}
		
		APage* get( std::string page );
		APage* get_root(){ return page_root; }
};

#endif

