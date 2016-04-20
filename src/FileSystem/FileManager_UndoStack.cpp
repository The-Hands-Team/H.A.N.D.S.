#include "FileManager.hpp"

#include <thread>
 
FileManager::UndoStack::UndoStack()
 : stack()
 , cur_rec( (std::size_t) -1)
{
}
FileManager::UndoStack::~UndoStack()
{
}

void FileManager::UndoStack::push(FileSystemAction act, fs::paths from, fs::paths to, fs::copy_options co)
{
    stack.erase( stack.begin() + cur_rec + 1, stack.end() );
    stack.push_back( {act, std::move(from), std::move(to), co} );
    cur_rec++;
    if( MAX_RECORDS == stack.size() )
    {
        stack.pop_front();
        cur_rec--;
    }
}
 
void FileManager::UndoStack::undo()
{
    if( (std::size_t) -1 != cur_rec )
    {
        record r = stack[cur_rec--];
        switch( r.act )
        {
            case FileSystemAction::COPY:
                std::thread( &FileManager::performAction,
                             FileSystemAction::DELETE,
                             r.to, r.from,
                             r.co & (fs::copy_options::recursive | fs::copy_options::copy_symlinks | fs::copy_options::skip_symlinks)
                           ).detach();
                break;
            case FileSystemAction::MOVE:
                std::thread( &FileManager::performAction,
                             FileSystemAction::MOVE,
                             r.to, r.from,
                             r.co & (fs::copy_options::recursive | fs::copy_options::copy_symlinks | fs::copy_options::skip_symlinks)
                           ).detach();
                break;
            case FileSystemAction::DELETE:
            default:
                break;
        }
    }
}

void FileManager::UndoStack::redo()
{
    if( stack.size() - 1 != cur_rec )
    {
        record r = stack[++cur_rec];
        std::thread( &FileManager::performAction,
                     r.act,
                     r.from, r.to,
                     r.co & (fs::copy_options::recursive | fs::copy_options::copy_symlinks | fs::copy_options::skip_symlinks)
                   ).detach();
    }
}


