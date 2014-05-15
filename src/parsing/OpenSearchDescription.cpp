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

#include "OpenSearchDescription.hpp"

using namespace std;
using namespace pugi;

OpenSearchDescription::OpenSearchDescription( const xml_document& doc ) : OpenSearchDescription(){
	load_internal( (xml_node&)doc, xmlns, short_name, description, contact, tags, long_name, developer, attribution, adult_content, language, input_encoding, output_encoding );
	
	auto open_search = doc.child( name );
	for( auto it : open_search.children( "Url" ) ){
		Url url;
		url.load( it );
		urls.push_back( url );
	}
	for( auto it : open_search.children( "Image" ) ){
		Image img;
		img.load( it );
		images.push_back( img );
	}
}

static string to_string( const pugi::xml_document& doc ){
	struct xml_string_writer : pugi::xml_writer{
		std::string result;
		virtual void write( const void* data, size_t size ) override{
			result += std::string( static_cast<const char*>( data ), size );
		}
	} writer;
	doc.print( writer );
	return writer.result;
}

string OpenSearchDescription::save(/* const pugi::xml_document& doc */) const{
	xml_document doc;
	
	auto decl = doc.prepend_child( node_declaration );
	decl.append_attribute( "version" ) = "1.0";
	decl.append_attribute( "encoding" ) = "UTF-8";
	
	save_internal( (xml_node&)doc, xmlns, short_name, description, contact, tags, long_name, developer, attribution, adult_content, language, input_encoding, output_encoding );
	
	auto open_search = doc.child( name );
	for( auto url : urls )
		url.save( open_search );
	for( auto image : images )
		image.save( open_search );
	
	return to_string( doc );
}
