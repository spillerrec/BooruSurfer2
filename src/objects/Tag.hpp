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

#ifndef TAG_H
#define TAG_H

#include <string>

class Tag{
	public:
		enum Type{
			NONE,
			ARTIST,
			COPYRIGHT,
			CHARACTER,
			SPECIAL,
			COMPANY,
			UNKNOWN = 255
		};
		
	private:
		unsigned id;
		std::string name;
		Type type;
		unsigned count;
		
		
	public:
		
		
};

#endif

