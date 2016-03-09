
#include <cerrno>
#include <functional>
#include "FileManager.hpp"
#include "MainController/GestureQueue.hpp"
#include <utility>
namespace fs = std::experimental::filesystem;

FileManager* FileManager::currentInstance = nullptr;

FileManager* FileManager::getInstance()
{
    if( currentInstance == nullptr )
    {
        currentInstance = new FileManager();
    }

    return currentInstance;
}

t_id FileManager::FileManager::deleteFiles( fs::paths& files, fs::copy_options options )
{
    std::thread t = std::thread( &static_DeleteFiles, files, options );
    t_id ret = t.get_id();
    threads.emplace(ret, std::move(t));
    return ret;
}
t_id FileManager::FileManager::deleteFile( const fs::path& file )
{
    fs::paths v = fs::paths(1, file);
    return deleteFiles( v );
}

t_id FileManager::copyFiles( fs::paths& from
             , fs::paths& to
             , fs::copy_options options )
{
    std::thread t = std::thread( &static_CopyFiles, from, to, options );
    t_id ret = t.get_id();
    threads.emplace(ret,std::move(t));
    return ret;
}
t_id FileManager::copyFiles( fs::paths& from
             , const fs::path &to
             , fs::copy_options options )
{
    fs::paths v = fs::paths(1, to);
    return copyFiles( from, v, options );
}
t_id FileManager::copyFile( const fs::path &from
             , const fs::path &to
             , fs::copy_options options )
{
    fs::paths v1 = fs::paths( 1, from );
    fs::paths v2 = fs::paths( 1, to );
    return copyFiles( v1, v2, options );
}

t_id FileManager::moveFiles( fs::paths& from
             , fs::paths& to
             , fs::copy_options options )
{
    std::thread t = std::thread( &static_MoveFiles, from, to, options );
    t_id ret = t.get_id();
    threads.emplace(ret,std::move(t));
    return ret;
}
t_id FileManager::moveFiles( fs::paths& from
             , const fs::path &to
             , fs::copy_options options )
{
    fs::paths v = fs::paths(1, to);
    return moveFiles( from, v, options );
}
t_id FileManager::moveFile( const fs::path &from
             , const fs::path &to
             , fs::copy_options options )
{
    fs::paths v1 = fs::paths( 1, from );
    fs::paths v2 = fs::paths( 1, to );
    return moveFiles( v1, v2, options );
}

void FileManager::static_DeleteFiles( fs::paths files, fs::copy_options options )
{
    getInstance()->doDeleteFiles( files, options );
}


void FileManager::static_CopyFiles( fs::paths from, fs::paths to, fs::copy_options options )
{
    getInstance()->doCopyFiles( from, to, options );
}

void FileManager::static_MoveFiles( fs::paths from, fs::paths to, fs::copy_options options )
{
    getInstance()->doMoveFiles( from, to, options );
}

void FileManager::doDeleteFiles( fs::paths files, fs::copy_options options )
{
    std::error_code ec;
    fs::paths canonicals;
    canonicals.reserve( files.size() );
    for( fs::paths::iterator it = files.begin(); files.end() != it; it++ )
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

        switch( checkError( ec, FileSystemAction::DELETE, *it ) )
        {
            case HandleErrorCommand::TERMINATE:
                return signalThreadEnd( FileSystemAction::DELETE );
            case HandleErrorCommand::RETRY:
                it--;
            /* break intentionally omitted */
            case HandleErrorCommand::IGNORE:
                continue;
            case HandleErrorCommand::NO_ERROR:
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
        switch( checkError( ec, FileSystemAction::DELETE, *it ) )
        {
            case HandleErrorCommand::TERMINATE:
                return signalThreadEnd( FileSystemAction::DELETE );
            case HandleErrorCommand::RETRY:
                it--;
            /* break intentionally omitted */
            case HandleErrorCommand::IGNORE:
                continue;
            case HandleErrorCommand::NO_ERROR:
                break;
        }
    }
    signalThreadEnd( FileSystemAction::DELETE );
}
void FileManager::doCopyFiles( fs::paths from, fs::paths to, fs::copy_options options )

{
    std::error_code ec;
    bool to_dir = 1 == to.size() && 1 < from.size();

    if( from.size() != to.size() &&
        !( to_dir &&
           fs::is_directory( to[0], ec ) ) ) do
    {
       switch( checkError( ec, FileSystemAction::COPY, to[0] ) )
       {
           case HandleErrorCommand::TERMINATE:
               return signalThreadEnd( FileSystemAction::COPY );
           case HandleErrorCommand::RETRY:
           case HandleErrorCommand::IGNORE:
               continue;
           case HandleErrorCommand::NO_ERROR:
               break;
       }

    ec.assign( EDOM, std::generic_category() );
    checkError( ec, FileSystemAction::COPY, to[0] );

    } while( ec );

    for( fs::paths::iterator from_it = from.begin(), to_it = to.begin();
         from.end() != from_it;
         from_it++, ( to_dir ? to_it : to_it++ ) )
    {

        std::cout << *from_it << " " << *to_it << std::endl;

        if( !compare_options( options, fs::copy_options::recursive )
            && fs::is_directory( *from_it, ec ) )
        {
            ec.assign( EISDIR, std::generic_category() );
        }
        switch( checkError( ec, FileSystemAction::COPY, *from_it ) )
        {
            case HandleErrorCommand::TERMINATE:
                return signalThreadEnd( FileSystemAction::COPY );
            case HandleErrorCommand::RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case HandleErrorCommand::IGNORE:
                continue;
            case HandleErrorCommand::NO_ERROR:
                break;
        }

        fs::copy( *from_it, *to_it, options, ec );

        switch( checkError( ec, FileSystemAction::COPY, *from_it, *to_it ) )
        {
            case HandleErrorCommand::TERMINATE:
                return signalThreadEnd( FileSystemAction::COPY );
            case HandleErrorCommand::RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case HandleErrorCommand::IGNORE:
                continue;
            case HandleErrorCommand::NO_ERROR:
                break;
        }
    }
    signalThreadEnd( FileSystemAction::COPY );
}
void FileManager::doMoveFiles( fs::paths from, fs::paths to, fs::copy_options options )
{
    std::error_code ec;
    bool to_dir = 1 == to.size() && 1 < from.size();

    if( from.size() != to.size() &&
        !( to_dir &&
           fs::is_directory( to[0], ec ) ) ) do
    {
       switch( checkError( ec, FileSystemAction::MOVE, to[0] ) )
       {
           case HandleErrorCommand::TERMINATE:
               return signalThreadEnd( FileSystemAction::MOVE );
           case HandleErrorCommand::RETRY:
           case HandleErrorCommand::IGNORE:
               continue;
           case HandleErrorCommand::NO_ERROR:
               break;
       }

    ec.assign( EDOM, std::generic_category() );
    checkError( ec, FileSystemAction::MOVE, to[0]);

    } while( ec );


    for( fs::paths::iterator from_it = from.begin(), to_it = to.begin(); from.end() != from_it; from_it++, ( to_dir ? to_it : to_it++ ) )
    {
        if( !compare_options( options, fs::copy_options::recursive ) && fs::is_directory( *from_it, ec ) )
        {
            ec.assign( EISDIR, std::generic_category() );
        }
        switch( checkError( ec, FileSystemAction::MOVE, *from_it ) )
        {
            case HandleErrorCommand::TERMINATE:
                return signalThreadEnd( FileSystemAction::MOVE );
            case HandleErrorCommand::RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case HandleErrorCommand::IGNORE:
                continue;
            case HandleErrorCommand::NO_ERROR:
                break;
        }

        fs::rename( *from_it, *to_it, ec );

        switch( checkError( ec, FileSystemAction::MOVE, *from_it, *to_it ) )
        {
            case HandleErrorCommand::TERMINATE:
                return signalThreadEnd( FileSystemAction::MOVE );
            case HandleErrorCommand::RETRY:
                from_it--;
                if( to_dir ) to_it--;
            /* break intentionally omitted */
            case HandleErrorCommand::IGNORE:
                continue;
            case HandleErrorCommand::NO_ERROR:
                break;
        }
    }
    signalThreadEnd( FileSystemAction::MOVE );
}


HandleErrorCommand FileManager::checkError( std::error_code& ec, FileSystemAction act, const fs::path p1, const fs::path p2 )
{
    HandleErrorCommand ret = HandleErrorCommand::NO_ERROR;
    if( ec )
    {
        auto m = std::make_unique<FileSystemMessage>
                    (
                    std::this_thread::get_id(),
                    ec.default_error_condition(),
                    act,
                    p1,
                    p2
                    );
        auto fut = m->getPromise().get_future();
        GestureQueue::getInstance()->push(std::move(m));

        ret = fut.get();
    }
    if( HandleErrorCommand::TERMINATE == ret ) std::cerr << "Terminating thread: " << std::this_thread::get_id() << std::endl;
    return ret;
}
void FileManager::signalThreadEnd( FileSystemAction act )
{
    auto m = std::make_unique<FileSystemMessage>(std::this_thread::get_id(), std::error_condition(), act, fs::path(), fs::path() );
    auto fut = m->getPromise().get_future();
    GestureQueue::getInstance()->push(std::move(m));
    fut.wait();
}

void FileManager::joinThread( t_id tid )
{
    threads.at(tid).join();
    threads.erase(tid);


}

FileSystemMessage::FileSystemMessage
    (
    std::thread::id tid,
    std::error_condition err,
    FileSystemAction act,
    fs::path p1,
    fs::path p2
    )
: Message(FILESYSTEM)
, id(tid)
, errCode(err)
, action(act)
, path1(p1)
, path2(p2)
{
}
t_id FileSystemMessage::get_t_id() const
{
    return id;
}
std::error_condition FileSystemMessage::getErrCode() const
{
    return errCode;
}
FileSystemAction FileSystemMessage::getAction() const
{
    return action;
}
fs::path FileSystemMessage::getPath1() const
{
    return path1;
}
fs::path FileSystemMessage::getPath2() const
{
    return path2;
}
std::promise<HandleErrorCommand>& FileSystemMessage::getPromise()
{
  return std::ref(p);
}


void FileSystemMessage::prettyPrintMessage(const FileSystemMessage& m, std::ostream& out)
{
    out << "Error: " << m.getErrCode().message() << " while ";
    switch( m.getAction() )
    {
        case FileSystemAction::COPY:
            out << "copying ";
            break;
        case FileSystemAction::MOVE:
            out << "moving ";
            break;
        case FileSystemAction::DELETE:
            out << "deleteing ";
            break;
    }

    out << m.getPath1();
    if( FileSystemAction::DELETE != m.getAction() )
    {
        out << " to " << m.getPath2();
    }

    std::endl(out);
}
