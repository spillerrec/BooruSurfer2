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

#ifndef OPEN_SEARCH_DESCRIPTION_HPP
#define OPEN_SEARCH_DESCRIPTION_HPP

#include "XmlBase.hpp"

class OpenSearchDescription : Structure{
	private:
		Attribute<> xmlns{ "xmlns", "http://a9.com/-/spec/opensearch/1.1/" };
		
	public:
		struct Url : Structure{
			Attribute<> temp{ "template" };
			Attribute<> type{ "type" };
			Attribute<> rel{ "rel" };
			Attribute<int> indexOffset{ "indexOffset", -1 };
			Attribute<int> pageOffset{ "pageOffset", -1 };
			
			Url() : Structure( "Url" ) { }
			void save( pugi::xml_node& node ) const
				{ save_internal( node, temp, type, rel, indexOffset, pageOffset ); }
			void load( pugi::xml_node& node )
				{ load_internal( node, temp, type, rel, indexOffset, pageOffset ); }
		};
		
		struct Image : Structure{
			Attribute<int> height{ "height", -1 };
			Attribute<int> width{ "width", -1 };
			Attribute<> type{ "type" };
			
			Image() : Structure( "Image" ) { }
			void save( pugi::xml_node& node ) const
				{ save_internal( node, width, height, type ); }
			void load( pugi::xml_node& node )
				{ load_internal( node, width, height, type ); }
		};
	public: //Root attributes
		//Required
		Value<> short_name{ "ShortName" };
		Value<> description{ "Description" };
		
		std::vector<Url> urls;
		
		//Optional
		Value<> contact{ "Contact" };
		Value<> tags{ "Tags" };
		Value<> long_name{ "LongName" };
		std::vector<Image> images;
		Value<> developer{ "Developer" };
		Value<> attribution{ "Attribution" };
		//SyndicationRight
		Value<bool> adult_content{ "AdultContent", false };
		Value<> language{ "Language" };
		Value<> input_encoding{ "InputEncoding" };
		Value<> output_encoding{ "OutputEncoding" };
		
	public: //Parsing
		OpenSearchDescription() : Structure("OpenSearchDescription"){ }
		OpenSearchDescription( const pugi::xml_document& doc );
		
		std::string save(/* const pugi::xml_document& doc */) const;
};

#endif

