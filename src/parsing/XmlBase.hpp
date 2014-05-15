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

#ifndef XML_BASE_HPP
#define XML_BASE_HPP

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
	protected:
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
		
		template<typename T2>
		Attribute& operator=( T2 new_val ){
			value = new_val;
			return *this;
		}
};

template<typename T=std::string>
class Value{
	protected:
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
		
		template<typename T2>
		Value& operator=( T2 new_val ){
			value = new_val;
			return *this;
		}
};

class Structure{
	protected:
		const char* const name;
	private:
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


#endif

