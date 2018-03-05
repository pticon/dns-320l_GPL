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

#include "Entry.hh"
#include "http/Agent.hh"

#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <algorithm>
#include <iterator>

namespace gr {

/// construct an entry for the root folder
Entry::Entry( ) :
	m_title			( "." ),
	m_is_dir		( true ),
	m_resource_id	( "folder:root" ),
	m_change_stamp	( -1 ),
	m_is_removed	( false )
{
}

const std::vector<std::string>& Entry::ParentHrefs() const
{
	return m_parent_hrefs ;
}

std::string Entry::Title() const
{
	return m_title ;
}

std::string Entry::Filename() const
{
	return m_filename ;
}

bool Entry::IsDir() const
{
	return m_is_dir ;
}

std::string Entry::MD5() const
{
	return m_md5 ;
}

DateTime Entry::MTime() const
{
	return m_mtime ;
}

std::string Entry::SelfHref() const
{
	return m_self_href ;
}

std::string Entry::ParentHref() const
{
	return m_parent_hrefs.empty() ? "" : m_parent_hrefs.front() ;
}

std::string Entry::ResourceID() const
{
	return m_resource_id ;
}

std::string Entry::ETag() const
{
	return m_etag ;
}

std::string Entry::ContentSrc() const
{
	return m_content_src ;
}

 u64_t Entry::Size() const
{
	return m_size ;
}

bool Entry::IsEditable() const
{
	return m_is_editable ;
}

long Entry::ChangeStamp() const
{
	return m_change_stamp ;
}

bool Entry::IsChange() const
{
	return m_change_stamp != -1 ;
}

bool Entry::IsRemoved() const
{
	return m_is_removed ;
}

std::string Entry::Name() const
{
	return !m_filename.empty() ? m_filename : m_title ;
}

std::string Entry::ToString(http::Agent *http) const
{
	Log( "#################start##################", log::info ) ;
	Log( "name = %1%, m_is_dir = %2%, m_md5 = %3%, m_mtime = %4%", Name(), m_is_dir, m_md5, m_mtime, log::info ) ;
	Log( "m_self_href = %1%, ParentHref = %2%, m_resource_id = %3%, m_etag = %4%", m_self_href, ParentHref(), m_resource_id, m_etag, log::info ) ;
	Log( "m_content_src = %1%, m_is_editable = %2%, m_change_stamp = %3%, m_is_removed = %4%", m_content_src, m_is_editable, m_change_stamp, m_is_removed, log::info ) ;
	Log( "#################end##################", log::info ) ;
	return "";
}

} // end of namespace gr
