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

#include "Rss.hpp"

using namespace std;
using namespace pugi;

Rss::Rss( const xml_document& doc ) : Rss(){
	auto rss = doc.child("rss");
	auto channel = rss.child("channel");
	
	load_internal( rss, title, link, description, language, copyright, managingEditor, webMaster, pubDate, lastBuildDate, category, generator, docs, ttl );
	
	for( auto it : channel.children( "item" ) ){
		Item item{ "item" };
		item.load( it );
		items.push_back( item );
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

static void add_optional( xml_node& node, const char* name, string value ){
	if( !value.empty() )
		node.append_child( name ).append_child(pugi::node_pcdata).set_value( value.c_str() );
}

string Rss::save(/* const pugi::xml_document& doc */) const{
	xml_document doc;
	
	auto decl = doc.prepend_child( node_declaration );
	decl.append_attribute( "version" ) = "1.0";
	decl.append_attribute( "encoding" ) = "UTF-8";
	
	auto root = doc.append_child( "rss" );
	root.append_attribute( "version" ) = "2.0";
	root.append_attribute( "xmlns:media" ) = "http://search.yahoo.com/mrss/";
	
	save_internal( root, title, link, description, language, copyright, managingEditor, webMaster, pubDate, lastBuildDate, category, generator, docs, ttl );
	
	auto channel = root.child("channel");
	for( auto item : items )
		item.save( channel );
	
	return to_string( doc );
}
