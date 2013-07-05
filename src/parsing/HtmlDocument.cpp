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

#include "HtmlDocument.hpp"

using namespace std;
using namespace pugi;
using namespace html;

HtmlDocument::HtmlDocument( string title ){
	//Set declaration
	xml_node declaration = doc.append_child( pugi::node_declaration );
	declaration.append_attribute( "version" ) = "1.0";
	declaration.append_attribute( "encoding" ) = "UTF-8";
	
	xml_node root = doc.append_child( "html" );
	head = root.append_child( "head" );
	body = root.append_child( "body" );
	
	head.append_child( "title" ).text().set( title.c_str() );
}


void HtmlDocument::add_stylesheet( std::string path, std::string media ){
	attr( attr( head.append_child( "link" ), "rel", "stylesheet" ), "href", path );
}


std::string HtmlDocument::output() const{
	//This is pretty much just the save_custom_writer.cpp example
	struct xml_string_writer : pugi::xml_writer{
		std::string result;
		virtual void write(const void* data, size_t size){
			result += std::string(static_cast<const char*>(data), size);
		}
	} writer;
	
	doc.print(writer);
	return writer.result;
}

