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

#include "XmlBase.hpp"

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

