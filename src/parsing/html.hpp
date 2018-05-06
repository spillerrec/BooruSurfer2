#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include <string>
#include <iterator>
#include <iostream>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "../Server.hpp"

namespace HTML{
	class Node;
	
	class Document{
		public:
			rapidxml::xml_document<> doc;
			rapidxml::xml_node<>* node;
			
		public:
			Document() : doc(){
				/* XHTML
				auto declaration = doc.allocate_node( rapidxml::node_declaration );
				declaration->append_attribute( doc.allocate_attribute( "version", "1.0" ) );
				declaration->append_attribute( doc.allocate_attribute( "encoding", "UTF-8" ) );
				doc.append_node( declaration );
				/*/// HTML5
				auto doctype = doc.allocate_node( rapidxml::node_doctype );
				doctype->value( "html" );
				doc.append_node( doctype );
				//*/
				
				/* XHTML
				node = doc.allocate_node( rapidxml::node_element, "html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"" );
				/*/// HTML5
				node = doc.allocate_node( rapidxml::node_element, "html" );
				//*/
				doc.append_node( node );
			}
			
			Node html();
			
			void print_to_cout(){
				rapidxml::print( std::cout, doc, 0 );
			}
			
			operator std::string() const{
				std::string s;
				rapidxml::print( std::back_inserter( s ), doc, rapidxml::print_no_indenting );
				return s;
			}
	};
	
	class Node{
		protected:
			rapidxml::xml_document<>& doc;
			rapidxml::xml_node<>* node;
			
		private:
			const char* const get_text( const char* const text ){ return text; }
			const char* const get_text( const std::string text ){ return doc.allocate_string( text.c_str() ); }
			
			Node& add( const Node& node ){
				this->node->append_node( node.node );
				return *this;
			}
			Node& add( const char* const text ){
				node->append_node( doc.allocate_node( rapidxml::node_data, 0, get_text( text ) ) );
				return *this;
			}
			Node& add( const std::string& s ){ return add( get_text( s ) ); }
			template<typename T>
			Node& add( const T& other ){ return add( boost::lexical_cast<std::string>( other ) ); }
			
		public:
			template<typename T1, typename T2>
			void add_attr( std::pair<T1,T2> attr ){
				node->append_attribute( doc.allocate_attribute( get_text( attr.first ), get_text( attr.second ) ) );
			}
			
		public:
			Node( const char* const name, Document& doc ) : doc( doc.doc ){
				node = this->doc.allocate_node( rapidxml::node_element, name );
				//TODO: attributes
			}
			
			template<typename T, typename... Args>
			Node( const char* const name, Document& doc, T&& arg1, Args&&... args ) : Node( name, doc, args... ){
				add_attr( arg1 );
			}
			
			Node( rapidxml::xml_document<>& doc, rapidxml::xml_node<>* node ) : doc( doc ), node( node ){ }
			
			Node& operator()(){ return *this; }
			template<typename T, typename... Args>
			inline Node& operator()( T&& Arg1, Args&&... args ){
				return add( Arg1 )( args... );
			}
			
			operator std::string() const{
				std::string s;
				rapidxml::print( std::back_inserter( s ), *node, rapidxml::print_no_indenting );
				return s;
			}
	};
	
	inline Node Document::html(){
		return Node( doc, node );
	}
	
//HTML elements
	#define NORMAL_HTML_NODE( name ) \
	template<typename... Args> inline Node name ( Document& doc, Args&&... args ){ return Node( #name , doc, args... ); }
	
	//Metadata and scripting
	NORMAL_HTML_NODE( head )
	NORMAL_HTML_NODE( title )
	NORMAL_HTML_NODE( meta )
	NORMAL_HTML_NODE( base )
	NORMAL_HTML_NODE( link )
	NORMAL_HTML_NODE( style )
	NORMAL_HTML_NODE( noscript )
	NORMAL_HTML_NODE( script )
	
	//Text-level semantics
	NORMAL_HTML_NODE( span )
	NORMAL_HTML_NODE( a )
	NORMAL_HTML_NODE( rt )
	NORMAL_HTML_NODE( rp )
	
	NORMAL_HTML_NODE( dfn )
	NORMAL_HTML_NODE( abbr )
	NORMAL_HTML_NODE( q )
	NORMAL_HTML_NODE( cite )
	
	NORMAL_HTML_NODE( em )
	NORMAL_HTML_NODE( time )
	NORMAL_HTML_NODE( var )
	NORMAL_HTML_NODE( samp )
	
	NORMAL_HTML_NODE( i )
	NORMAL_HTML_NODE( b )
	NORMAL_HTML_NODE( sub )
	NORMAL_HTML_NODE( sup )
	
	NORMAL_HTML_NODE( small )
	NORMAL_HTML_NODE( strong )
	NORMAL_HTML_NODE( mark )
	NORMAL_HTML_NODE( ruby )
	
	NORMAL_HTML_NODE( ins )
	NORMAL_HTML_NODE( del )
	NORMAL_HTML_NODE( bdi )
	NORMAL_HTML_NODE( bdo )
	
	NORMAL_HTML_NODE( s )
	NORMAL_HTML_NODE( kdb )
	NORMAL_HTML_NODE( wbr )
	NORMAL_HTML_NODE( code )
	
	//Grouping content
	NORMAL_HTML_NODE( br )
	NORMAL_HTML_NODE( hr )
	NORMAL_HTML_NODE( figcaption )
	NORMAL_HTML_NODE( figure )
	
	NORMAL_HTML_NODE( p )
	NORMAL_HTML_NODE( ol )
	NORMAL_HTML_NODE( ul )
	NORMAL_HTML_NODE( li )
	
	NORMAL_HTML_NODE( div )
	NORMAL_HTML_NODE( pre )
	NORMAL_HTML_NODE( blockquote )
	NORMAL_HTML_NODE( dl )
	NORMAL_HTML_NODE( dt )
	NORMAL_HTML_NODE( dd )
	
	//Forms
	NORMAL_HTML_NODE( fieldset )
	NORMAL_HTML_NODE( meter )
	NORMAL_HTML_NODE( legend )
	NORMAL_HTML_NODE( label )
	NORMAL_HTML_NODE( input )
	NORMAL_HTML_NODE( textarea )
	
	NORMAL_HTML_NODE( form )
	NORMAL_HTML_NODE( select )
	NORMAL_HTML_NODE( optgroup )
	NORMAL_HTML_NODE( option )
	NORMAL_HTML_NODE( output )
	NORMAL_HTML_NODE( button )
	
	NORMAL_HTML_NODE( datalist )
	NORMAL_HTML_NODE( keygen )
	NORMAL_HTML_NODE( progress )
	
	//Document sections
	NORMAL_HTML_NODE( body )
	NORMAL_HTML_NODE( aside )
	NORMAL_HTML_NODE( address )
	
	NORMAL_HTML_NODE( h1 )
	NORMAL_HTML_NODE( h2 )
	NORMAL_HTML_NODE( h3 )
	NORMAL_HTML_NODE( h4 )
	NORMAL_HTML_NODE( h5 )
	NORMAL_HTML_NODE( h6 )
	
	NORMAL_HTML_NODE( section )
	NORMAL_HTML_NODE( header )
	NORMAL_HTML_NODE( nav )
	NORMAL_HTML_NODE( article )
	NORMAL_HTML_NODE( footer )
	NORMAL_HTML_NODE( hgroup )
	
	//Interactive elements
	NORMAL_HTML_NODE( menu )
	NORMAL_HTML_NODE( command )
	NORMAL_HTML_NODE( summary )
	NORMAL_HTML_NODE( details )
	
	//Tabular data
	NORMAL_HTML_NODE( col )
	NORMAL_HTML_NODE( colgroup )
	NORMAL_HTML_NODE( caption )
	
	NORMAL_HTML_NODE( table )
	NORMAL_HTML_NODE( tr )
	NORMAL_HTML_NODE( td )
	NORMAL_HTML_NODE( th )
	NORMAL_HTML_NODE( tbody )
	NORMAL_HTML_NODE( thead )
	NORMAL_HTML_NODE( tfoot )
	
	//Embedding content
	NORMAL_HTML_NODE( img )
	NORMAL_HTML_NODE( area )
	NORMAL_HTML_NODE( map )
	NORMAL_HTML_NODE( embed )
	NORMAL_HTML_NODE( object )
	NORMAL_HTML_NODE( param )
	NORMAL_HTML_NODE( source )
	NORMAL_HTML_NODE( iframe )
	NORMAL_HTML_NODE( canvas )
	NORMAL_HTML_NODE( track )
	NORMAL_HTML_NODE( audio )
	NORMAL_HTML_NODE( video )
	
	
//Attributes
	#define HTML_ATTRIBUTE( name, value ) \
	template<typename T> \
	inline std::pair<const char* const, T> name ( T value_real ){ return std::pair<const char* const, T>( value , value_real ); }
	
	HTML_ATTRIBUTE( ID, "id" )
	HTML_ATTRIBUTE( CLASS, "class" )
	HTML_ATTRIBUTE( STYLE, "style" )
	HTML_ATTRIBUTE( SRC, "src" )
	HTML_ATTRIBUTE( ALT, "alt" )
	HTML_ATTRIBUTE( REL, "rel" )
	HTML_ATTRIBUTE( TITLE, "title" )
	HTML_ATTRIBUTE( TYPE, "type" )
	HTML_ATTRIBUTE( NAME, "name" )
	HTML_ATTRIBUTE( CONTENT, "content" )
	HTML_ATTRIBUTE( PLACEHOLDER, "placeholder" )
	
	HTML_ATTRIBUTE( HREF, "href" )
	
	HTML_ATTRIBUTE( LOOP, "loop" )
	HTML_ATTRIBUTE( CONTROLS, "controls" )
	HTML_ATTRIBUTE( AUTOPLAY, "autoplay" )
	
	HTML_ATTRIBUTE( DATA, "data" )
	HTML_ATTRIBUTE( VALUE, "value" )
	HTML_ATTRIBUTE( WIDTH, "width" )
	HTML_ATTRIBUTE( HEIGHT, "height" )
	
	HTML_ATTRIBUTE( HTTP_EQUIV, "http-equiv" )
}


