#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include "MainController/Message.hpp"

#include <deque>
#include <future>

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

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

    FileManager() = default;
    ~FileManager() = default;

    void deleteFiles
    (
        fs::paths& file,
        fs::copy_options options = fs::copy_options::none
    );
    void copyFiles
    (
        fs::paths& from,
        fs::paths& to,
        fs::copy_options options = fs::copy_options::none
    );
    void moveFiles
    (
        fs::paths& from,
        fs::paths& to,
        fs::copy_options options = fs::copy_options::none
    );
    
    void undo();
    void redo();


private:

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

    static inline bool compare_options
    (
        fs::copy_options lhs,
        fs::copy_options rhs
    )
    {
        return ( lhs & rhs ) != fs::copy_options::none;
    };
        
    
    class UndoStack
    {
        public:
            struct record
            {
                FileSystemAction act;
                fs::paths from;
                fs::paths to;
                fs::copy_options co;
            };
            
        UndoStack();
        ~UndoStack();
        void undo();
        void redo();
        void push(FileSystemAction act, fs::paths from, fs::paths to, fs::copy_options co);
        
        private:
        std::deque<record> stack;
        std::size_t cur_rec;
        static const std::size_t MAX_RECORDS = 10;

    } undoStack;


};

#endif //FILEMANAGER_HPP
