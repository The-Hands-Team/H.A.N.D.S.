
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

t_id FileManager::FileManager::deleteFiles( v_paths& files, fs::copy_options options )
{
    std::thread t = std::thread( &static_DeleteFiles, files, options );
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

void FileManager::static_DeleteFiles( v_paths files, fs::copy_options options )
{
    getInstance()->doDeleteFiles( files, options );
}


void FileManager::static_CopyFiles( v_paths from, v_paths to, fs::copy_options options )
{
    getInstance()->doCopyFiles( from, to, options );
}

void FileManager::static_MoveFiles( v_paths from, v_paths to, fs::copy_options options )
{
    getInstance()->doMoveFiles( from, to, options );
}

void FileManager::doDeleteFiles( v_paths files, fs::copy_options options )
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
            case RETRY:
                it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                break;
        }

        if( fs::is_symlink( can ) )
        {
            if( fs::copy_options::skip_symlinks & options )
            {
                continue;
            }
            else if( fs::copy_options::copy_symlinks & options )
            {

            }
            else
            {
                can = read_symlink( can, ec );
            }
        }

        switch( checkError( ec, *it ) )
        {
            case TERMINATE:
                return;
            case RETRY:
                it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                canonicals.emplace_back( can );
                break;
        }

    }

    for( v_paths::iterator it = canonicals.begin(); canonicals.end() != it; it++ )
    {
        setLock.lock();
        if( lockedPaths.count( *it ) )
        {
            setLock.unlock();
            ec.assign( EAGAIN, std::generic_category());
            switch( checkError( ec, *it ) )
            {
            case RETRY:
                it--;
                /* break intentionally omitted */
            case IGNORE:
                continue;
            case TERMINATE:
            default:
                return;
            }
        }
        else
        {
            lockedPaths.insert( *it );
        }

        setLock.unlock();
        if( fs::copy_options::recursive & options )
        {
            fs::remove_all( *it, ec );
        }
        else
        {
            fs::remove( *it, ec );
        }

        setLock.lock();
        lockedPaths.erase( *it );
        setLock.unlock();
        switch( checkError( ec, *it ) )
        {
            case TERMINATE:
                return;
            case RETRY:
                it--;
            /* break intentionally omitted */
            case IGNORE:
            case NO_ERROR:
            default:
                break;
        }
    }
}
#define EDOM 33 // TODO remove this line, eclipse is just dumb
void FileManager::doCopyFiles( v_paths from, v_paths to, fs::copy_options options )
{
    std::error_code ec;
    bool to_dir = 1 == to.size() && 1 < from.size();
    
    if( from.size() != to.size() &&
        ( to.size() != 1 ||
          !fs::is_directory( to[0], ec ) ) )
    {
        if( !ec )
        {
            ec.assign( EDOM, std::generic_category() );
            checkError( ec );
            return;
        }
        else if( RETRY == checkError( ec, to[0]) )
        {
            doCopyFiles( from, to, options );
        }
        return;
    }
    v_paths canonical_from;
    canonical_from.reserve( from.size() );
    for( v_paths::iterator it = from.begin(); from.end() != it; it++ )
    {
        fs::path can = fs::canonical( *it, ec );


        switch( checkError( ec, *it ) )
        {
            case TERMINATE:
                return;
            case RETRY:
                it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                canonical_from.emplace_back( can );
                break;
        }

    }

    v_paths canonical_to;
    canonical_to.reserve( to.size() );
    for( v_paths::iterator it = to.begin(); to.end() != it; it++ )
    {
        fs::path can = fs::canonical( *it, ec );

        HandleErrorCommand error = checkError( ec, *it );

        if( NO_ERROR == error && !to_dir && fs::is_directory( can, ec ) )
        {
            ec.assign( EISDIR, std::generic_category() );
        }


        switch( checkError( ec, *it ) )
        {
            case TERMINATE:
                return;
            case RETRY:
                it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                canonical_to.emplace_back( can );
                break;
        }

    }

    while( to_dir )
    {
        fs::path to = canonical_to[0];
        setLock.lock();
        if( lockedPaths.count( to ) )
        {
            setLock.unlock();
            ec.assign( EAGAIN, std::generic_category());
            switch( checkError( ec, to ) )
            {
            case RETRY:
                continue;
            case TERMINATE:
            default:
                return;
            }
        }
        else
        {
            lockedPaths.insert( to );
            setLock.unlock();
            break;
        }
    }

    for( v_paths::iterator from_it = canonical_from.begin(), to_it = canonical_to.begin(); canonical_from.end() != from_it; from_it++ )
    {
        setLock.lock();
        if( lockedPaths.count( *from_it ) )
        {
            setLock.unlock();
            ec.assign( EAGAIN, std::generic_category());
            switch( checkError( ec, *from_it ) )
            {
            case RETRY:
                from_it--;
                continue;
            case IGNORE:
                if( !to_dir )
                {
                    to_it++;
                }
                continue;
            case TERMINATE:
            default:
                return;
            }
        }
        else
        {
            lockedPaths.insert( *from_it );
        }

        if( !to_dir )
        {
            if (lockedPaths.count( *to_it ) )
            {
                setLock.unlock();
                ec.assign( EAGAIN, std::generic_category());
                switch( checkError( ec, *to_it ) )
                {
                case RETRY:
                    to_it--;
                    setLock.lock();
                    lockedPaths.erase( *from_it );
                    setLock.unlock();
                    from_it--;
                    /* break intentionally omitted */
                case IGNORE:
                    continue;
                case TERMINATE:
                default:
                    return;
                }
            }
            else
            {
                lockedPaths.insert( *to_it );
            }
        }

        setLock.unlock();
        fs::copy( *from_it, *to_it, ec );
        setLock.lock();
        lockedPaths.erase( *from_it );
        lockedPaths.erase( *to_it );
        setLock.unlock();
        switch( checkError( ec, *from_it, *to_it ) )
        {
            case TERMINATE:
                return;
            case RETRY:
                from_it--;
                if( !to_dir)
                {
                    to_it--;
                }
            /* break intentionally omitted */
            case IGNORE:
            case NO_ERROR:
            default:
                break;
        }
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
    // TODO send error to controller and wait
    if( ec )
    {
        std::cerr << ec.message() << std::endl;
        return IGNORE;
    }
    return NO_ERROR;
}
void FileManager::notifySuccess()
{
    // TODO notify with zero
}
