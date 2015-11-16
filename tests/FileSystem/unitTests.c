
#include <experimental/filesystem>
#include <iostream>

#include "../../src/FileSystem/FileManager.hpp"


using namespace std::experimental::filesystem;

int main(int argc, char* argv[])
{
	path p = absolute( path( "./testfile1" ) ); 
    std::cout << "Testing:" << std::endl;
    
    std::cout << "deleting testfile1" << std::endl;
    FileManager::getInstance()->deleteFile( p );
    std::cout << "deleted testfile1" << std::endl;
    
    
    std::cout << "Testing Done" << std::endl;
}
