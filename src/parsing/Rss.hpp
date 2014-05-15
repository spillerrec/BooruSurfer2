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

#ifndef RSS_HPP
#define RSS_HPP

#include "pugixml.hpp"
#include <string>
#include <vector>

static const char* as_string( std::string value ){ return value.c_str(); }
static const char* as_string( int value ){ return std::to_string( value ).c_str(); }
static const char* as_string( bool value ){ return value ? "true" : "false"; }
static void from_string( bool& result, std::string value ){ result = value == "true"; }
static void from_string( int& result, std::string value ){
	try{ result = std::stoi( value ); }
	catch(...){ result = -1; }
}
static void from_string( std::string& result, std::string value ){ result = value; }

static bool is_valid( std::string value ){ return !value.empty(); }
static bool is_valid( int value ){ return value >= 0; }
static bool is_valid( bool value ){ return true; } //TODO: do something useful?

template<typename T=std::string>
class Attribute{
	private:
		const char* const name;
	public:
		T value;
		
		Attribute( const char* const name, T value = T() ) : name(name), value(value) { }
		
		void load( pugi::xml_node& node ){
			from_string( value, node.attribute( name ).as_string() );
		}
		
		void save( pugi::xml_node& node ) const{
			if( is_valid( value ) )
				node.append_attribute( name ) = as_string( value );
		}
};

template<typename T=std::string>
class Value{
	private:
		const char* const name;
	public:
		T value;
		
		Value( const char* const name, T value = T() ) : name(name), value(value) { }
		
		void load( pugi::xml_node& node ){
			from_string( value, node.child( name ).child_value() );
		}
		
		void save( pugi::xml_node& node ) const{
			if( is_valid( value ) )
				node.append_child( name ).append_child( pugi::node_pcdata ).set_value( as_string( value ) );
		}
};

class Structure{
	private:
		const char* const name;
		void save_internal2( pugi::xml_node& node ) const { }
		template<typename T, typename... Args>
		void save_internal2( pugi::xml_node& node, T&& arg, Args&&... args ) const{
			arg.save( node );
			save_internal2( node, args... );
		}
		void load_internal2( pugi::xml_node& node ) { }
		template<typename T, typename... Args>
		void load_internal2( pugi::xml_node& node, T&& arg, Args&&... args ){
			arg.load( node );
			load_internal2( node, args... );
		}
	protected:
		template<typename... Args>
		void save_internal( pugi::xml_node& node, Args&&... args ) const{
			auto base = node.append_child( name );
			save_internal2( base, args... );
		}
		template<typename... Args>
		void load_internal( pugi::xml_node& node, Args&&... args ){
			auto root = node.child( name );
			load_internal2( root, args... );
		}
		template<typename... Args>
		void load_internal_vector( pugi::xml_node& node, Args&&... args ){
			//TODO: fix
			load_internal2( node, args... );
		}
		Structure( const char* const name ) : name(name) { }
};

class Rss : Structure{
	private:
		
	public: //Data structures
		struct Image : Structure{
			//Required
			Value<> url{ "url" };
			Value<> title{ "title" };
			Value<> link{ "link" };
			
			//Optional
			Value<int> width{ "width", -1 };
			Value<int> height{ "height", -1 };
			Value<> description{ "description" };
			
			Image( const char* const name ) : Structure( name ) { }
			void save( pugi::xml_node& node ) const{
				save_internal( node, url, title, link, width, height, description );
			}
			void load( pugi::xml_node& node ){
				load_internal( node, url, title, link, width, height, description );
			}
		};
		struct Source : Structure{
			//Optional?
			Value<> name{ "name" };
			
			//Required
			Value<> url{ "url" };
			
			Source( const char* const name ) : Structure( name ) { }
			void load( pugi::xml_node& node ){
				load_internal( node, name, url );
			}
			void save( pugi::xml_node& node ) const{
				save_internal( node, name, url );
			}
		};
		struct MediaContent : Structure{
			Attribute<> url{ "url" };
			MediaContent( const char* const name ) : Structure( name ) { }
			void load( pugi::xml_node& node ){ load_internal( node, url ); }
			void save( pugi::xml_node& node ) const{ save_internal( node, url ); }
		};
		struct MediaThumbnail : Structure{
			Attribute<> url{ "url" };
			MediaThumbnail( const char* const name ) : Structure( name ) { }
			void load( pugi::xml_node& node ){ load_internal( node, url ); }
			void save( pugi::xml_node& node ) const{ save_internal( node, url ); }
		};
		struct Item : Structure{
			//Optional, but must have either title or description
			Value<> title{ "title" };
			Value<> link{ "link" };
			Value<> description{ "description" };
			Value<> content{ "content:encoded" };
			Value<> author{ "author" };
			Value<> category{ "category" };
			//Value<> category_domain{ "" };
			Value<> comments{ "comments" };
			//Value<> enclosure;
			Value<> guid{ "guid" };
			bool guid_isPermaLink{ true }; //TODO: is it important if this is present or not?
			Value<> pubDate{ "pubDate" };
			Source source{ "source" };
			
			//Yahoo media extension
			MediaThumbnail media_thumbnail{ "media:thumbnail" };
			MediaContent media_content{ "media:content" };
			
			Item( const char* const name ) : Structure( name ) { }
			void load( pugi::xml_node& node ){
				load_internal_vector( node, title, link, description, content, author, category, comments, guid, pubDate, source, media_thumbnail, media_content );
			}
			void save( pugi::xml_node& node ) const{
				save_internal( node, title, link, description, content, author, category, comments, guid, pubDate, source, media_thumbnail, media_content );
			}
		};
		
	public: //Root attributes
		//Required
		Value<> title{ "title" };
		Value<> link{ "link" };
		Value<> description{ "description" };
		
		//Optional
		Value<> language{ "language" };
		Value<> copyright{ "copyright" };
		Value<> managingEditor{ "managingEditor" };
		Value<> webMaster{ "webMaster" };
		Value<> pubDate{ "pubDate" };
		Value<> lastBuildDate{ "lastBuildDate" };
		Value<> category{ "category" };
		Value<> generator{ "generator" };
		Value<> docs{ "docs" };
		//Cloud cloud;
		Value<int> ttl{ "ttl", -1 };
		
		std::vector<Item> items;
		
	public: //Parsing
		Rss() : Structure("channel"){ }
		Rss( const pugi::xml_document& doc );
		
		std::string save(/* const pugi::xml_document& doc */) const;
};

#endif

