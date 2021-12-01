#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "File.h"

#define BYTE_PER_SECTOR 512
#define DIRECTORY_ENTRY_SIZE 32
#define PASSWORD_ENTRY_SIZE 16

using namespace std;

class Volume
{
private:
	int sizeOfName = 14; //fixed name size
	int sizeOfPassword = 24; //fixed password length

	string name;

	string password;

	unsigned short version;
	unsigned int volumeSize; //in sectors
	unsigned short bytePerSector;
	unsigned short sectorPerCluster;
	unsigned short sectorInBootSector;
	unsigned short numFAT;
	unsigned int FATSize;
	unsigned int clusterOfRDET;
	unsigned int clusterOfPET;

	string path;
	fstream vol;

	vector<File> fileList;
public:
	Volume();
	~Volume();
	
	bool create(const char* path, unsigned int volumeSize, unsigned short bytePerSector, unsigned short sectorPerCluster);
	bool open(const char* path);
	
	void format();

	string getPassword();
	void changePassword(string newPassword);

	bool loadDirectory();
	void printFileList();
	void printDetail();

	vector<unsigned int> getFreeClusterFromFAT();
	bool setClusterUsed(unsigned int index, unsigned int value);
	unsigned int getFATValue(unsigned int index);
	bool setReservedCluster();

	vector<unsigned short> getFreeRDETEntry();
	vector<unsigned short> getFreePETEntry();

	bool setEntryToRDET(unsigned short entry, File file);
	bool deleteEntryInRDET(unsigned short entry);
	bool editPassword(unsigned short entry, string password);

	string getPasswordFromPET(unsigned short entry);

	bool writeCluster(unsigned int index, const char* data, unsigned int length);
	char* readCluster(unsigned int index);

	bool importFile(const char* path);
	bool exportFile(int fileIndex);

	bool deleteFile(int fileIndex);

	int getSizeOfBootSector();
};