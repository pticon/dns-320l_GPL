/*
	REST API item list ("Feed") implementation
	Copyright (C) 2015  Vitaliy Filippov

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "CommonUri.hh"
#include "Feed2.hh"
#include "Entry2.hh"
#include "util/OS.hh"

#include "http/Agent.hh"
#include "http/Header.hh"
#include "json/Val.hh"
#include "json/ValResponse.hh"
#include "util/log/Log.hh"

#include <iostream>
#include <boost/format.hpp>

namespace gr { namespace v2 {

Feed2::Feed2( const std::string& url ):
	Feed( url )
{
}

bool Feed2::GetNext( http::Agent *http )
{
	if ( m_next.empty() )
		return false ;
	
	http::ValResponse out ;
	int max_retry_time = 0;
	while(true){
		long response = http->Get( m_next, &out, http::Header() ) ;
		if(response == 410 || response == 412){
			if(max_retry_time <= 5){
					max_retry_time++;
					Log( "GetNext request, request failed with %1%, retrying whole upload in 5s, max_retry_time = %2%", response, max_retry_time, log::warning ) ;
					os::Sleep( 5 );
				}else{
					Log( "GetNext request, request failed with %1%, retrying max times exceed to 5 times, ignore it, next.", response, log::warning ) ;
					max_retry_time = 0;
					return false;
				}
		}else
			break;
	}
	Val m_content = out.Response() ;
	
	Val::Array items = m_content["items"].AsArray() ;
	m_entries.clear() ;
	for ( Val::Array::iterator i = items.begin() ; i != items.end() ; ++i )
		m_entries.push_back( Entry2( *i ) );
	
	Val url ;
	m_next = m_content.Get( "nextLink", url ) ? url : std::string( "" ) ;
	return true ;
}

} } // end of namespace gr::v2
