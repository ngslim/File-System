#include "Volume.h"

unsigned int FREE = 0x00000000;
unsigned int EOFILE = 0xEFFFFFFF;

unsigned int nameLength = 10;
unsigned int extensionLength = 5;
unsigned int passwordLength = 16;

int Volume::getListSize()
{
	return fileList.size();
}

Volume::Volume()
{
	version = 1;
	name = "MyFS";
	sectorInBootSector = 1;
	numFAT = 1;
	clusterOfRDET = 2;
	clusterOfPET = 3;
}

Volume::~Volume()
{
}

bool Volume::create(const char* path, unsigned int volumeSize, unsigned short bytePerSector, unsigned short sectorPerCluster)
{
	this->volumeSize = volumeSize;
	this->bytePerSector = bytePerSector;
	this->sectorPerCluster = sectorPerCluster;
	this->path = path;

	unsigned int eachSectorFATEntry = bytePerSector / sizeof(unsigned int);
	
	FATSize = (volumeSize - sectorInBootSector) / (2 * eachSectorFATEntry + 1);

	vol.open(path, ios::binary | ios::out);	

	if (vol.fail())
	{
		return false;
	}

	vol.write(name.c_str(), VOLUME_NAME_SIZE);
	vol.write((char*)&version, sizeof(version));
	vol.write((char*)&volumeSize, sizeof(volumeSize));
	vol.write((char*)&bytePerSector, sizeof(bytePerSector));
	vol.write((char*)&sectorPerCluster, sizeof(sectorPerCluster));
	vol.write((char*)&sectorInBootSector, sizeof(sectorInBootSector));
	vol.write((char*)&numFAT, sizeof(numFAT));
	vol.write((char*)&FATSize, sizeof(FATSize));
	vol.write((char*)&clusterOfRDET, sizeof(clusterOfRDET));
	vol.write((char*)&clusterOfPET, sizeof(clusterOfPET));

	password = "";
	vol.write(password.c_str(), VOLUME_PASSWORD_SIZE);
	
	char c = 0;
	for (int i = 0; i < bytePerSector * sectorInBootSector - getSizeOfBootSector(); i++)
		vol.write(&c, sizeof(c));
	for (int i = 0; i < (volumeSize - sectorInBootSector) * bytePerSector; i++)
		vol.write(&c, sizeof(c));

	vol.close();
	
	setReservedCluster();

	return true;
}

bool Volume::open(const char* path)
{
	this->path = path;
	
	vol.open(path, ios::binary | ios::in);
	
	if (vol.fail())
	{
		return false;
	}

	char* temp = new char[VOLUME_NAME_SIZE + 1];
	vol.read(temp, VOLUME_NAME_SIZE);
	temp[VOLUME_NAME_SIZE] = '\0';
	name.assign(temp, strlen(temp));

	delete[] temp;

	vol.read((char*)&version, sizeof(version));
	vol.read((char*)&volumeSize, sizeof(volumeSize));
	vol.read((char*)&bytePerSector, sizeof(bytePerSector));
	vol.read((char*)&sectorPerCluster, sizeof(sectorPerCluster));
	vol.read((char*)&sectorInBootSector, sizeof(sectorInBootSector));
	vol.read((char*)&numFAT, sizeof(numFAT));
	vol.read((char*)&FATSize, sizeof(FATSize));
	vol.read((char*)&clusterOfRDET, sizeof(clusterOfRDET));
	vol.read((char*)&clusterOfPET, sizeof(clusterOfPET));

	temp = new char[VOLUME_PASSWORD_SIZE + 1];
	vol.read(temp, VOLUME_PASSWORD_SIZE);
	temp[VOLUME_PASSWORD_SIZE] = '\0';
	password.assign(temp, strlen(temp));

	delete[] temp;
	
	vol.close();

	return true;
}

void Volume::format()
{
	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
	{
		return;
	}

	char c = 0;
	 
	vol.seekp(getSizeOfBootSector() - VOLUME_PASSWORD_SIZE);

	for (int i = 0; i < bytePerSector * sectorInBootSector - getSizeOfBootSector() - VOLUME_PASSWORD_SIZE; i++)
		vol.write(&c, sizeof(c));
	for (int i = 0; i < (volumeSize - sectorInBootSector) * bytePerSector; i++)
		vol.write(&c, sizeof(c));

	vol.close();

	setReservedCluster();
}

string Volume::getName()
{
	return name;
}

bool Volume::loadDirectory()
{
	fileList.clear();

	vol.open(path, ios::binary | ios::in);

	if (vol.fail())
	{
		return false;
	}

	unsigned int offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR + (clusterOfRDET * sectorPerCluster) * BYTE_PER_SECTOR;

	unsigned short entryCount = sectorPerCluster * bytePerSector / DIRECTORY_ENTRY_SIZE;

	for (unsigned short i = 0; i < entryCount; i++)
	{
		char isFile = 0;
		vol.seekg(offset + i * DIRECTORY_ENTRY_SIZE);
		vol.read((char*)&isFile, sizeof(isFile));
		if (isFile == 1)
		{
			string name;
			string extension;
			unsigned int size;
			unsigned int startCluster;
			unsigned short passwordEntry;

			string password;
			
			char* temp = new char[nameLength + 1];
			vol.read(temp, nameLength);
			temp[nameLength] = '\0';
			name.assign(temp, strlen(temp));

			delete[] temp;

			temp = new char[extensionLength + 1];
			vol.read(temp, extensionLength);
			temp[extensionLength] = '\0';
			extension.assign(temp, strlen(temp));

			delete[] temp;

			vol.read((char*)&size, sizeof(size));
			vol.read((char*)&startCluster, sizeof(startCluster));
			vol.read((char*)&passwordEntry, sizeof(passwordEntry));

			File newFile(i, name, extension, size, startCluster, passwordEntry);

			//newFile.setPassword(getPasswordFromPET(passwordEntry));

			fileList.push_back(newFile);
		}
	}

	vol.close();

	return true;
}

void Volume::printFileList()
{
	cout << "Index\tName\t\t\tSize" << endl << endl;

	for (int i = 0; i < fileList.size(); i++)
	{
		cout << i + 1 << ".\t" << fileList[i].getName() << '.' << fileList[i].getExtension() << "\t\t" << fileList[i].getSize() << endl;
	}
}

void Volume::printDetail()
{
	cout << "Name: " << name << endl;
	cout << "Version: " << version << endl;
	cout << "Size: " << volumeSize * BYTE_PER_SECTOR << " bytes" << endl;
	cout << "Bytes per sector: " << bytePerSector << endl;
	cout << "Sector per cluster: " << sectorPerCluster << endl;
	cout << "Sector in Boot Sector: " << sectorInBootSector << endl;
	cout << "Number of FAT: " << numFAT << endl;
	cout << "FAT Size: " << FATSize << endl;
	cout << "Cluster of RDET: " << clusterOfRDET << endl;
	cout << "Cluster of PET: " << clusterOfPET << endl;
	cout << "Password: " << password << endl;
}

vector<unsigned int> Volume::getFreeClusterFromFAT()
{
	vector<unsigned int> list;

	vol.open(path, ios::binary | ios::in);

	unsigned int value;

	unsigned int numIndexInFAT = FATSize * bytePerSector / sizeof(unsigned int);

	unsigned int offsetFAT = sectorInBootSector * bytePerSector;

	for (unsigned int i = 0; i < numIndexInFAT; i++)
	{
		vol.seekg(offsetFAT + i * sizeof(unsigned int));
		vol.read((char*)&value, sizeof(value));

		if (value == FREE)
		{
			list.push_back(i);
		}
	}

	vol.close();

	return list;
}

bool Volume::setClusterUsed(unsigned int index, unsigned int value)
{
	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
	{
		return false;
	}
	
	vol.seekp(sectorInBootSector * bytePerSector + index * 4);

	vol.write((char*)&value, sizeof(value));

	vol.close();

	return true;
}

unsigned int Volume::getFATValue(unsigned int index)
{
	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
	{
		return 0x00000000;
	}

	unsigned int value = EOFILE;

	vol.seekg(sectorInBootSector * bytePerSector + index * 4);

	vol.read((char*)&value, sizeof(value));

	vol.close();

	return value;
}

bool Volume::setReservedCluster()
{
	if (!setClusterUsed(0, EOFILE))
	{
		return false;
	}
	if (!setClusterUsed(1, EOFILE))
	{
		return false;
	}
	if (!setClusterUsed(clusterOfRDET, EOFILE))
	{
		return false;
	}
	if (!setClusterUsed(clusterOfPET, EOFILE))
	{
		return false;
	}

	return true;
}

vector<unsigned short> Volume::getFreeRDETEntry()
{
	unsigned short entryCount = sectorPerCluster * bytePerSector / DIRECTORY_ENTRY_SIZE;
	vector<unsigned short> list;

	char* RDET = readCluster(clusterOfRDET);

	for (unsigned short i = 0; i < entryCount; i++)
	{
		if (RDET[DIRECTORY_ENTRY_SIZE * i] == 0)
		{
			list.push_back(i);
		}
	}

	return list;
}

vector<unsigned short> Volume::getFreePETEntry()
{	
	unsigned short entryCount = sectorPerCluster * bytePerSector / PASSWORD_ENTRY_SIZE;
	vector<unsigned short> list;

	char* PET = readCluster(clusterOfPET);

	for (unsigned short i = 0; i < entryCount; i++)
	{
		if (PET[PASSWORD_ENTRY_SIZE * i] == 0)
		{
			list.push_back(i);
		}
	}

	return list;
}

bool Volume::setEntryToRDET(unsigned short entry, File file)
{
	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
		return false;

	unsigned int offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR + (clusterOfRDET * sectorPerCluster) * BYTE_PER_SECTOR;
	
	char isFile = 1;

	unsigned int size = file.getSize();
	unsigned int startCluster = file.getStartCluster();
	unsigned short passwordEntry = file.getPasswordEntry();

	vol.seekp(offset + entry * DIRECTORY_ENTRY_SIZE);
	vol.write((char*)&isFile, sizeof(isFile));
	vol.write(file.getName().c_str(), nameLength);
	vol.write(file.getExtension().c_str(), extensionLength);
	vol.write((char*)&size, sizeof(size));
	vol.write((char*)&startCluster, sizeof(startCluster));
	vol.write((char*)&passwordEntry, sizeof(passwordEntry));

	vol.close();

	return true;
}

bool Volume::deleteEntryInRDET(unsigned short entry)
{
	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
		return false;

	unsigned int offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR + (clusterOfRDET * sectorPerCluster) * BYTE_PER_SECTOR;

	char blank = 0;

	vol.seekp(offset + entry * DIRECTORY_ENTRY_SIZE);

	for (int i = 0; i < DIRECTORY_ENTRY_SIZE; i++)
	{
		vol.write((char*)&blank, sizeof(blank));
	}

	vol.close();

	return true;
}

string Volume::getPasswordFromPET(unsigned short entry)
{
	string password = "";

	if (entry == 0xFFFF)
	{
		return password;
	}

	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
		return password;

	unsigned int offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR + (clusterOfPET * sectorPerCluster) * BYTE_PER_SECTOR;

	char* temp = new char[passwordLength + 1];

	vol.seekg(offset + entry * PASSWORD_ENTRY_SIZE);
	vol.read(temp, passwordLength);
	temp[passwordLength] = '\0';
	password.assign(temp, strlen(temp));

	vol.close();

	return password;
}

bool Volume::editFilePassword(int fileIndex, string password)
{
	vector<unsigned short> freePETEntry = getFreePETEntry();

	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
		return false;

	unsigned short entry = fileList[fileIndex].getEntry();
	unsigned short passwordEntry;
	unsigned short currentPasswordEntry;

	unsigned int offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR + (clusterOfRDET * sectorPerCluster) * BYTE_PER_SECTOR;
	vol.seekg(offset + entry * DIRECTORY_ENTRY_SIZE + sizeof(char) + nameLength + extensionLength + sizeof(unsigned int) + sizeof(unsigned int));
	
	vol.read((char*)&currentPasswordEntry, sizeof(currentPasswordEntry));

	
	
	vol.seekp(offset + entry * DIRECTORY_ENTRY_SIZE + sizeof(char) + nameLength + extensionLength + sizeof(unsigned int) + sizeof(unsigned int));

	if (password.length() == 0)
	{
		passwordEntry = 0xFFFF;
		if (currentPasswordEntry != 0xFFFF)
		{
			vol.write((char*)&passwordEntry, sizeof(passwordEntry));
			char blank = 0;
			offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR + (clusterOfPET * sectorPerCluster) * BYTE_PER_SECTOR;
			vol.seekp(offset + currentPasswordEntry * PASSWORD_ENTRY_SIZE);
			
			for (int i = 0; i < PASSWORD_ENTRY_SIZE; i++)
			{
				vol.write((char*)&blank, 1);
			}
		}
	}
	else
	{
		if (currentPasswordEntry != 0xFFFF)
		{
			passwordEntry = currentPasswordEntry;
			
		}
		else
		{
			passwordEntry = freePETEntry[0];
			vol.write((char*)&passwordEntry, sizeof(passwordEntry));
		}

		offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR + (clusterOfPET * sectorPerCluster) * BYTE_PER_SECTOR;
		vol.seekp(offset + passwordEntry * PASSWORD_ENTRY_SIZE);
		vol.write(password.c_str(), passwordLength);
	}

	vol.close();

	return true;
}

bool Volume::writeCluster(unsigned int index, const char* data, unsigned int length)
{
	vol.open(path, ios::binary | ios::in | ios::out);

	if (vol.fail())
	{
		return false;
	}

	unsigned int offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR;

	vol.seekp(offset + (index * sectorPerCluster) * BYTE_PER_SECTOR);

	vol.write(data, length);

	vol.close();
	
	return true;
}

char* Volume::readCluster(unsigned int index)
{
	vol.open(path, ios::binary | ios::in);

	if (vol.fail())
	{
		return nullptr;
	}
	
	char* data = new char[sectorPerCluster * BYTE_PER_SECTOR];

	unsigned int offset = (sectorInBootSector + FATSize * numFAT) * BYTE_PER_SECTOR;

	vol.seekg(offset + (index * sectorPerCluster) * BYTE_PER_SECTOR);
	vol.read(data, sectorPerCluster * BYTE_PER_SECTOR);

	vol.close();

	return data;
}

string Volume::getFileName(int fileIndex)
{
	return fileList[fileIndex].getName() + "." + fileList[fileIndex].getExtension();
}

string Volume::getFilePassword(int fileIndex)
{
	return getPasswordFromPET(fileList[fileIndex].getPasswordEntry());
}

string Volume::getPassword()
{
	return password;
}

void Volume::changePassword(string newPassword)
{
	vol.open(path, ios::binary | ios::in | ios::out);
	
	if (vol.fail())
	{
		return;
	}

	password = newPassword;

	vol.seekp(getSizeOfBootSector() - VOLUME_PASSWORD_SIZE, ios::beg);
	vol.write(password.c_str(), VOLUME_PASSWORD_SIZE);
	vol.close();
}

bool Volume::importFile(const char* path)
{
	fstream file;
	file.open(path, ios::binary | ios::in | ios::ate);
	
	if (file.fail())
	{
		return false;
	}

	string pathString = path;

	int pathSeparator = pathString.find_last_of("/\\");

	string fileName = pathString.substr(0, pathSeparator);

	int extensionSeparator = fileName.find_last_of(".");

	string name = fileName.substr(0, extensionSeparator);
	string extension = fileName.substr(extensionSeparator + 1);

	unsigned int size = file.tellg();

	int clusterNeed = size / (sectorPerCluster * bytePerSector) + 1;

	vector<unsigned int>  freeCluster = getFreeClusterFromFAT();

	for (int i = 0; i < clusterNeed; i++)
	{
		setClusterUsed(freeCluster[i], freeCluster[i + 1]);
	}

	setClusterUsed(freeCluster[clusterNeed - 1], EOFILE);

	vector<unsigned short>  freeRDETEntry = getFreeRDETEntry();

	File newFile(0, name, extension, size, freeCluster[0], 0xFFFF);

	setEntryToRDET(freeRDETEntry[0], newFile);

	file.seekg(0);
	
	int bufferLength = sectorPerCluster * bytePerSector;

	char* buffer = new char[bufferLength];

	int sizeLeft = size;

	for (int i = 0; i < clusterNeed - 1; i++)
	{
		file.read(buffer, bufferLength);
		writeCluster(freeCluster[i], buffer, bufferLength);
		sizeLeft -= bufferLength;
	}

	file.read(buffer, sizeLeft);
	writeCluster(freeCluster[clusterNeed - 1], buffer, sizeLeft);

	delete[] buffer;

	file.close();

	return true;
}

bool Volume::exportFile(int fileIndex)
{
	int i = fileIndex;

	if (i >= fileList.size())
	{
		return false;
	}

	vector<unsigned int> clusterList;

	unsigned int clusterPointer = fileList[i].getStartCluster();

	while (clusterPointer != EOFILE)
	{
		clusterList.push_back(clusterPointer);
		clusterPointer = getFATValue(clusterPointer);
	}

	string fileName = fileList[i].getName() + "." + fileList[i].getExtension();

	fstream file;
	file.open(fileName.c_str(), ios::binary | ios::out);

	if (file.fail())
	{
		return false;
	}

	int bufferLength = sectorPerCluster * bytePerSector;
	char* buffer = new char[bufferLength];

	int sizeLeft = fileList[i].getSize();
	
	for (int i = 0; i < clusterList.size() - 1; i++)
	{
		buffer = readCluster(clusterList[i]);
		file.write(buffer, bufferLength);
		sizeLeft -= bufferLength;
	}

	buffer = readCluster(clusterList[clusterList.size() - 1]);
	file.write(buffer, sizeLeft);

	file.close();

	delete[] buffer;

	return true;
}

bool Volume::deleteFile(int fileIndex)
{
	int i = fileIndex;

	if (i >= fileList.size())
	{
		return false;
	}

	vector<unsigned int> clusterList;

	unsigned int clusterPointer = fileList[i].getStartCluster();

	while (clusterPointer != EOFILE)
	{
		clusterList.push_back(clusterPointer);
		clusterPointer = getFATValue(clusterPointer);
	}

	for (int i = 0; i < clusterList.size(); i++)
	{
		setClusterUsed(clusterList[i], FREE);
	}

	deleteEntryInRDET(fileList[i].getEntry());
	
	return true;
}

int Volume::getSizeOfBootSector()
{
	return VOLUME_NAME_SIZE + sizeof(version) + sizeof(volumeSize) + sizeof(bytePerSector) + sizeof(sectorPerCluster) + sizeof(sectorInBootSector) + sizeof(numFAT) + sizeof(FATSize) + sizeof(clusterOfRDET) + sizeof(clusterOfPET) + VOLUME_PASSWORD_SIZE;
}

