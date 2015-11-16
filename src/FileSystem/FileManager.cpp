
#include <iostream>
#include "FileManager.hpp"
using namespace std::experimental::filesystem;

FileManager* FileManager::currentInstance = nullptr;

FileManager::FileManager()
{
	currentInstance = this;
}

FileManager::~FileManager()
{
	
}

FileManager* FileManager::getInstance()
{
	if( currentInstance == nullptr )
	{
		currentInstance = new FileManager();
	}
	
	return currentInstance;
}

void FileManager::checkError( std::error_code& ec )
{
	std::cerr << ec.value() << std::endl;
	std::cerr << ec.default_error_condition().value() << std::endl;
	
}

void FileManager::deleteFile( path& file )
{
	std::error_code ec;
	bool success = remove( file, ec );
	
	if( !success )
	{
		checkError( ec );
	}
}

void FileManager::copyFile( path& from, path& to, copy_options options )
{
	std::error_code ec;
	bool success = copy( from, to, options, ec );
	
	if( !success )
	{
		checkError( ec );
	}
}
void FileManager::moveFile( path& from, path& to )
{
	std::error_code ec;
	bool success = rename( from, to, ec );
	
	if( !success )
	{
		checkError( ec );
	}
}
