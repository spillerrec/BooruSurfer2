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

#include "TimeHandling.hpp"

//TODO: Remove dependency on Poco
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>


Time Time::FromSimpleFormat( std::string_view formatted_string ){
	Poco::DateTime datetime;
	int timezone;
	
	Poco::DateTimeParser::parse( Poco::DateTimeFormat::SORTABLE_FORMAT, std::string(formatted_string), datetime, timezone );
	return { datetime.timestamp().epochTime() };
}

std::string Time::formatSimple() const{
	auto poco_time = Poco::Timestamp::fromEpochTime( value );
	return Poco::DateTimeFormatter::format( poco_time, "%Y-%m-%d %H:%M" );
}

std::string Time::formatRFC822() const{
	auto poco_time = Poco::Timestamp::fromEpochTime( value );
	return Poco::DateTimeFormatter::format( poco_time, Poco::DateTimeFormat::RFC1123_FORMAT );
}
