
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
#include <system_error>
#include <unordered_map>
#include <mutex>

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP
class FileManager
{
	public:
	
	static FileManager* getInstance();
	
	void deleteFile( path& file );
	void copyFile( path& from
	             , path& to
	             , copy_options options = copy_options::none );
	void moveFile( path& from, path& to );
	
	
	
	private:
	
	FileManager();
	~FileManager();
	
	static FileManager* currentInstance;
	
    std::unordered_map<path, void> locks;
	std::mutex mapLock;
	
	void checkError( std::error_code& ec );
	
	
};



#endif //FILEMANAGER_HPP
