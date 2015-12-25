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

#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include <boost/optional.hpp>

#include <algorithm>
#include <string>
#include <vector>

template<typename T>
class ArrayView{
	private:
		const T* data{ nullptr };
		unsigned size{ 0 };
		
	public:
		ArrayView() { }
		ArrayView( const T* data, unsigned size )
			:	data(data), size(size) { }
		ArrayView( const std::vector<T>& arr )
			:	data(arr.data()), size(arr.size()) { }
		ArrayView( const std::basic_string<T>& str )
			:	data(str.data()), size(str.size()) { }
			
		const T* begin() const{ return data; }
		const T* end()   const{ return data+size; }
		
	//Query
		unsigned lenght() const{ return size; }
		bool isEmpty() const{ return size == 0; }
		bool isNotEmpty() const{ return !isEmpty(); }
		
	//Conversions
		std::basic_string<T> toString() const{ return { data, size }; }
		
	//Reduce view space
		ArrayView<T> middle( unsigned start, unsigned lenght ) const{
			if( start > size )
				return {};
			if( start+lenght > size )
				lenght = size-start;
			return { data+start, lenght };
		}
		ArrayView<T> left( unsigned lenght ) const
			{ return middle( 0, lenght ); }
		ArrayView<T> right( unsigned lenght ) const{
			if( size < lenght )
				lenght = size;
			return middle( size-lenght, lenght );
		}
		
		ArrayView<T> removeLeft( unsigned count ) const
			{ return right( size-count ); }
		
		ArrayView<T> removeRight( unsigned count ) const
			{ return left( size-count ); }
		
		
	//Comparisions
		bool isEqual( ArrayView<T> other ) const{
			if( size != other.size )
				return false;
			return std::equal( begin(), end(), other.begin(), other.end() );
		}
		
		bool startsWith( ArrayView<T> other ) const{
			if( other.size > size )
				return false;
			return left( other.size ).isEqual( other );
		}
		bool endsWith( ArrayView<T> other ) const{
			if( other.size > size )
				return false;
			return right( other.size ).isEqual( other );
		}
		
	//Searching
		boost::optional<unsigned> find( const T& val ) const{
			auto it = std::find( begin(), end(), val );
			if( it != end() )
				return it-begin();
			else
				return boost::none;
		}
		
		template<typename Predicate>
		boost::optional<unsigned> findIf( Predicate func ) const{
			auto it = std::find_if( begin(), end(), func );
			if( it != end() )
				return it-begin();
			else
				return boost::none;
		}
		
		std::vector<ArrayView<T>> split( const T& value ) const{
			std::vector<ArrayView<T>> arr;
			auto remaining = *this;
			
			while( remaining.isNotEmpty() ){
				auto pos = remaining.find( value );
				if( pos ){
					arr.push_back( remaining.left( *pos ) );
					remaining = remaining.right( remaining.lenght() - *pos - 1 );
				}
				else{
					arr.push_back( remaining );
					remaining = {};	
				}
			}
			
			return arr;
		}
		
	//Remove
		template<typename Predicate>
		ArrayView<T> trimLeft( Predicate func ) const{
			unsigned pos=0;
			for( ; pos<size && func(data[pos]); pos++ );
			return removeLeft(pos);
		}
		
		template<typename Predicate>
		ArrayView<T> trimRight( Predicate func ) const{
			unsigned pos=0;
			for( ; pos<size && func(data[size-pos-1]); pos++ );
			return removeRight(pos);
		}
		
		template<typename Predicate>
		ArrayView<T> trim( Predicate func ) const
			{ return trimLeft( func ).trimRight( func ); }
		
		ArrayView<T> removePrefix( ArrayView<T> prefix ) const{
			if( startsWith( prefix ) )
				return removeLeft( prefix.lenght() );
			return {}; //TODO: throw?
		}
		ArrayView<T> removePostfix( ArrayView<T> postfix ) const{
			if( endsWith( postfix ) )
				return removeRight( postfix.lenght() );
			return {}; //TODO: throw?
		}
};

using StringView=ArrayView<char>;

StringView cStrView( const char* str ){
	return StringView( str, strlen( str ) );
}

template<typename T>
bool operator<( ArrayView<T> arr1, ArrayView<T> arr2 ){
	return std::lexicographical_compare( arr1.begin(), arr1.end(), arr2.begin(), arr2.end() );
}

template<typename T>
bool operator==( ArrayView<T> arr1, ArrayView<T> arr2 ){
	return arr1.isEqual( arr2 );
}

#endif

