#ifndef FILESYSTEMMESSAGE_HPP
#define FILESYSTEMMESSAGE_HPP

#include "FileManager.hpp"
#include "MainController/Message.hpp"


class FileSystemMessage : public Message
{
public:
    FileSystemMessage
        (
        std::error_condition err,
        FileSystemAction act,
        fs::path p1,
        fs::path p2
        );

    std::error_condition getErrCode() const;
    FileSystemAction getAction() const;
    fs::path getPath1() const;
    fs::path getPath2() const;
    std::promise<HandleErrorCommand>& getPromise();

    void prettyPrintMessage( std::ostream& out);

private:
    std::error_condition errCode;
    FileSystemAction action;
    fs::path path1;
    fs::path path2;
    std::promise<HandleErrorCommand> p;
};

#endif // FILESYSTEMMESSAGE_HPP
