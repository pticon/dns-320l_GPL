/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

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

#include "AuthAgent.hh"

#include "http/Error.hh"
#include "http/Header.hh"
#include "http/XmlResponse.hh"
#include "json/ValResponse.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "util/File.hh"

#include <cassert>

namespace gr {

using namespace http ;
AuthAgent::AuthAgent( const OAuth2& auth, std::auto_ptr<Agent> real_agent ) :
	m_auth	( auth ),
	m_agent	( real_agent )
{
	assert( m_agent.get() != 0 ) ;
}

Header AuthAgent::AppendHeader( const Header& hdr ) const
{
	Header h(hdr) ;
	h.Add( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	h.Add( "GData-Version: 3.0" ) ;
	return h ;
}

long AuthAgent::Put(
	const std::string&	url,
	const std::string&	data,
	DataStream			*dest,
	const Header&		hdr)
{
	return Put(url, data, dest, hdr, false);
}

long AuthAgent::Put(
	const std::string&	url,
	const std::string&	data,
	DataStream			*dest,
	const Header&		hdr,
	bool largeFile)
{
	Header auth = AppendHeader(hdr) ;
	
	long response ;
	bool keepTrying = true;
	while ( keepTrying ) {
		response = m_agent->Put( url, data, dest, AppendHeader(hdr) );
		keepTrying = CheckRetry( response, largeFile );
		Log( "keepTrying = %1%, response = %2%", keepTrying, response, log::info ) ;
		if ( keepTrying  || response != 200) {
			ValResponse *valResponse = dynamic_cast<ValResponse*>(dest);
			if( valResponse ){
				//Log( "valResponse->Clear() called", log::info ) ;
				valResponse->Clear();
			}
		}
	}
	if(largeFile == false){
		// On 410 Gone or 412 Precondition failed, recovery may be possible so don't
		// throw an exception
		if ( response == 410 || response == 412)
			return response;
	}else{
		// drop all response to caller to decide.
		return response;
	}
	return CheckHttpResponse(response, url, AppendHeader(hdr) ) ;
}

long AuthAgent::Put(
	const std::string&	url,
	File				*file,
	DataStream			*dest,
	const Header&		hdr,
	bool largeFile)
{
	Header auth = AppendHeader(hdr) ;
	
	long response ;
	bool keepTrying = true;
	while ( keepTrying ) {
		response = m_agent->Put( url, file, dest, AppendHeader(hdr), largeFile );
		keepTrying = CheckRetry( response , largeFile);
		Log( "keepTrying = %1%, response = %2%", keepTrying, response, log::info ) ;
		if ( keepTrying  || response != 200) {
			file->Seek( 0, SEEK_SET );
			ValResponse *valResponse = dynamic_cast<ValResponse*>(dest);
			if( valResponse ){
				Log( "valResponse->Clear() called", log::info ) ;
				valResponse->Clear();
			}
		}
		
	}
	if(largeFile == false){
		// On 410 Gone or 412 Precondition failed, recovery may be possible so don't
		// throw an exception
		if ( response == 410 || response == 412)
			return response;
	}else{
		// drop all response to caller to decide.
		return response;
	}
	return CheckHttpResponse(response, url, AppendHeader(hdr) ) ;
}

long AuthAgent::Put(
	const std::string&	url,
	File				*file,
	DataStream			*dest,
	const Header&		hdr)
{
	return Put(url, file, dest, hdr, false);
}

long AuthAgent::Get(
	const std::string& 	url,
	DataStream			*dest,
	const Header&		hdr )
{
	return Get(url, dest, hdr, false);
}

long AuthAgent::Get(
	const std::string& 	url,
	DataStream			*dest,
	const Header&		hdr,
	bool largeFile)
{
	Header auth = AppendHeader(hdr) ;

	long response ;
	bool keepTrying = true;
	while ( keepTrying ) {
		response = m_agent->Get( url, dest, AppendHeader(hdr), largeFile );
		keepTrying = CheckRetry( response, largeFile );
		Log( "keepTrying = %1%, response = %2%", keepTrying, response, log::info ) ;
		if ( keepTrying  || response != 200) {
			ValResponse *valResponse = dynamic_cast<ValResponse*>(dest);
			if( valResponse ){
				Log( "valResponse->Clear() called", log::info ) ;
				valResponse->Clear();
			}
		}
	}
	if(largeFile == false){
		// On 410 Gone or 412 Precondition failed, recovery may be possible so don't
		// throw an exception
		if ( response == 410 || response == 412)
			return response;
	}else{
		// drop all response to caller to decide.
		return response;
	}
	return CheckHttpResponse(response, url, AppendHeader(hdr) ) ;
}

long AuthAgent::Post(
	const std::string& 	url,
	const std::string&	data,
	DataStream			*dest,
	const Header&		hdr)
{
	Header auth = AppendHeader(hdr) ;
	
	long response ;
	bool keepTrying = true;
	while ( keepTrying ) {
		response = m_agent->Post( url, data, dest, AppendHeader(hdr) );
		keepTrying = CheckRetry( response );
		Log( "keepTrying = %1%, response = %2%", keepTrying, response, log::info ) ;
		if ( keepTrying  || response == 410 || response == 412) {
			ValResponse *valResponse = dynamic_cast<ValResponse*>(dest);
			if( valResponse ){
				Log( "valResponse->Clear() called", log::info ) ;
				valResponse->Clear();
			}
		}
	}
	// On 410 Gone or 412 Precondition failed, recovery may be possible so don't
	// throw an exception
	if ( response == 410 || response == 412 )
		return response;
	return CheckHttpResponse(response, url, AppendHeader(hdr) ) ;
}

long AuthAgent::Custom(
	const std::string&	method,
	const std::string&	url,
	DataStream			*dest,
	const Header&		hdr )
{
	Header auth = AppendHeader(hdr) ;

	long response ;
	while ( CheckRetry(
		response = m_agent->Custom( method, url, dest, AppendHeader(hdr) ) ) ) ;
	
	return CheckHttpResponse(response, url, AppendHeader(hdr) ) ;
}

std::string AuthAgent::RedirLocation() const
{
	return m_agent->RedirLocation() ;
}

std::string AuthAgent::GetRange() const
{
	return m_agent->GetRange() ;
}

std::string AuthAgent::Escape( const std::string& str )
{
	return m_agent->Escape( str ) ;
}

std::string AuthAgent::Unescape( const std::string& str )
{
	return m_agent->Unescape( str ) ;
}

bool AuthAgent::CheckRetry( long response)
{
	return CheckRetry(response, false);
}

bool AuthAgent::CheckRetry( long response, bool largeFile)
{
#if 0
	// HTTP 500 and 503 should be temperory. just wait a bit and retry
	if ( response >= 500 && response < 600)
	{
		Log( "resquest failed due to temperory error: %1%. retrying in 5 seconds",
			response, log::warning ) ;
			
		os::Sleep( 5 ) ;
		return true ;
	}
	
	// HTTP 401 Unauthorized. the auth token has been expired. refresh it
	else if ( response == 401 )
	{
		Log( "resquest failed due to auth token expired: %1%. refreshing token",
			response, log::warning ) ;
			
		os::Sleep( 5 ) ;
		m_auth.Refresh() ;
		return true ;
	}
	else
		return false ;
#else
	if(largeFile == false){
		if (response == 401){
			Log( "resquest failed due to auth token expired: %1%. refreshing token",
				response, log::warning ) ;
			os::Sleep( 5 ) ;
			m_auth.Refresh() ;
			return true;
		}else if(response == 410 || response == 412){
			return false;
		}else if(response != 200){
			Log( "resquest failed due to temperory error: %1%. retrying in 5 seconds",
				response, log::warning ) ;
			os::Sleep( 5 ) ;
			return true;
		}else if(response == 200)
			return false;
	}else{
		if (response == 401){
			Log( "resquest failed due to auth token expired: %1%. refreshing token",
				response, log::warning ) ;
			os::Sleep( 5 ) ;
			m_auth.Refresh() ;
			return true;
		}else
			return false;
	}

#endif
}


long AuthAgent::CheckHttpResponse(
		long 				response,
		const std::string&	url,
		const http::Header&	hdr  )
{
	// throw for other HTTP errors
	if ( response >= 400 && response < 500 )
	{
 	/*	BOOST_THROW_EXCEPTION(
 			Error()
				<< HttpResponse( response )
 				<< Url( url )
				<< HttpHeader( hdr ) ) ;
*/
		Log( "CheckHttpResponse error: response = %1%, url = %2%", response , url, log::info ) ;
	}
	
	return response ;
}

} // end of namespace
