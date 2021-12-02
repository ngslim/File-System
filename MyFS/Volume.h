#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include "File.h"

#define BYTE_PER_SECTOR 512
#define DIRECTORY_ENTRY_SIZE 32
#define PASSWORD_ENTRY_SIZE 16

#define VOLUME_NAME_SIZE 14
#define VOLUME_PASSWORD_SIZE 24

using namespace std;

class Volume
{
private:
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

	string getName();

	string getPassword();
	void changePassword(string newPassword);

	int getListSize();

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

	string getPasswordFromPET(unsigned short entry);

	bool writeCluster(unsigned int index, const char* data, unsigned int length);
	char* readCluster(unsigned int index);

	string getFileName(int fileIndex);
	string getFilePassword(int fileIndex);

	bool editFilePassword(int fileIndex, string password);

	bool importFile(const char* path);
	bool exportFile(int fileIndex);

	bool deleteFile(int fileIndex);

	int getSizeOfBootSector();
};