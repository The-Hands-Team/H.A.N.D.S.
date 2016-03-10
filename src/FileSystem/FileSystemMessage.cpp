#include "FileSystemMessage.hpp"


FileSystemMessage::FileSystemMessage
    (
    std::error_condition err,
    FileSystemAction act,
    fs::path p1,
    fs::path p2
    )
: Message(FILESYSTEM)
, errCode(err)
, action(act)
, path1(p1)
, path2(p2)
{
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


void FileSystemMessage::prettyPrintMessage( std::ostream& out)
{
    out << "Error: " << errCode.message() << " while ";
    switch( action )
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

    out << path1;
    if( FileSystemAction::DELETE != action )
    {
        out << " to " << path2;
    }

    std::endl(out);
}
