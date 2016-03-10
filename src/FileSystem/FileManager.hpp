#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include "MainController/Message.hpp"

#include <experimental/filesystem>
#include <future>
#include <ostream>
#include <thread>


namespace fs = std::experimental::filesystem;

using t_id = std::thread::id;
namespace std{
    namespace experimental{
        namespace filesystem{
            using paths = vector<path>;
        }
    }

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

    static void deleteFiles
    (
        fs::paths& file,
        fs::copy_options options = fs::copy_options::none
    );
    static void copyFiles
    (
        fs::paths& from,
        fs::paths& to,
        fs::copy_options options = fs::copy_options::none
    );
    static void moveFiles
    (
        fs::paths& from,
        fs::paths& to,
        fs::copy_options options = fs::copy_options::none
    );


private:

    FileManager() = delete;
    ~FileManager() = delete;

    static void performAction
    (
        FileSystemAction act,
        fs::paths from,
        fs::paths to,
        fs::copy_options options
    );


    static HandleErrorCommand checkError
    (
        std::error_code& ec,
        FileSystemAction act,
        const fs::path p1 = fs::path(),
        const fs::path p2 = fs::path()
    );

    static void signalThreadEnd( FileSystemAction act );

    inline static bool compare_options
    (
        fs::copy_options lhs,
        fs::copy_options rhs
    )
    {
        return ( lhs & rhs ) != fs::copy_options::none;
    }


};

#endif //FILEMANAGER_HPP
