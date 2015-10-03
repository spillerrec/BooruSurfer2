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
#include <memory>

class APage;

class PageHandler{
	private:
		typedef std::unique_ptr<APage> APage_ptr;
		std::unordered_map<std::string, APage_ptr > pages;
		APage_ptr page_root;
		APage_ptr page_404;
		
	public:
		PageHandler();
		~PageHandler();
		
		template<class T>
		void add( std::string name ){ //TODO: support arguments to constructor
			pages[ name ] = std::make_unique<T>();
		}
		
		APage* get( std::string page );
		APage* get_root(){ return page_root.get(); }
};

#endif

