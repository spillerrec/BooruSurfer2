#include "myhtml/api.h"

#include <stdexcept>
#include <string_view>
#include <string>
#include <iostream>

namespace MyHtml{

void validate( void* ptr ){
	if( !ptr )
		throw std::runtime_error( "Failed to alloc MyHtml object" ); //TODO: new exception?
}
void validate( mystatus_t status_code ){
	if( status_code != MyHTML_STATUS_OK )
		throw std::runtime_error( "MyHTML call failed with code" + std::to_string(status_code) ); //TODO: Show code?
}


class Tag{
	public:
		myhtml_tag_id_t tag_id;
		
	public:
		Tag( myhtml_tag_id_t id ) : tag_id(id) { }
		
		bool isText()    const{ return tag_id == MyHTML_TAG__TEXT; }
		bool isComment() const{ return tag_id == MyHTML_TAG__COMMENT; }
		bool hasText() const{ return isText() || isComment(); }
		
		bool operator==( Tag other ) const{ return tag_id == other.tag_id; }
};

class Attribute{
	private:
		myhtml_tree_attr_t* attr;
		
	public:
		Attribute( myhtml_tree_attr_t* attr ) : attr(attr) { }
		
 		Attribute next(){ return myhtml_attribute_next( attr ); }
 		Attribute prev(){ return myhtml_attribute_prev( attr ); }
 		
 		std::string_view key(){
			size_t length = 0;
			auto str = myhtml_attribute_key( attr, &length );
			return { str, length };
		}
 		
 		std::string_view value(){
			size_t length = 0;
			auto str = myhtml_attribute_value( attr, &length );
			return { str, length };
		}
		
		Attribute& operator*(){ return *this; }
		bool operator !=( Attribute other ){ return attr != other.attr; }
		Attribute& operator++(){ return *this = next(); }
};

class AttributeList{
	private:
		Attribute first;
		
	public:
		AttributeList( Attribute first ) : first(first) { }
		
		Attribute begin(){ return first; }
		Attribute end(){ return nullptr; }
		
		std::string_view valueOf( std::string_view key ){
			for( auto attr : *this )
				if( attr.key() == key )
					return attr.value();
			return {};
		}
};

class Node{
	private:
		myhtml_tree_node_t* node = nullptr;
		
	public:
		Node() = default;
		Node( myhtml_tree_node_t* node ) : node(node) { }
		
		myhtml_tree_node_t* get_internal(){ return node; }
		
		Node next()     { return { node ? myhtml_node_next(       node ) : nullptr }; }
		Node prev()     { return { node ? myhtml_node_prev(       node ) : nullptr }; }
		Node parent()   { return { node ? myhtml_node_parent(     node ) : nullptr }; }
		Node child()    { return { node ? myhtml_node_child(      node ) : nullptr }; }
		Node lastChild(){ return { node ? myhtml_node_last_child( node ) : nullptr }; }
		
		Node child( Tag t ){
			for( auto child : *this )
				if( child.getTag() == t )
					return child;
			return {};
		}
		Node next( Tag t ){
			Node search = next();
			do{
				if( search.getTag() == t )
					return search;
				search = search.next();
			} while( search );
			return {}; //No such node found :(
		}
		
		std::string_view text(){
			if( !node ) return {};
			
			size_t length = 0;
			auto str = myhtml_node_text( node, &length );
			return { str, length };
		}
		
		operator bool() const { return node != nullptr; }
		
		Node begin(){ return child(); };
		Node end(){ return Node{nullptr}; }
		
		Node& operator* (){ return *this; }
		Node& operator++(){ return *this = next(); }
		Node& operator--(){ return *this = prev(); }
		
		Tag getTag(){ return { myhtml_node_tag_id( node ) }; }
		
		AttributeList attributes() { return { { node ? myhtml_node_attribute_first( node ) : nullptr } }; }
};

class Html{
	private:
		myhtml_t* html = nullptr;
		
		
	public:
		Html(myhtml_options opt=MyHTML_OPTIONS_PARSE_MODE_SEPARATELY, size_t thread_count=1, size_t queue_size=4096)
		:	html( myhtml_create() )
		{
			if( !html )
				throw std::runtime_error( "Failed to alloc" );
			validate( myhtml_init( html, opt, thread_count, queue_size ) );
		}
		~Html() { myhtml_destroy( html ); }
		
		myhtml_t* get_internal(){ return html; }
};


class Tree{
	private:
		myhtml_tree_t* tree = nullptr;
		
	public:
		Tree( Html& html ) : tree( myhtml_tree_create() ){
			validate( tree );
			myhtml_tree_init( tree, html.get_internal() );
		}
		~Tree(){ myhtml_tree_destroy( tree ); }
		
		void parse( const char* html_string, size_t string_size, myencoding_t encoding=MyENCODING_UTF_8 ){
			validate( myhtml_parse( tree, encoding, html_string, string_size ) );
		}
		
		Node getDocument()
			{ return { myhtml_tree_get_document(tree) }; }
		
		Node getHtml()
			{ return { myhtml_tree_get_node_html(tree) }; }
			
		Node getHead()
			{ return { myhtml_tree_get_node_head(tree) }; }
			
		Node getBody()
			{ return { myhtml_tree_get_node_body(tree) }; }
		
		myhtml_tree_t* get_internal(){ return tree; }
		
		std::string_view tagName( Tag t ){
			size_t length = 0;
			auto str = myhtml_tag_name_by_id( tree, t.tag_id, &length );
			return { str, length };
		}
		
		Tag tag( std::string_view name ){
			return { myhtml_tag_id_by_name( tree, name.data(), name.length() ) };
		}
};

struct SearchIt{
	myhtml_tree_node_t** current;
	
	Node operator*(){ return { *current }; }
	
	SearchIt& operator ++() {
		current++;
		return *this;
	}
	
	bool operator !=(const SearchIt& other) const
		{ return current != other.current; }
};

class Search{
	private:
		myhtml_collection_t* collection = nullptr;
		Node scope;
		
	public:
		Search( size_t size=1024 ){
			mystatus_t error = MyHTML_STATUS_OK;
			collection = myhtml_collection_create( size, &error ); //TODO: Avoid?
			validate( collection );
			validate( error );
		}
		Search( Node scope, size_t size=1024 ) : Search(size) { this->scope = scope; }
		Search( Search&& other ) : collection(other.collection)
      		{ other.collection = nullptr; }
		~Search(){ /*myhtml_collection_destroy( collection );*/ }
		
		Search& byName( Tree& tree, std::string_view name ){
			mystatus_t error = MyHTML_STATUS_OK;
			myhtml_get_nodes_by_name( tree.get_internal(), collection, name.data(), name.length(), &error );
			validate( error );
			return *this;
		}
		
		Search& setAttrValue( Tree& tree, std::string_view attribute, std::string_view value, bool case_sensitive=true ){
			mystatus_t error = MyHTML_STATUS_OK;
			myhtml_get_nodes_by_attribute_value(
					tree.get_internal()
				,	collection
				,	scope.get_internal()
				,	case_sensitive
				,	attribute.data(), attribute.length()
				,	value.data(), value.length()
				,	&error
				);
			validate( error );
			return *this;
		}
         static Search byAttrValue(Tree& tree, std::string_view attribute, std::string_view value, bool case_sensitive=true ){
            Search s;
            s.setAttrValue(tree, attribute, value, case_sensitive);
            return s;
         }
		
		Search& setAttrValueSpaceSeperated( Tree& tree, std::string_view attribute, std::string_view value, bool case_sensitive=true ){
			mystatus_t error = MyHTML_STATUS_OK;
			myhtml_get_nodes_by_attribute_value_whitespace_separated(
					tree.get_internal()
				,	collection
				,	scope.get_internal()
				,	case_sensitive
				,	attribute.data(), attribute.length()
				,	value.data(), value.length()
				,	&error
				);
			validate( error );
			return *this;
		}
		static Search byAttrValueSpaceSeperated( Tree& tree, std::string_view attribute, std::string_view value, bool case_sensitive=true ){
		   Search s;
		   s.setAttrValueSpaceSeperated(tree, attribute, value, case_sensitive);
		   return s;
		}
		
		static Node byId( Tree& tree, std::string_view id, bool case_sensitive=true ){
		   return byAttrValue(tree, "id", id, case_sensitive ).first();
		}
		
		SearchIt begin() { return { collection ? collection->list                      : nullptr }; }
		SearchIt end()   { return { collection ? collection->list + collection->length : nullptr }; }
		
		Node first(){ return collection->length > 0 ? *begin() : Node(); }
};


}
