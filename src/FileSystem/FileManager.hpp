
#include <cassert>
#include <condition_variable>
#include <experimental/filesystem>
#include <functional>
#include <mutex>
#include <system_error>
#include <thread>
#include <unordered_map>

namespace fs = std::experimental::filesystem;

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

using t_id = std::thread::id;
namespace fs
{
    using paths = std::vector<fs::path>;
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

class FileManager
{
    public:

    enum HandleErrorCommand
    {
        TERMINATE,
        IGNORE,
        RETRY,
        NO_ERROR
    };

    enum FileSystemAction
    {
        COPY,
        MOVE,
        DELETE
    };

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

    void replyToError( t_id message_address, HandleErrorCommand message );

    void joinThread( t_id tid );


    private:

    FileManager();
    ~FileManager();

    static FileManager* currentInstance;


    static void static_DeleteFiles( fs::paths file, fs::copy_options options );
    static void static_CopyFiles( fs::paths from, fs::paths to, fs::copy_options options );
    static void static_MoveFiles( fs::paths from, fs::paths to, fs::copy_options options );

    void doDeleteFiles( fs::paths file, fs::copy_options options );
    void doCopyFiles( fs::paths from, fs::paths to, fs::copy_options options );
    void doMoveFiles( fs::paths from, fs::paths to, fs::copy_options options );


    HandleErrorCommand checkError( std::error_code& ec, FileSystemAction act, const fs::path p1 = fs::path(), const fs::path p2 = fs::path() );
    void signalThreadEnd( FileSystemAction act );

    std::mutex cond_mutex;
    std::condition_variable cond_var;
    bool mess_ready;
    t_id mess_addr;
    HandleErrorCommand mess;

    std::unordered_map<t_id, std::thread> threads;

    inline bool compare_options( fs::copy_options lhs, fs::copy_options rhs)
    {
        return ( lhs & rhs ) != fs::copy_options::none;
    }


};



#endif //FILEMANAGER_HPP
