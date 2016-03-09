#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <cassert>
#include <experimental/filesystem>
#include <future>
#include <ostream>
#include <system_error>
#include <thread>
#include <unordered_map>
#include "MainController/Message.hpp"

namespace fs = std::experimental::filesystem;


using t_id = std::thread::id;
namespace std{
    namespace experimental{
        namespace filesystem{
            using paths = vector<path>;
        }
    }
}


namespace std
{
    template <>
    struct hash<fs::path>
    {
        size_t operator()(const fs::path& key) const
        {
            size_t ret = 5381;
            const fs::path::value_type* str = key.c_str();
            size_t c;
            while ( (c = *str++) )
            {
                ret = ((ret << 5) + ret) + c; /* hash * 33 + c */
            }
            return ret;
        }
    };
}


enum class HandleErrorCommand
{
    TERMINATE,
    IGNORE,
    RETRY,
    NO_ERROR
};

enum class FileSystemAction
{
    COPY,
    MOVE,
    DELETE
};

class FileManager
{
    public:

    static FileManager* getInstance();

    t_id deleteFile( const fs::path &file );
    t_id deleteFiles( fs::paths& file, fs::copy_options options = fs::copy_options::none  );
    t_id copyFiles( fs::paths& from
                 , fs::paths& to
                 , fs::copy_options options = fs::copy_options::none );
    t_id copyFiles( fs::paths& from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id copyFile( const fs::path &from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFiles( fs::paths& from
                 , fs::paths& to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFiles( fs::paths& from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFile( const fs::path &from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );

    void joinThread( t_id tid );


    private:

    FileManager() = default;
    ~FileManager() = default;

    static FileManager* currentInstance;


    static void static_DeleteFiles( fs::paths file, fs::copy_options options );
    static void static_CopyFiles( fs::paths from, fs::paths to, fs::copy_options options );
    static void static_MoveFiles( fs::paths from, fs::paths to, fs::copy_options options );

    void doDeleteFiles( fs::paths file, fs::copy_options options );
    void doCopyFiles( fs::paths from, fs::paths to, fs::copy_options options );
    void doMoveFiles( fs::paths from, fs::paths to, fs::copy_options options );


    HandleErrorCommand checkError( std::error_code& ec, FileSystemAction act, const fs::path p1 = fs::path(), const fs::path p2 = fs::path() );
    void signalThreadEnd( FileSystemAction act );
    t_id mess_addr;
        std::unordered_map<t_id, std::thread> threads;

    inline bool compare_options( fs::copy_options lhs, fs::copy_options rhs)
    {
        return ( lhs & rhs ) != fs::copy_options::none;
    }


};



class FileSystemMessage : public Message
{
public:
    FileSystemMessage
        (
        std::thread::id tid,
        std::error_condition err,
        FileSystemAction act,
        fs::path p1,
        fs::path p2
        );
    t_id get_t_id() const;
    std::error_condition getErrCode() const;
    FileSystemAction getAction() const;
    fs::path getPath1() const;
    fs::path getPath2() const;
    std::promise<HandleErrorCommand>& getPromise();

    static void prettyPrintMessage(const FileSystemMessage& m, std::ostream& out);

private:
    t_id id;
    std::error_condition errCode;
    FileSystemAction action;
    fs::path path1;
    fs::path path2;
    std::promise<HandleErrorCommand> p;
};

#endif //FILEMANAGER_HPP
