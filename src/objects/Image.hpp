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

#ifndef IMAGE_H
#define IMAGE_H

#include <string>

class Image{
	public:
		enum Size{
				THUMB
			,	RESIZED
			,	COMPRESSED
			,	ORIGINAL
		};
		static Size from_string( std::string input );
		
		std::string url;
		int width=0;
		int height=0;
		int size=0;
		
		bool isLocal() const;
		std::string localPath() const;
		
		void setLocalPath( const std::string& path )
			{ url = "file:///" + path; }
};

#endif

