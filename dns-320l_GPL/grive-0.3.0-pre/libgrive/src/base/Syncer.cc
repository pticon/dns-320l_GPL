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

#include "Syncer.hh"
#include "Resource.hh"
#include "Entry.hh"
#include "http/Agent.hh"
#include "http/Header.hh"
#include "http/Download.hh"
#include "util/OS.hh"
#include "util/log/Log.hh"
#include "util/Crypt.hh"
#include "CommonUri.hh"

namespace gr {

Syncer::Syncer( http::Agent *http ):
	m_http( http )
{
}

http::Agent* Syncer::Agent() const
{
	return m_http;
}

void Syncer::Download( Resource *res, const fs::path& file )
{
#if 1
	http::Download dl( file.string(), http::Download::NoChecksum() ) ;
	long response = 0;
	bool hasSameCheckSum = false;
	File downloadFile = File(file);
	while( true ){
		http::Header hdr ;
		std::ostringstream range ;
//		File downloadFile = File(file);
		Log( "downloadFile.Size() = %1%, total size = %2%", downloadFile.Size(), res->ContentSize(), log::warning ) ;
		if(downloadFile.Size() < res->ContentSize()){
			range << "Range: bytes=" << downloadFile.Size() << "-" << (res->ContentSize() - 1);
			hdr.Add( range.str() ) ;
		}else{
			break;
		}
		std::string url = v2::feeds::files + "/" + res->ResourceID() + "?alt=media";
		response = m_http->Get( url, &dl, hdr, true ) ;
		if(response <= 400 && downloadFile.Size() == res->ContentSize())
			break;
		else{
			Log( "get file request, request failed, retry it.", log::warning ) ;
			os::Sleep( 5 );
		}
	}

	std::string md5 = crypt::MD5::Get( file ) ;
	if(res->MD5() == md5 || md5 == "")
		hasSameCheckSum = true;
	Log( "Download, md5 = %1%, res->MD5() = %2%, hasSameCheckSum = %3%, response = %4%", md5,res->MD5(),hasSameCheckSum, response, log::info ) ;

	downloadFile.Chmod(0777);

	if ( response <= 400 && hasSameCheckSum == true)
	{
		Log( "res->MTime() = %1%.", res->MTime(), log::warning ) ;
		if ( res->MTime() != DateTime() )
			os::SetFileTime( file, res->MTime() ) ;
		else
			Log( "encountered zero date time after downloading %1%", file, log::warning ) ;
	}
#else
	http::Download dl( file.string(), http::Download::NoChecksum() ) ;
	long r = m_http->Get( res->ContentSrc(), &dl, http::Header() ) ;
	if ( r <= 400 )
	{
		if ( res->MTime() != DateTime() )
			os::SetFileTime( file, res->MTime() ) ;
		else
			Log( "encountered zero date time after downloading %1%", file, log::warning ) ;
	}
#endif
}

void Syncer::AssignIDs( Resource *res, const Entry& remote )
{
	res->AssignIDs( remote );
}

void Syncer::AssignMTime( Resource *res, const DateTime& mtime )
{
	res->m_mtime = mtime;
}

} // end of namespace gr
