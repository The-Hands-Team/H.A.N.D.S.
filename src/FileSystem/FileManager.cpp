
#include <cerrno>
#include <functional>
#include <iostream>
#include "FileManager.hpp"
namespace fs = std::experimental::filesystem;

FileManager* FileManager::currentInstance = nullptr;

FileManager::FileManager()
{
    currentInstance = this;
    
}

FileManager::~FileManager()
{  
}

FileManager* FileManager::getInstance()
{
    if( currentInstance == nullptr )
    {
        currentInstance = new FileManager();
    }
    
    return currentInstance;
}

t_id FileManager::FileManager::deleteFiles( v_paths& files )
{
    std::thread t = std::thread( &static_DeleteFiles, files );
    return t.get_id();
}
t_id FileManager::FileManager::deleteFile( fs::path& file )
{
    v_paths v = v_paths(1, file);
    return deleteFiles( v );        
}

t_id FileManager::copyFiles( v_paths& from
             , v_paths& to
             , fs::copy_options options )
{
    std::thread t = std::thread( &static_CopyFiles, from, to, options );
    return t.get_id();
}
t_id FileManager::copyFiles( v_paths& from
             , fs::path &to
             , fs::copy_options options )
{
    v_paths v = v_paths(1, to);
    return copyFiles( from, v, options );
}
t_id FileManager::copyFile( fs::path &from
             , fs::path &to
             , fs::copy_options options )
{
    v_paths v1 = v_paths( 1, from );
    v_paths v2 = v_paths( 1, to );
    return copyFiles( v1, v2, options );
}

t_id FileManager::moveFiles( v_paths& from
             , v_paths& to
             , fs::copy_options options )
{
    std::thread t = std::thread( &static_MoveFiles, from, to, options );
    return t.get_id();
}
t_id FileManager::moveFiles( v_paths& from
             , fs::path &to
             , fs::copy_options options )
{
    v_paths v = v_paths(1, to);
    return moveFiles( from, v, options );
}
t_id FileManager::moveFile( fs::path &from
             , fs::path &to
             , fs::copy_options options )
{
    v_paths v1 = v_paths( 1, from );
    v_paths v2 = v_paths( 1, to );
    return moveFiles( v1, v2, options );
}

void FileManager::static_DeleteFiles( v_paths files )
{
	getInstance()->doDeleteFiles( files );
}


void FileManager::static_CopyFiles( v_paths from, v_paths to, fs::copy_options options )
{
	getInstance()->doCopyFiles( from, to, options );
}

void FileManager::static_MoveFiles( v_paths from, v_paths to, fs::copy_options options )
{
	getInstance()->doMoveFiles( from, to, options );
}

void FileManager::doDeleteFiles( v_paths files )
{
    std::error_code ec;
    v_paths canonicals;
    canonicals.reserve( files.size() );
    for( v_paths::iterator it = files.begin(); files.end() != it; it++ )
    {
		fs::path can = fs::canonical( *it, ec );
		
		switch( checkError( ec, *it ) )
		{
			case TERMINATE:
				return;
				break;
			case RETRY:
				it--;
			/* break intentionally omitted */
			case IGNORE:
				continue;
				break;
			case NO_ERROR:
				canonicals.emplace_back( can );
				break;
		}
		
	}
		v_paths deleteStack;
	    deleteStack.reserve( canonicals.size() );
	
	while( canonicals.size() > 0 )
	{
		std::unique_lock<std::mutex> lock(setLock);
		notifier.wait( lock, (std::function<bool()>)
			[&]()
			{
				for( v_paths::reverse_iterator it = canonicals.rbegin(); canonicals.rend() != it; it++ )
				{
					if( !lockedPaths.count( *it ) )
					{
						lockedPaths.insert( *it );
						deleteStack.emplace_back( *it );
						canonicals.erase( (it+1).base() );
					}
				}
				return deleteStack.size() == 0 && canonicals.size() > 0;
			}
		);
		lock.unlock();
		for( v_paths::iterator it = deleteStack.begin(); deleteStack.end() != it; it++ )
		{
			fs::remove( *it, ec );
			switch( checkError( ec, *it ) )
			{
				case TERMINATE:
					lock.lock();
					for( /*intentionally ommited*/ ; deleteStack.end() != it; it++ )
					{
						lockedPaths.erase( *it );
					}
					canonicals.clear();
					notifier.notify_all();
					lock.unlock();
					return;
					break;
				case RETRY:
					it--;
				/* break intentionally omitted */
				case IGNORE:
					continue;
					break;
				case NO_ERROR:
					lock.lock();
					lockedPaths.erase( *it );
					notifier.notify_all();
					lock.unlock();
					break;
			}
		}
	}
	
	// TODO notify of success

}

void FileManager::doCopyFiles( v_paths from, v_paths to, fs::copy_options options )
{
    std::error_code ec;
    //fs::copy( from, to, options, ec );
    
    if( !ec )
    {
        //checkError( ec );
    }
}
void FileManager::doMoveFiles( v_paths from, v_paths to, fs::copy_options options )
{
    std::error_code ec;
    //fs::rename( from, to, ec );
    
    if( !ec )
    {
        //checkError( ec );
    }
}


FileManager::HandleErrorCommand FileManager::checkError( std::error_code& ec, fs::path p1, fs::path p2 )
{
    std::cerr << ec.message() << std::endl;
    return IGNORE;
}
