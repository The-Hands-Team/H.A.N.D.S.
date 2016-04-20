#include "FileManager.hpp"

#include <ostream>
#include <thread>

#include "FileSystemMessage.hpp"
#include "MainController/GestureQueue.hpp"


void FileManager::deleteFiles
(
    fs::paths& files,
    fs::copy_options options // = fs::copy_options::none
)
{
    undoStack.push( FileSystemAction::DELETE, files, fs::paths(), options );

    std::thread(
        &performAction,
        FileSystemAction::DELETE,
        std::move(files),
        fs::paths(),
        options
    ).detach();
}

void FileManager::copyFiles
(
    fs::paths& from,
    fs::paths& to,
    fs::copy_options options // = fs::copy_options::none
)
{
    undoStack.push( FileSystemAction::COPY, from, to, options );
    
    std::thread(
        &performAction,
        FileSystemAction::COPY,
        std::move(from),
        std::move(to),
        options
    ).detach();
}

void FileManager::moveFiles
(
    fs::paths& from,
    fs::paths& to,
    fs::copy_options options // = fs::copy_options::none
)
{
    undoStack.push( FileSystemAction::MOVE, from, to, options );
    
    std::thread(
        &performAction,
        FileSystemAction::MOVE,
        std::move(from),
        std::move(to),
        options
    ).detach();
}

void FileManager::undo()
{
    undoStack.undo();
}

void FileManager::redo()
{
    undoStack.redo();
}

/*static */ void FileManager::performAction
(
    FileSystemAction act,
    fs::paths from,
    fs::paths to,
    fs::copy_options options
)
{
    std::error_code ec;
    bool to_dir = 1 == to.size() && 1 < from.size();

    if( FileSystemAction::DELETE != act &&
        from.size() != to.size() &&
        !( to_dir &&
           fs::is_directory( to[0], ec ) ) )
    do
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

    } while( ec );


    for( auto from_it = from.cbegin(), to_it = to.cbegin();
         from.cend() != from_it;
         from_it++, ( to_dir ? to_it : to_it++ ) )
    {
        if( !compare_options( options, fs::copy_options::recursive ) &&
            fs::is_directory( *from_it, ec ) )
        {
            ec.assign( EISDIR, std::generic_category() );
        }
        switch( checkError( ec, act, *from_it ) )
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

        switch(act)
        {
            case FileSystemAction::DELETE:
                if( compare_options( fs::copy_options::recursive, options ) )
                {
                    fs::remove_all( *from_it, ec );
                }
                else
                {
                    fs::remove( *from_it, ec );
                }
                break;
            case FileSystemAction::MOVE:
                fs::rename( *from_it, *to_it, ec );
                break;
            case FileSystemAction::COPY:
                fs::copy( *from_it, *to_it, options, ec );
                break;
        }

        switch( checkError( ec, act, *from_it, *to_it ) )
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
    signalThreadEnd( act );
}


/*static */ HandleErrorCommand FileManager::checkError
(
    std::error_code& ec,
    FileSystemAction act,
    const fs::path p1, // = fs::path()
    const fs::path p2 // = fs::path()
)
{
    HandleErrorCommand ret = HandleErrorCommand::NO_ERROR;
    if( ec )
    {
        auto m = std::make_unique<FileSystemMessage>
                    (
                        ec.default_error_condition(),
                        act,
                        p1,
                        p2
                    );
        auto fut = m->getPromise().get_future();
        GestureQueue::getInstance()->push(std::move(m));

        ret = fut.get();
    }
    if( HandleErrorCommand::TERMINATE == ret )
    {
        std::cerr << "Terminating thread: "
                  << std::this_thread::get_id()
                  << std::endl;
    }
    return ret;
}
/*static */ void FileManager::signalThreadEnd( FileSystemAction act )
{
    auto m = std::make_unique<FileSystemMessage>
                (
                    std::error_condition(),
                    act,
                    fs::path(),
                    fs::path()
                );
    auto fut = m->getPromise().get_future();
    GestureQueue::getInstance()->push(std::move(m));
    fut.wait();
}
