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

#include "Resource.hh"
#include "Entry.hh"
#include "Syncer.hh"

#include "json/Val.hh"
#include "util/CArray.hh"
#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "util/File.hh"

#include <boost/bind.hpp>

#include <cassert>

// for debugging
#include <iostream>

#include <fnmatch.h>


std::vector<std::string> exclude_file;
std::string path_to_sync_dir;
bool use_include;


namespace gr {

/// default constructor creates the root folder
Resource::Resource( const fs::path& root_folder ) :
	m_name		( root_folder.string() ),
	m_kind		( "folder" ),
	m_id		( "folder:root" ),
	m_href		( "root" ),
	m_parent	( 0 ),
	m_state		( sync ),
	m_is_editable( true ),
	m_addFrom	( 0 ),
	m_dir_mtime (0)
{
}

Resource::Resource( const std::string& name, const std::string& kind ) :
	m_name		( name ),
	m_kind		( kind ),
	m_parent	( 0 ),
	m_state		( unknown ),
	m_is_editable( true ),
	m_addFrom	( 0 ),
	m_dir_mtime (0)

{
}

void Resource::SetState( State new_state )
{
	// only the new and delete states need to be set recursively
	assert(
		new_state == remote_new || new_state == remote_deleted ||
		new_state == local_new  || new_state == local_deleted
	) ;
	
	m_state = new_state ;
	std::for_each( m_child.begin(), m_child.end(),
		boost::bind( &Resource::SetState, _1, new_state ) ) ;
}

void Resource::FromRemoteFolder( const Entry& remote, const DateTime& last_sync )
{
	fs::path path = Path() ;
	if ( !remote.IsEditable() )
		Log( "folder %1% is read-only", path, log::verbose ) ;
    
	//Log( "remote.MTime() = %1%, last_sync = %2%, path = %3%", remote.MTime(), last_sync, path, log::info ) ;

	// already sync
	if ( fs::is_directory( path ) )
	{
		Log( "folder %1% is in sync", path, log::verbose ) ;
		m_state = sync ;
	}
	
	// remote file created after last sync, so remote is newer
	else if ( remote.MTime() > last_sync )
	{
		if ( fs::exists( path ) )
		{
			// TODO: handle type change
			Log( "%1% changed from folder to file", path, log::verbose ) ;
			m_state = sync ;
		}
		else
		{
			// make all children as remote_new, if any
			Log( "folder %1% is created in remote", path, log::verbose ) ;
			SetState( remote_new ) ;
		}
	}
	else
	{
		if ( fs::exists( path ) )
		{
			// TODO: handle type chage
			Log( "%1% changed from file to folder", path, log::verbose ) ;
			m_state = sync ;
		}
		else
		{
		
			Log( "folder %1% is deleted in local", path, log::verbose ) ;
			SetState( local_deleted ) ;
		}
	}
}

/// Update the state according to information (i.e. Entry) from remote. This function
/// compares the modification time and checksum of both copies and determine which
/// one is newer.
void Resource::FromRemote( const Entry& remote, const DateTime& last_sync )
{
	// sync folder
	if ( remote.IsDir() && IsFolder() )
		FromRemoteFolder( remote, last_sync ) ;
	else
		FromRemoteFile( remote, last_sync ) ;
	
	AssignIDs( remote ) ;
	Log( "FromRemote, remote time = %1%, m_state = %2%", remote.MTime(),m_state, log::info ) ;
	
	assert( m_state != unknown ) ;
	
//	if ( m_state == remote_new || m_state == remote_changed || m_state == local_deleted)
	{
		m_md5	= remote.MD5() ;
		m_mtime	= remote.MTime() ;
	}
}

void Resource::AssignIDs( const Entry& remote )
{
	// the IDs from change feed entries are different
	if ( !remote.IsChange() )
	{
		m_id		= remote.ResourceID() ;
		m_href		= remote.SelfHref() ;
		m_content	= remote.ContentSrc() ;
		m_is_editable = remote.IsEditable() ;
		m_etag		= remote.ETag() ;
		m_size		= remote.Size();
	}
}

void Resource::FromRemoteFile( const Entry& remote, const DateTime& last_sync )
{
	assert( m_parent != 0 ) ;
	
	fs::path path = Path() ;
	//Log( "FromRemoteFile, last_sync = %1%, remote.IsDir() = %2%, path = %3%", last_sync, remote.IsDir(), path, log::info ) ;
	Log( "FromRemoteFile, remote.MD5() = %1%, m_md5 = %2%, path = %3%", remote.MD5(),m_md5,path, log::info ) ;

	// recursively create/delete folder
	if ( m_parent->m_state == remote_new || m_parent->m_state == remote_deleted ||
		 m_parent->m_state == local_new  || m_parent->m_state == local_deleted )
	{
		Log( "file %1% parent %2% recursively in %3% (%4%)", path,
			( m_parent->m_state == remote_new || m_parent->m_state == local_new )      ? "created" : "deleted",
			( m_parent->m_state == remote_new || m_parent->m_state == remote_deleted ) ? "remote"  : "local",
			m_parent->m_state, log::verbose ) ;
		
		m_state = m_parent->m_state ;
	}

	// local not exists
	else if ( !fs::exists( path ) )
	{
		Trace( "file %1% change stamp = %2%", Path(), remote.ChangeStamp() ) ;
		if ( remote.MTime() > last_sync || remote.ChangeStamp() > 0 )
		{
			Log( "file %1% is created in remote (change %2%)", path,
				remote.ChangeStamp(), log::verbose ) ;
			
			m_state = remote_new ;
		}
		else
		{
			Log( "file %1% is deleted in local", path, log::verbose ) ;
			m_state = local_deleted ;
		}
	}
	
	// remote checksum unknown, assume the file is not changed in remote
	else if ( remote.MD5().empty() )
	{
		Log( "file %1% has unknown checksum in remote. assuned in sync",
			Path(), log::verbose ) ;
		m_state = sync ;
	}
	
	// if checksum is equal, no need to compare the mtime
	else if ( remote.MD5() == m_md5 )
	{
		Log( "file %1% is already in sync", Path(), log::verbose ) ;
		m_state = sync ;
	}

	// use mtime to check which one is more recent
	else
	{
		assert( m_state != unknown ) ;

		// if remote is modified
		Log( "path = %1%, remote.MTime() %2%, m_mtime = %3%", path, remote.MTime(), m_mtime, log::info ) ;
		if ( remote.MTime() > m_mtime )
		{
			Log( "file %1% is changed in remote", path, log::verbose ) ;
			m_state = remote_changed ;
		}
		
		// remote also has the file, so it's not new in local
		else if ( m_state == local_new || m_state == remote_deleted )
		{
			if(remote.MTime().ToString() != m_mtime.ToString()){
				Log( "file %1% is changed in local, m_state = %2%", path, m_state, log::info ) ;
				m_state = local_changed ;
			}else{
				m_state = sync;
				Log( "file %1% state is %2%", m_name, m_state, log::info ) ;
			}
		}
		else
			Log( "file %1% state is %2%", m_name, m_state, log::info ) ;
	}
}

/// Update the resource with the attributes of local file or directory. This
/// function will propulate the fields in m_entry.
void Resource::FromLocal( const DateTime& last_sync )
{
	fs::path path = Path() ;
	//assert( fs::exists( path ) ) ;
        
	// root folder is always in sync
	if ( !IsRoot() )
	{
		m_mtime = os::FileCTime( path ) ;
		// follow parent recursively
		if ( m_parent->m_state == local_new || m_parent->m_state == local_deleted ){
			m_state = local_new ;
            		//Log( "Resource: FromLocal1, path = %1%, m_state = %2%, m_mtime = %3%, last_sync = %4%", path, m_state, m_mtime.ToString(), last_sync.ToString(), log::info ) ;
		}
		
		// if the file is not created after last sync, assume file is
		// remote_deleted first, it will be updated to sync/remote_changed
		// in FromRemote()
		else{
			m_state =  m_mtime > last_sync ? local_new : remote_deleted  ;
			if(m_state == remote_deleted){
				if(m_sync == SYNC_DIRECTION_DOWN && getHiddenFileExt(path) == ""){
					m_state = local_new;
				}else if(m_sync == SYNC_DIRECTION_TWO 
					&& (getHiddenFileExt(path) == "" 
						|| getHiddenFileExt(path) == FILE_TYPE_LOCAL_CREATE)){
					m_state = local_new;
				}
			}
            		Log( "Resource: FromLocal2, path = %1%, m_state = %2%, m_mtime = %3%, last_sync = %4%", path, m_state, m_mtime.ToString(), last_sync.ToString(), log::info ) ;
		}
		
		m_name		= path.filename().string() ;
		m_kind		= IsFolder() ? "folder" : "file" ;
		m_md5		= IsFolder() ? ""		: crypt::MD5::Get( path ) ;
	}
	
	assert( m_state != unknown ) ;
}

std::string Resource::SelfHref() const
{
	return m_href ;
}

std::string Resource::ContentSrc() const
{
	return m_content ;
}

u64_t Resource::ContentSize() const
{
	return m_size ;
}

std::string Resource::ETag() const
{
	return m_etag ;
}

std::string Resource::Name() const
{
	return m_name ;
}

std::string Resource::Kind() const
{
	return m_kind ;
}

DateTime Resource::MTime() const
{
	return m_mtime ;
}

std::string Resource::ResourceID() const
{
	return m_id ;
}

Resource::State Resource::GetState() const
{
	return m_state ;
}

const Resource* Resource::Parent() const
{
	assert( m_parent == 0 || m_parent->IsFolder() ) ;
	return m_parent ;
}

Resource* Resource::Parent()
{
	assert( m_parent == 0 || m_parent->IsFolder() ) ;
	return m_parent ;
}

void Resource::AddChild( Resource *child )
{
	assert( child != 0 ) ;
	assert( child->m_parent == 0 || child->m_parent == this ) ;
	assert( child != this ) ;

	child->m_parent = this ;
	m_child.push_back( child ) ;
}

bool Resource::IsFolder() const
{
	return m_kind == "folder" ;
}

bool Resource::IsEditable() const
{
	return m_is_editable ;
}

fs::path Resource::Path() const
{
	assert( m_parent != this ) ;
	assert( m_parent == 0 || m_parent->IsFolder() ) ;

	return m_parent != 0 ? (m_parent->Path() / m_name) : m_name ;
}

bool Resource::IsInRootTree() const
{
	assert( m_parent == 0 || m_parent->IsFolder() ) ;
	return m_parent == 0 ? IsRoot() : m_parent->IsInRootTree() ;
}

Resource* Resource::FindChild( const std::string& name )
{
	for ( std::vector<Resource*>::iterator i = m_child.begin() ; i != m_child.end() ; ++i )
	{
		assert( (*i)->m_parent == this ) ;
		if ( (*i)->m_name == name )
			return *i ;
	}
	return 0 ;
}


bool PartialMatchTest(std::string pattern,std::string path){
    
    int full_match = fnmatch(pattern.c_str(),path.c_str(),FNM_FILE_NAME|FNM_NOESCAPE);
    
    if(full_match == 0){
        return true;// full match by patterm
    }
    else{
        int path_len=path.size();
        int patt_len=pattern.size();
        int len=path_len+patt_len;
        
        for(int i=0;i<len;i++){ 
            
            if(path[i] == pattern[i]){
                path_len--;
                patt_len--;
                
                if(path_len <= 0){// the path completely contains in a pattern as initial substring
                    return true;
                }
                
                if(patt_len <= 0){
                    return false;
                }
            }
            else{
                return false;
            }
        }
        
    }
    return false;
}



bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


// try to change the state to "sync"
void Resource::Sync( Syncer *syncer, DateTime& sync_time, const Val& options )
{
	assert( m_state != unknown ) ;
	assert( !IsRoot() || m_state == sync ) ;	// root folder is already synced
        
        
// ==== EXCLUDING FROM SYNC =====    
        
        // restore full canonical path
        std::string fp=this->Name();
        Resource* current=this;
        while((current=current->m_parent)!=0){           
            fp=current->Name()+"/"+fp; 
        }
        
        replace(fp,path_to_sync_dir,"");
        
        if(fp!=""){
            
          // std::cout<< "\n";
            bool match=false;
             
            for( std::vector<std::string>::iterator i=exclude_file.begin();i!=exclude_file.end();i++){
                
                
                
                if( !use_include ){
                    int res=fnmatch((*i).c_str(),fp.c_str(),FNM_FILE_NAME|FNM_NOESCAPE|FNM_LEADING_DIR|FNM_PERIOD);

                        if( (res)!=FNM_NOMATCH  ){
                            return;
                        }
                }
                else{
                   // int res=fnmatch((*i).c_str(),fp.c_str(),FNM_FILE_NAME|FNM_NOESCAPE|FNM_LEADING_DIR|FNM_PERIOD);
                    bool res=PartialMatchTest(*i,fp);
                    char tt=fp[10000];
                    if(  (res)){//!=FNM_NOMATCH 
                        match=true;
//                        std::cout<< *i+" == "+fp;
//                        std::cout<<"   MATCHED!!!!!\n";
                    }
                    else{
//                        std::cout<< *i+" == "+fp;
//                        std::cout<<"   NOT MATCHED!!!!!\n";
                    }
                }

//                else{
//                    if( (res)!=FNM_NOMATCH ){
//                        return;
//                    }                    
//                }
            }
            
            if((!match)&&(use_include)){
                 return;
            }
            
        }
        
//===============================        

	SyncSelf( syncer, options ) ;
	
	// we want the server sync time, so we will take the server time of the last file uploaded to store as the sync time
	// m_mtime is updated to server modified time when the file is uploaded
	sync_time = std::max(sync_time, m_mtime);
    	sync_time = std::max(sync_time, m_dir_mtime);
	// if myself is deleted, no need to do the childrens
	if ( m_state != local_deleted && m_state != remote_deleted )
		std::for_each( m_child.begin(), m_child.end(),
			boost::bind( &Resource::Sync, _1, syncer, boost::ref(sync_time), options ) ) ;
}

void Resource::SyncSelf( Syncer* syncer, const Val& options )
{
	assert( !IsRoot() || m_state == sync ) ;	// root is always sync
	assert( IsRoot() || !syncer || m_parent->IsFolder() ) ;
	assert( IsRoot() || m_parent->m_state != remote_deleted ) ;
	assert( IsRoot() || m_parent->m_state != local_deleted ) ;

	const fs::path path = Path() ;

	switch ( m_state )
	{
	case local_new :
        
    		if ( m_sync == SYNC_DIRECTION_DOWN)
		{
			Log( "%1% is local file (%2%). Don't upload in force downloading mode", path, m_state, log::info ) ;
			createHIddenFile(path, true);
			m_state = sync ;
			break;
		}

		Log( "sync %1% doesn't exist in server, uploading", path, log::info ) ;
		
		// FIXME: (?) do not write new timestamp on failed upload
		if ( syncer && syncer->Create( this ) ){
			createHIddenFile(path, false, FILE_ADD_FROM_LOCAL);
			m_state = sync ;
		}
		break ;
	
	case local_deleted :
		if ( m_sync == SYNC_DIRECTION_DOWN)
		{
			Log( "%1% was removed (%2%). Download remote files in force downloading mode", path, m_state, log::info ) ;
			if ( syncer )
			{
				if ( IsFolder() ){
					fs::create_directories( path ) ;
					m_dir_mtime = os::FileCTime( path ) ;
					createHIddenFile(path, false);
					fs::permissions(path, fs::all_all);
				}else{
        				if ( syncer ){
						syncer->Download(this, path);
						createHIddenFile(path, false);
                                     }
                              }
				
				m_state = sync ;
			}
			break;
		}

               if(getHiddenFileExt(path) == FILE_TYPE_REMOTE_CREATE){//remote create
                    if(m_sync != SYNC_DIRECTION_TWO){
                        Log( "path = %1% has hidden flag(%2%), is remote file, don't delete it.", path, m_state, log::info ) ;
                        break;
                    }
                }else if((getHiddenFileExt(path) == FILE_TYPE_UPLOAD)){
                    if(m_sync != SYNC_DIRECTION_TWO){
                        Log( "path = %1% has hidden flag(%2%), is local file, delete it.", path, m_state, log::info ) ;
                    }
                }

        
		Log( "sync %1% deleted in local(%2%). deleting remote", path, m_state, log::info ) ;
		if ( syncer ){
			syncer->DeleteRemote( this ) ;
			DeleteHiddenFiles(path);
               }
		break ;
	
	case local_changed :
		if ( m_sync == SYNC_DIRECTION_DOWN)
		{
			Log( "%1% was modified (%2%). Download remote files to overwirte in force downloading mode", path, m_state, log::info ) ;
			if ( syncer )
			{
				if ( IsFolder() ){
					fs::create_directories( path ) ;
					fs::permissions(path, fs::all_all);
					m_dir_mtime = os::FileCTime( path ) ;
    					createHIddenFile(path, false);
				}else{
        				if ( syncer ){
                                     	syncer->Download(this, path);
                                     	createHIddenFile(path, false);
                                     }
                              }
				
				m_state = sync ;
			}
			break;
		}
        
		Log( "sync %1% changed in local. uploading", path, log::info ) ;
		if ( syncer && syncer->EditContent( this, options["new-rev"].Bool() ) ){
		        createHIddenFile(path, false, FILE_ADD_FROM_LOCAL);
			m_state = sync ;
		}
		break ;
	
	case remote_new :
        
		if ( m_sync == SYNC_DIRECTION_UP)
		{
		        if(getHiddenFileExt(path) == FILE_TYPE_UPLOAD){
                    		if ( syncer ){
                    			syncer->DeleteRemote( this ) ;
                    			DeleteHiddenFiles(path);
                               }
                        }else if(getHiddenFileExt(path) == ""){
            			Log( "%1% is remote file (%2%). Don't download in force uploading mode, getHiddenFileExt(path) = %3%", path, m_state, getHiddenFileExt(path), log::info ) ;
            			createHIddenFile(path, true);
            			m_state = sync ;
                        }
      			  break;
		}
        
		Log( "sync %1% created in remote(%2%). creating local", path, m_state, log::info ) ;
		if ( syncer )
		{
			if ( IsFolder() ){
				fs::create_directories( path ) ;
				m_dir_mtime = os::FileCTime( path ) ;
				createHIddenFile(path, false, FILE_ADD_FROM_REMOTE);
				fs::permissions(path, fs::all_all);
			}else{
				syncer->Download(this, path);
				createHIddenFile(path, false, FILE_ADD_FROM_REMOTE);
			}
			
			m_state = sync ;
		}
		break ;
	
	case remote_changed :
		assert( !IsFolder() ) ;
		if ( m_sync == SYNC_DIRECTION_UP)
		{
			Log( "%1% was modified (%2%). Upload local files to overwirte in force uploading mode", path, m_state, log::info ) ;
            		if ( syncer && syncer->EditContent( this, options["new-rev"].Bool() ) ){
              			createHIddenFile(path, false);
        			m_state = sync ;
        		}
			break;
		}
        
		Log( "sync %1% changed in remote. downloading", path, log::info ) ;
		if ( syncer )
		{
			syncer->Download( this, path ) ;
			createHIddenFile(path, false, FILE_ADD_FROM_REMOTE);
			m_state = sync ;
		}
		break ;
	
	case remote_deleted :
		if ( m_sync == SYNC_DIRECTION_UP)
		{
			Log( "%1% was removed (%2%). Upload local files  in force uploading mode", path, m_state, log::info ) ;
        		if ( syncer && syncer->Create( this ) ){
        			createHIddenFile(path, false);
        			m_state = sync ;
        		}
			break;
		}
        
                if(getHiddenFileExt(path) == FILE_TYPE_LOCAL_CREATE){//local create
                    if(m_sync != SYNC_DIRECTION_TWO){
                        Log( "path = %1% has hidden flag(%2%), is local file, don't delete it.", path, m_state, log::info ) ;
                        break;
                    }
                }else if((getHiddenFileExt(path) == FILE_TYPE_DOWNLOAD)){
                    if(m_sync != SYNC_DIRECTION_TWO){
                        Log( "path = %1% has hidden flag(%2%), is remote file, delete it.", path, m_state, log::info ) ;
                    }
                }

		Log( "sync %1% deleted in remote. deleting local", path, log::info ) ;
		if ( syncer ){
			DeleteLocal() ;
    			DeleteHiddenFiles(path);
               }
		break ;
	
	case sync :
		Log( "sync %1% already in sync", path, log::verbose ) ;
		break ;

	// shouldn't go here
	case unknown :
		assert( false ) ;
		break ;
		
	default :
		break ;
	}
}

/// this function doesn't really remove the local file. it renames it.
void Resource::DeleteLocal()
{
	static const boost::format trash_file( "%1%-%2%" ) ;

	assert( m_parent != 0 ) ;
	fs::path parent = m_parent->Path() ;
	fs::path dest	= ".trash" / parent / Name() ;
	
	std::size_t idx = 1 ;
	while ( fs::exists( dest ) && idx != 0 )
		dest = ".trash" / parent / (boost::format(trash_file) % Name() % idx++).str() ;
	
	// wrap around! just remove the file
	if ( idx == 0 )
		fs::remove_all( Path() ) ;
	else
	{
		fs::create_directories( dest.parent_path() ) ;
		m_dir_mtime = os::FileCTime( dest.parent_path() ) ;
		createHIddenFile(dest.parent_path(), false);
		fs::rename( Path(), dest ) ;
	}
}

Resource::iterator Resource::begin() const
{
	return m_child.begin() ;
}

Resource::iterator Resource::end() const
{
	return m_child.end() ;
}

std::size_t Resource::size() const
{
	return m_child.size() ;
}

std::ostream& operator<<( std::ostream& os, Resource::State s )
{
	static const char *state[] =
	{
		"sync",	"local_new", "local_changed", "local_deleted", "remote_new",
		"remote_changed", "remote_deleted"
	} ;
	assert( s >= 0 && s < Count(state) ) ;
	return os << state[s] ;
}

std::string Resource::StateStr() const
{
	std::ostringstream ss ;
	ss << m_state ;
	return ss.str() ;
}

std::string Resource::MD5() const
{
	return m_md5 ;
}

bool Resource::IsRoot() const
{
	// Root entry does not show up in file feeds, so we check for empty parent (and self-href)
	return m_parent == 0 ;
}

bool Resource::HasID() const
{
	return !m_href.empty() && !m_id.empty() ;
}

void Resource::createHIddenFile(fs::path path, bool isNew) 
{
	createHIddenFile(path, isNew, -1);
}

void Resource::createHIddenFile(fs::path path, bool isNew, int twoWayDirection) 
{
    if(path.empty() == true) return ;
    
    std::string ext = "";
    {
        if(m_sync == SYNC_DIRECTION_DOWN){
            if(isNew == false)
                ext = FILE_TYPE_DOWNLOAD;//download
            else
                ext = FILE_TYPE_LOCAL_CREATE;//local create
        }else if(m_sync == SYNC_DIRECTION_UP){
            if(isNew == false)
                ext = FILE_TYPE_UPLOAD;//upload
            else
                ext = FILE_TYPE_REMOTE_CREATE;//remote create.
        }else if(twoWayDirection != -1 && m_sync == SYNC_DIRECTION_TWO){
        	if(twoWayDirection == FILE_ADD_FROM_REMOTE)//download
        		ext = FILE_TYPE_DOWNLOAD;
		else if(twoWayDirection == FILE_ADD_FROM_LOCAL)//upload
			ext = FILE_TYPE_UPLOAD;
        }
    }
    
    DeleteHiddenFiles(path);
    
    //Log( "path.parent_path() = %1%, path.filename() = %2%", path.parent_path(), path.filename(), log::info ) ;
    std::string hidden = BuildHiddenFilePath(path, ext);
    std::string cmd = "touch '" + hidden + "'";
    //Log( "cmd = %1%, cmd = %2%", hidden, cmd, log::info ) ;
    system(cmd.c_str());
    cmd = "chmod 777 '" + hidden + "'";
    //Log( "cmd = %1%, cmd = %2%", hidden, cmd, log::info ) ;
    system(cmd.c_str());
}

std::string Resource::getHiddenFileExt(fs::path path) 
{
    if(path.empty() == true) return "";

    std::string ext = FILE_TYPE_LOCAL_CREATE;
    std::string hidden = BuildHiddenFilePath(path, ext);

    if(fs::exists(hidden)){
        return FILE_TYPE_LOCAL_CREATE;
    }
    
    ext = FILE_TYPE_REMOTE_CREATE;
    hidden = BuildHiddenFilePath(path, ext);
    if(fs::exists(hidden)){
        return FILE_TYPE_REMOTE_CREATE;
    }

    ext = FILE_TYPE_DOWNLOAD;
    hidden = BuildHiddenFilePath(path, ext);
    if(fs::exists(hidden)){
        return FILE_TYPE_DOWNLOAD;
    }

    ext = FILE_TYPE_UPLOAD;
    hidden = BuildHiddenFilePath(path, ext);
    if(fs::exists(hidden)){
        return FILE_TYPE_UPLOAD;
    }

    return "";

}

void Resource::DeleteHiddenFiles(fs::path path) {

    if(path.empty() == true || path == ".") return ;
    
    std::string ext = FILE_TYPE_DOWNLOAD;
    std::string ext2 = FILE_TYPE_UPLOAD;
    std::string ext3 = FILE_TYPE_LOCAL_CREATE;
    std::string ext4 = FILE_TYPE_REMOTE_CREATE;
    std::string hidden= BuildHiddenFilePath(path, ext);
    std::string cmd = "rm -rf '" + hidden + "'";
    system(cmd.c_str());

    hidden = BuildHiddenFilePath(path, ext2);
    cmd = "rm -rf '" + hidden + "'";
    system(cmd.c_str());

    hidden = BuildHiddenFilePath(path, ext3);
    cmd = "rm -rf '" + hidden + "'";
    system(cmd.c_str());

    hidden = BuildHiddenFilePath(path, ext4);
    cmd = "rm -rf '" + hidden + "'";
    system(cmd.c_str());

}

std::string Resource::BuildHiddenFilePath(fs::path path, std::string ext){
    return path.parent_path().string() + "/." + path.filename().string() + ext;
}

} // end of namespace
