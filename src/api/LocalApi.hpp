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

#ifndef LOCAL_API_H
#define LOCAL_API_H

#include "Api.hpp"

#include <string>
#include <vector>

class LocalApi : public Api{
	public:
		LocalApi( std::string short_hand );

		std::string get_name() const override{ return "Local files"; }
		std::string get_shorthand() const override{ return "local-" + site_name; }
		std::string get_url() const override{ return "file:///"; }

		Post fetch_post( unsigned id, Image::Size level ) override;
		Post get_post( unsigned id, Image::Size level, bool force_refresh ) override;
		Index get_index( std::string search, int page, int limit=-1 ) override;

		ThemeColor main_color() override{ return {0xA4,0x81,0x5E}; }
		ThemeColor secondary_color() override{ return {0xFF,0xFF,0xFF}; }

		std::string original_post_url( unsigned id ) override{ return {}; }
		std::string original_index_url( std::string search ) override{ return {}; }
};

#endif
