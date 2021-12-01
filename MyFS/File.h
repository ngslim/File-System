#include <iostream>
#include <string>

using namespace std;

class File
{
private:
	unsigned short entry;

	string name;
	string extension;
	unsigned int size;
	unsigned int startCluster;
	unsigned short passwordEntry;

public:
	File();
	File(unsigned short entry, string name, string extension, unsigned int size, unsigned int startCluster, unsigned short passwordEntry);
	~File();

	void setEntry(unsigned short entry);
	unsigned short getEntry();

	string getName();
	string getExtension();
	unsigned int getSize();
	unsigned int getStartCluster();
	unsigned short getPasswordEntry();
	
	void changePassword(string newPassword);
};