
#include <cerrno>
#include <functional>
#include <iostream>
#include "FileManager.hpp"
#include "MainController/GestureQueue.hpp"
namespace fs = std::experimental::filesystem;

FileManager* FileManager::currentInstance = nullptr;

FileManager::FileManager() : mess_ready(false)
{
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
    t_id ret = t.get_id();
    threads.emplace(ret, std::move(t));
    return ret;
}
t_id FileManager::FileManager::deleteFile( const fs::path& file )
{
    v_paths v = v_paths(1, file);
    return deleteFiles( v );
}

t_id FileManager::copyFiles( v_paths& from
             , v_paths& to
             , fs::copy_options options )
{
    std::thread t = std::thread( &static_CopyFiles, from, to, options );
    t_id ret = t.get_id();
    threads.emplace(ret,std::move(t));
    return ret;
}
t_id FileManager::copyFiles( v_paths& from
             , const fs::path &to
             , fs::copy_options options )
{
    v_paths v = v_paths(1, to);
    return copyFiles( from, v, options );
}
t_id FileManager::copyFile( const fs::path &from
             , const fs::path &to
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
    t_id ret = t.get_id();
    threads.emplace(ret,std::move(t));
    return ret;
}
t_id FileManager::moveFiles( v_paths& from
             , const fs::path &to
             , fs::copy_options options )
{
    v_paths v = v_paths(1, to);
    return moveFiles( from, v, options );
}
t_id FileManager::moveFile( const fs::path &from
             , const fs::path &to
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
        fs::path to_delete = *it;
        if( fs::is_symlink( *it ) )
        {
            if( compare_options(fs::copy_options::skip_symlinks, options) )
            {
                continue;
            }
            else if( !compare_options( fs::copy_options::copy_symlinks, options ) )
            {
                to_delete = fs::read_symlink( *it, ec );
            }
        }

        switch( checkError( ec, DELETE, *it ) )
        {
            case TERMINATE:
                return signalThreadEnd( DELETE );
            case RETRY:
                it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                break;
        }

        if( compare_options( fs::copy_options::recursive, options ) )
        {
            fs::remove_all( *it, ec );
        }
        else
        {
            fs::remove( *it, ec );
        }
        // TODO move to trash
        switch( checkError( ec, DELETE, *it ) )
        {
            case TERMINATE:
                return signalThreadEnd( DELETE );
            case RETRY:
                it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                break;
        }
    }
    signalThreadEnd( DELETE );
}
void FileManager::doCopyFiles( v_paths from, v_paths to, fs::copy_options options )

{
    std::error_code ec;
    bool to_dir = 1 == to.size() && 1 < from.size();

    if( from.size() != to.size() &&
        !( to_dir &&
           fs::is_directory( to[0], ec ) ) ) do
    {
       switch( checkError( ec, COPY, to[0] ) )
       {
           case TERMINATE:
               return signalThreadEnd( COPY );
           case RETRY:
           case IGNORE:
               continue;
           case NO_ERROR:
               break;
       }

    ec.assign( EDOM, std::generic_category() );
    checkError( ec, COPY, to[0] );

    } while( ec );

    for( v_paths::iterator from_it = from.begin(), to_it = to.begin();
         from.end() != from_it;
         from_it++, ( to_dir ? to_it : to_it++ ) )
    {
		
		std::cout << *from_it << " " << *to_it << std::endl;

        if( !compare_options( options, fs::copy_options::recursive )
            && fs::is_directory( *from_it, ec ) )
        {
            ec.assign( EISDIR, std::generic_category() );
        }
        switch( checkError( ec, COPY, *from_it ) )
        {
            case TERMINATE:
                return signalThreadEnd( COPY );
            case RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                break;
        }

        fs::copy( *from_it, *to_it, options, ec );

        switch( checkError( ec, COPY, *from_it, *to_it ) )
        {
            case TERMINATE:
                return signalThreadEnd( COPY );
            case RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                break;
        }
    }
    signalThreadEnd( COPY );
}
void FileManager::doMoveFiles( v_paths from, v_paths to, fs::copy_options options )
{
    std::error_code ec;
    bool to_dir = 1 == to.size() && 1 < from.size();

    if( from.size() != to.size() &&
        !( to_dir &&
           fs::is_directory( to[0], ec ) ) ) do
    {
       switch( checkError( ec, MOVE, to[0] ) )
       {
           case TERMINATE:
               return signalThreadEnd( MOVE );
           case RETRY:
           case IGNORE:
               continue;
           case NO_ERROR:
               break;
       }

    ec.assign( EDOM, std::generic_category() );
    checkError( ec, MOVE, to[0]);

    } while( ec );


    for( v_paths::iterator from_it = from.begin(), to_it = to.begin(); from.end() != from_it; from_it++, ( to_dir ? to_it : to_it++ ) )
    {
        if( !compare_options( options, fs::copy_options::recursive ) && fs::is_directory( *from_it, ec ) )
        {
            ec.assign( EISDIR, std::generic_category() );
        }
        switch( checkError( ec, MOVE, *from_it ) )
        {
            case TERMINATE:
                return signalThreadEnd( MOVE );
            case RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                break;
        }

        fs::rename( *from_it, *to_it, ec );

        switch( checkError( ec, MOVE, *from_it, *to_it ) )
        {
            case TERMINATE:
                return signalThreadEnd( MOVE );
            case RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case IGNORE:
                continue;
            case NO_ERROR:
                break;
        }
    }
    signalThreadEnd( MOVE );
}

void FileManager::replyToError( t_id message_address, HandleErrorCommand message )
{
    std::unique_lock<std::mutex> lk(cond_mutex);
    cond_var.wait(lk, [=]{return !mess_ready;});
    mess_addr = message_address;
    mess = message;
    mess_ready = true;
    lk.unlock();
    cond_var.notify_all();
}

FileManager::HandleErrorCommand FileManager::checkError( std::error_code& ec, FileSystemAction act, const fs::path p1, const fs::path p2 )
{
    HandleErrorCommand ret = NO_ERROR;
    if( ec )
    {
        GestureQueue::getInstance()->push(new FileSystemMessage(std::this_thread::get_id(), ec.default_error_condition(), act, p1, p2 ));
        std::unique_lock<std::mutex> lk(cond_mutex);
        cond_var.wait(lk, [=]{return mess_ready && std::this_thread::get_id() == mess_addr;});
        ret = mess;
        mess_ready = false;
        lk.unlock();
        cond_var.notify_all();
    }
    if( TERMINATE == ret ) std::cerr << "Terminating thread: " << std::this_thread::get_id() << std::endl;
    return ret;
}
void FileManager::signalThreadEnd( FileSystemAction act)
{
    GestureQueue::getInstance()->push(new FileSystemMessage(std::this_thread::get_id(), std::error_condition(), act, fs::path(), fs::path() ));
}

void FileManager::joinThread( t_id tid )
{
    threads.at(tid).join();
    threads.erase(tid);


}
