#include "File.h"

File::File()
{
	entry = 0xFFFF;
	size = 0;
	startCluster = 0xFFFFFFFF;
	passwordEntry = 0xFFFF;
}

File::File(unsigned short entry, string name, string extension, unsigned int size, unsigned int startCluster, unsigned short passwordEntry)
{
	this->entry = entry;
	this->name = name;
	this->extension = extension;
	this->size = size;
	this->startCluster = startCluster;
	this->passwordEntry = passwordEntry;
}

File::~File()
{

}

void File::setEntry(unsigned short entry)
{
	this->entry = entry;
}

unsigned short File::getEntry()
{
	return entry;
}

string File::getName()
{
	return name;
}

string File::getExtension()
{
	return extension;
}

unsigned int File::getSize()
{
	return size;
}

unsigned int File::getStartCluster()
{
	return startCluster;
}

unsigned short File::getPasswordEntry()
{
	return passwordEntry;
}

