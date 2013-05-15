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

#ifndef FILE_PAGE_H
#define FILE_PAGE_H

#include "APage.hpp"

#include <unordered_map>

class FilePage : public APage{
	private:
		std::unordered_map<std::string, std::string> mimes;
		
	public:
		FilePage();
		std::string serve( std::vector<std::string> args, std::vector<header> &headers ) const;
		std::string get_mime( std::string ext ) const;
};

#endif
