
#include <cassert>
#include <condition_variable>
#include <experimental/filesystem>
#include <functional>
#include <mutex>
#include <system_error>
#include <thread>
#include <unordered_set>

namespace fs = std::experimental::filesystem;

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

typedef std::thread::id t_id;
typedef std::vector<fs::path> v_paths;

class FileManager
{
    public:
    
    static FileManager* getInstance();

    t_id deleteFile( const fs::path &file );
    t_id deleteFiles( v_paths& file, fs::copy_options options = fs::copy_options::none  );
    t_id copyFiles( v_paths& from
                 , v_paths& to
                 , fs::copy_options options = fs::copy_options::none );
    t_id copyFiles( v_paths& from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id copyFile( const fs::path &from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFiles( v_paths& from
                 , v_paths& to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFiles( v_paths& from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFile( const fs::path &from
                 , const fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    
    enum HandleErrorCommand
    {
        TERMINATE,
        IGNORE,
        RETRY,
        NO_ERROR
    };
    
    
    
    private:
    
    FileManager();
    ~FileManager();
    
    static FileManager* currentInstance;
    

    static void static_DeleteFiles( v_paths file, fs::copy_options options );
    static void static_CopyFiles( v_paths from, v_paths to, fs::copy_options options );
    static void static_MoveFiles( v_paths from, v_paths to, fs::copy_options options );
    
    void doDeleteFiles( v_paths file, fs::copy_options options );
    void doCopyFiles( v_paths from, v_paths to, fs::copy_options options );
    void doMoveFiles( v_paths from, v_paths to, fs::copy_options options );

    
    HandleErrorCommand checkError( std::error_code& ec, const fs::path p1 = fs::path(), const fs::path p2 = fs::path() );
    void notifySuccess();
    
    inline bool compare_options( fs::copy_options lhs, fs::copy_options rhs)
    {
        return ( lhs & rhs ) != fs::copy_options::none;
    }

    
};



#endif //FILEMANAGER_HPP
