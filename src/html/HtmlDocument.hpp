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

#ifndef HTML_DOCUMENT_H
#define HTML_DOCUMENT_H

#include "pugixml.hpp"
#include <string>

namespace html{
	class HtmlDocument{
		private:
			pugi::xml_document doc;
			pugi::xml_node head;
			pugi::xml_node body;
			
		public:
			HtmlDocument( std::string title );
			
			std::string output() const;
			
			operator pugi::xml_node() const{ return doc; }
			operator std::string() const{ return output(); }
	};
	
	inline pugi::xml_node attr( pugi::xml_node parent, std::string attribute, std::string value ){
		parent.append_attribute( attribute.c_str() ) = value.c_str();
		return parent;
	}
	
	inline pugi::xml_node p( pugi::xml_node parent, std::string text ){
		pugi::xml_node p = parent.append_child( "p" );
		p.text().set( text.c_str() );
		return p;
	}
	
	inline pugi::xml_node div( pugi::xml_node parent ){
		return parent.append_child( "div" );
	}
	inline pugi::xml_node div( pugi::xml_node parent, std::string attribute, std::string value ){
		return attr( div( parent ), attribute, value );
	}
	
	inline pugi::xml_node image( pugi::xml_node parent, std::string src, std::string alt ){
		pugi::xml_node img = attr( parent.append_child( "img" ), "src", src );
		return attr( img, "alt", alt );
	}
}

#endif

