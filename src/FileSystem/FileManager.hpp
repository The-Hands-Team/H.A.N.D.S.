
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

namespace std
{
    template <>
    struct hash<fs::path>
    {
        size_t operator()(const fs::path& key) const
        {
            assert( key.is_absolute() );
            size_t ret = 5381;
            const fs::path::value_type* str = key.c_str();
            size_t c;
            while (c = *str++)
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
    
    static FileManager* getInstance();

    t_id deleteFile( fs::path &file );
    t_id deleteFiles( v_paths& file );
    t_id copyFiles( v_paths& from
                 , v_paths& to
                 , fs::copy_options options = fs::copy_options::none );
    t_id copyFiles( v_paths& from
                 , fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id copyFile( fs::path &from
                 , fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFiles( v_paths& from
                 , v_paths& to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFiles( v_paths& from
                 , fs::path &to
                 , fs::copy_options options = fs::copy_options::none );
    t_id moveFile( fs::path &from
                 , fs::path &to
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
    

    static void static_DeleteFiles( v_paths file );
    static void static_CopyFiles( v_paths from, v_paths to, fs::copy_options options );
    static void static_MoveFiles( v_paths from, v_paths to, fs::copy_options options );
    
	void doDeleteFiles( v_paths file );
    void doCopyFiles( v_paths from, v_paths to, fs::copy_options options );
    void doMoveFiles( v_paths from, v_paths to, fs::copy_options options );
    
    std::unordered_set<fs::path> lockedPaths;
    std::mutex setLock;
    std::condition_variable notifier;
    
    HandleErrorCommand checkError( std::error_code& ec, fs::path p1 = fs::path(), fs::path p2 = fs::path() );
    
    
};



#endif //FILEMANAGER_HPP
