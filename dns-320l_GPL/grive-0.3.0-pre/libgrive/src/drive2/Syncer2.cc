/*
	REST API Syncer implementation
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

#include "base/Resource.hh"
#include "CommonUri.hh"
#include "Entry2.hh"
#include "Feed2.hh"
#include "Syncer2.hh"

#include "http/Agent.hh"
#include "http/Download.hh"
#include "http/Header.hh"
#include "http/StringResponse.hh"
//#include "http/ResponseLog.hh"
#include "json/ValResponse.hh"
#include "json/JsonWriter.hh"

#include "util/OS.hh"
#include "util/log/Log.hh"

#include <boost/exception/all.hpp>

#include <cassert>

// for debugging
#include <iostream>

namespace gr { namespace v2 {

Syncer2::Syncer2( http::Agent *http ):
	Syncer( http )
{
	assert( http != 0 ) ;
}

void Syncer2::DeleteRemote( Resource *res )
{
	http::StringResponse str ;
	http::Header hdr ;
	hdr.Add( "If-Match: " + res->ETag() ) ;
	m_http->Post( res->SelfHref() + "/trash", "", &str, hdr ) ;
}

void Syncer2::DeleteRemote( std::string etag, std::string selfHref )
{
	Log( "upload file failed, delete metadata info, DeleteRemote etag= %1%,selfHref=%2%", etag,selfHref,log::info ) ;
	if(selfHref.empty())
		return;
	
	http::StringResponse str ;
	http::Header hdr ;
	//hdr.Add( "If-Match: " + etag ) ;
	m_http->Post( selfHref + "/trash", "", &str, hdr ) ;
}

bool Syncer2::EditContent( Resource *res, bool new_rev )
{
	assert( res->Parent() ) ;
	assert( !res->ResourceID().empty() ) ;
	assert( res->Parent()->GetState() == Resource::sync ) ;

	if ( !res->IsEditable() )
	{
		Log( "Cannot upload %1%: file read-only. %2%", res->Name(), res->StateStr(), log::warning ) ;
		return false ;
	}

	return Upload( res ) ;
}

bool Syncer2::Create( Resource *res )
{
	assert( res->Parent() ) ;
	assert( res->Parent()->IsFolder() ) ;
	assert( res->Parent()->GetState() == Resource::sync ) ;
	assert( res->ResourceID().empty() ) ;
	
	if ( !res->Parent()->IsEditable() )
	{
		Log( "Cannot upload %1%: parent directory read-only. %2%", res->Name(), res->StateStr(), log::warning ) ;
		return false ;
	}
	
	return Upload( res );
}

bool Syncer2::Upload( Resource *res )
{
	File file( res->Path() ) ;
	if(file.Size() > (5 * 1024 * 1024))
		return UploadLargeFile(res);
	else
		return UploadSmallFile(res);
}

Val Syncer2::UploadMetadata( Resource *res )
{
	Val meta;
	meta.Add( "title", Val( res->Name() ) );
	meta.Add( "modifiedDate", Val( res->MTime().ToString()) );
	if ( res->IsFolder() )
	{
		meta.Add( "mimeType", Val( mime_types::folder ) );
	}
	if ( !res->Parent()->IsRoot() )
	{
		Val parent;
		parent.Add( "id", Val( res->Parent()->ResourceID() ) );
		Val parents( Val::array_type );
		parents.Add( parent );
		meta.Add( "parents", parents );
	}
	std::string json_meta = WriteJson( meta );

	Val valr ;
	int max_retry_time = 0;
	while(true){
		// Issue metadata update request
		{
			http::Header hdr2 ;
			hdr2.Add( "Content-Type: application/json" );
			http::ValResponse vrsp ;
			long http_code = 0;
			if ( res->ResourceID().empty() )
				http_code = m_http->Post( feeds::files + "?setModifiedDate=true", json_meta, &vrsp, hdr2 ) ;
			else
				http_code = m_http->Put( feeds::files + "/" + res->ResourceID() + "?setModifiedDate=true", json_meta, &vrsp, hdr2 ) ;

			if ( http_code == 410 || http_code == 412)
			{
				if(max_retry_time <= 5){
					max_retry_time++;
					Log( "Issue metadata update request, request failed with %1%, retrying whole upload in 5s, max_retry_time = %2%", http_code, max_retry_time, log::warning ) ;
					os::Sleep( 5 );
				}else{
					Log( "Issue metadata update request, request failed with %1%, retrying max times exceed to 5 times, ignore it, next.", http_code, log::warning ) ;
					max_retry_time = 0;
					return valr;
				}
			}
			else{
				valr = vrsp.Response();
				assert( !( valr["id"].Str().empty() ) );
				return valr;
			}
		}
	}
}

bool Syncer2::UploadLargeFile( Resource *res )
{
	Log( "UploadLargeFile, path = %1%", res->Path(),log::info ) ;
	Val valr;
	long http_code = 0;

	if ( !res->IsFolder() )
	{
		int max_retry_time = 0;
		bool first_run = true;
		http::ValResponse vrsp;

		Val meta;
		meta.Add( "title", Val( res->Name() ) );
		meta.Add( "modifiedDate", Val( res->MTime().ToString()) );
		if ( res->IsFolder() )
		{
			meta.Add( "mimeType", Val( mime_types::folder ) );
		}
		if ( !res->Parent()->IsRoot() )
		{
			Val parent;
			parent.Add( "id", Val( res->Parent()->ResourceID() ) );
			Val parents( Val::array_type );
			parents.Add( parent );
			meta.Add( "parents", parents );
		}
		std::string json_meta = WriteJson( meta );

		while ( true )
		{
		
			File file( res->Path(), true ) ;
			
			http::Header hdr2 ;
			std::ostringstream xcontent_len2 ;
			xcontent_len2 << "X-Upload-Content-Length: " << file.Size() ;
			hdr2.Add( "Content-Type: application/json" ) ;
			hdr2.Add( "X-Upload-Content-Type: application/octet-stream" ) ;
			hdr2.Add( xcontent_len2.str() ) ;
			if(res->ResourceID().empty())
				http_code = m_http->Post( upload_base + "?uploadType=resumable&setModifiedDate=true", json_meta, &vrsp, hdr2) ;
			else
				http_code = m_http->Put( upload_base + "/" + res->ResourceID() + "?uploadType=resumable&setModifiedDate=true", json_meta, &vrsp, hdr2) ;
				
			if ( http_code == 410 || http_code == 412)
			{
				if(max_retry_time <= 5){
					max_retry_time++;
					Log( "put file request, request failed with %1%, retrying whole upload in 5s, max_retry_time = %2%", http_code, max_retry_time, log::warning ) ;
					os::Sleep( 5 );
				}else{
					Log( "put file request, request failed with %1%, retrying max times exceed to 5 times, ignore it, next.", http_code, log::warning ) ;
					max_retry_time = 0;
					return false;
				}
			}
			else		
			{
				m_uplink = m_http->RedirLocation() ;
			}
			if(m_uplink.empty()) 
				return false;
			
			std::ostringstream xcontent_len ;
			xcontent_len << "Content-Length: " << file.Size() ;

			http::Header hdr ;
			hdr.Add( "Content-Type: application/octet-stream" ) ;
			hdr.Add( xcontent_len.str() ) ;
			if ( valr.Has( "etag" ) )
				hdr.Add( "If-Match: " + valr["etag"].Str() ) ;
			http_code = m_http->Put(m_uplink, &file, &vrsp, hdr, true) ;
			if ( http_code == 410 || http_code == 412)
			{
				if(max_retry_time <= 5){
					max_retry_time++;
					Log( "put file request, request failed with %1%, retrying whole upload in 5s, max_retry_time = %2%", http_code, max_retry_time, log::warning ) ;
					os::Sleep( 5 );
				}else{
					Log( "put file request, request failed with %1%, retrying max times exceed to 5 times, ignore it, next.", http_code, log::warning ) ;
					max_retry_time = 0;
					return false;
				}
			}else if(http_code != 200){
retry:			
				os::Sleep( 5 );
				file.Seek( 0, SEEK_SET, false );
				http::Header hdr ;
				std::ostringstream xcontent_len ;
				xcontent_len << "Content-Range: bytes */" << file.Size() ;
				hdr.Add( xcontent_len.str() ) ;
				Log( "Request the upload status.", log::info ) ;
				http_code = m_http->Put(m_uplink, "", &vrsp, hdr, true) ;
				Log( "upload status request response: %1%.", http_code, log::info ) ;
				if(http_code == 308){//Resume Incomplete
					m_range = m_http->GetRange();//get uploaded data range first.
					//bytes=0-8388607
					Log( "resume upload start. m_range = %1%", m_range, log::info ) ;
					std::string offset;
					int pos = -1;
					off_t uploaded_len = 0;
					pos = m_range.rfind("bytes=0-");
					if(pos >= 0){
						offset = m_range.substr(pos + 8, m_range.length());
						Log( "offset = %1%", offset, log::info ) ;
						uploaded_len = atoll(offset.c_str())+1;
					}else
						return false;
					
					http::Header hdr ;
					std::ostringstream xcontent_len , xcontent_range;
					xcontent_range << "Content-Range: bytes " << uploaded_len << "-" << (file.Size() - 1) << "/" << file.Size();
					hdr.Add( xcontent_range.str() ) ;
					file.Seek( uploaded_len, SEEK_SET, true );
					Log( "Resuming an interrupted upload.",  log::info ) ;
					http_code = m_http->Put(m_uplink, &file, &vrsp, hdr, true) ;
					Log( "interrupted upload response: %1%.", http_code, log::info ) ;
				} 

				if(http_code == 200){//uploaded finished.
					valr = vrsp.Response() ;
					assert( !( valr["id"].Str().empty() ) );
					break ;
				}else{
					Log( "uploaded incomplete, continue. ", log::info ) ;
					goto retry;
				}
			}else		
			{
				valr = vrsp.Response() ;
				assert( !( valr["id"].Str().empty() ) );
				break ;
			}
		}
	}
	Entry2 responseEntry = Entry2( valr ) ;
	AssignIDs( res, responseEntry ) ;
	AssignMTime( res, responseEntry.MTime() ) ;
	return true ;
}

bool Syncer2::UploadSmallFile( Resource *res )
{
	Log( "UploadSmallFile, path = %1%", res->Path(),log::info ) ;
retry:
	Val valr = UploadMetadata(res);
	Log( "UploadSmallFile, UploadMetadata ok = %1%", res->Path(),log::info ) ;
#if 0
	os::Sleep(30);
	Log( "after 30s, delete it= %1%", res->Path(),log::info ) ;
	std::string etag = valr["etag"].Str();
	std::string selfHref = valr["selfLink"].Str();
	DeleteRemote(etag, selfHref);
	Log( "delete ok", log::info ) ;
	return true;
#endif	
	if ( !res->IsFolder() )
	{
		int max_retry_time = 0;
		bool first_run = true;
		http::ValResponse vrsp;
		while ( true )
		{

			File file( res->Path() ) ;
			std::ostringstream xcontent_len ;
			xcontent_len << "Content-Length: " << file.Size() ;

			http::Header hdr ;
			hdr.Add( "Content-Type: application/octet-stream" ) ;
			hdr.Add( xcontent_len.str() ) ;
			if ( valr.Has( "etag" ) )
				hdr.Add( "If-Match: " + valr["etag"].Str() ) ;
#if 0//enable to test
			std::string id = "";
			if(first_run == true){
				id = "dsfdferelrelreklwr";
				first_run = false;
			}else{
				id = valr["id"].Str();
			}
#else	
			std::string id = valr["id"].Str();
			std::string etag = valr["etag"].Str();
			std::string selfHref = valr["selfLink"].Str();
#endif
			long http_code = m_http->Put( upload_base + "/" + id + "?uploadType=media", &file, &vrsp, hdr) ;
			if ( http_code == 410 || http_code == 412)
			{
				if(max_retry_time <= 5){
					max_retry_time++;
					Log( "put file request, request failed with %1%, retrying whole upload in 5s, max_retry_time = %2%", http_code, max_retry_time, log::warning ) ;
					os::Sleep( 5 );
				}else{
					Log( "put file request, request failed with %1%, retrying max times exceed to 5 times, goto retry.", http_code, log::warning ) ;
					max_retry_time = 0;
					DeleteRemote(etag, selfHref);
					goto retry;
				}
			}
			else		
			{
				valr = vrsp.Response() ;
				assert( !( valr["id"].Str().empty() ) );
				break ;
			}
		}
	}

	Entry2 responseEntry = Entry2( valr ) ;
	AssignIDs( res, responseEntry ) ;
	Log( "UploadSmallFile, upload data successfully.", log::info ) ;
	UploadMetadata(res);//update modifiedtime first.
	AssignMTime( res, responseEntry.MTime() ) ;
	return true ;

}

std::auto_ptr<Feed> Syncer2::GetFolders()
{
	return std::auto_ptr<Feed>( new Feed2( feeds::files + "?maxResults=1000&q=%27me%27+in+readers+and+trashed%3dfalse+and+mimeType%3d%27" + mime_types::folder + "%27" ) );
}

std::auto_ptr<Feed> Syncer2::GetAll()
{
	return std::auto_ptr<Feed>( new Feed2( feeds::files + "?maxResults=1000&q=%27me%27+in+readers+and+trashed%3dfalse" ) );
}

std::string ChangesFeed( long changestamp, int maxResults = 1000 )
{
	boost::format feed( feeds::changes + "?maxResults=%1%&includeSubscribed=false" + ( changestamp > 0 ? "&startChangeId=%2%" : "" ) ) ;
	return ( changestamp > 0 ? feed % maxResults % changestamp : feed % maxResults ).str() ;
}

std::auto_ptr<Feed> Syncer2::GetChanges( long min_cstamp )
{
	return std::auto_ptr<Feed>( new Feed2( ChangesFeed( min_cstamp ) ) );
}

long Syncer2::GetChangeStamp( long min_cstamp )
{
	http::ValResponse res ;
	long response = m_http->Get( ChangesFeed( min_cstamp, 1 ), &res, http::Header() ) ;
	if(response == 410 || response == 412){
		Log( "GetChangeStamp error: response = %1%", response, log::info ) ;
		return -1;
	}

	return std::atoi( res.Response()["largestChangeId"].Str().c_str() );
}

void Syncer2::GetAbout()
{
	http::ValResponse res ;
	
	Log( "Getting abouts", log::info ) ;
	long response = m_http->Get( feeds::about, &res, http::Header() ) ;
	if(response == 410 || response == 412){
		Log( "GetAbout error: response = %1%", response, log::info ) ;
		return;
	}

	std::cout<< res.Response()["gd:quotaBytesUsed"].Str().c_str() << std::endl ;
	std::cout<< res.Response()["gd:quotaBytesTotal"].Str().c_str() << std::endl ;
}

} } // end of namespace gr::v1
