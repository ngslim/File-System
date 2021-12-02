#include "FS.h"

Volume vol;
int sizeInMB;

void mainMenu()
{
	system("cls");
	int choice;
	cout << "-- MAIN MENU --" << endl << endl;
	cout << "1.\tCreate new volume" << endl;
	cout << "2.\tOpen existing volume" << endl;
	cout << "0.\tExit" << endl << endl;
	do
	{
		cout << "-> Your option: ";
		cin >> choice;
	} while (choice < 0 || choice > 2);

	system("cls");

	switch (choice)
	{
	case 0:
		exit(0);
	case 1:
		cout << "-> Input size in MB: ";
		cin >> sizeInMB;

		cout << endl << "Creating volume..." << endl << endl;

		if (vol.create("MyFS.Dat", sizeInMB * 1024 * 1024 / BYTE_PER_SECTOR, BYTE_PER_SECTOR, 2))
		{
			cout << "Succeeded" << endl << endl;
		}
		else
		{
			cout << "Failed" << endl << endl;
			return;
		}
		break;
	case 2:
		cout << "Opening volume..." << endl << endl;
		if (!vol.open("MyFS.Dat"))
		{
			cout << "Err: Volume not existed" << endl << endl;
			system("pause");
			return;
		}
		else
		{
			cout << "Succeeded" << endl << endl;
		}
		break;
	}

	Sleep(SLEEP_TIME);

	volumeMenu();
}

void volumeMenu()
{
	system("cls");

	string password = vol.getPassword();

	bool validate = true;
	
	string temp;

	if (password != "")
	{
		validate = false;
		cout << "Your volume is protected by password" << endl << endl;
		cout << "* Enter 0 to return" << endl;
		while (!validate)
		{
			cout << "-> Input password: ";
			cin >> temp;

			if (temp == password)
			{
				cout << endl << "Correct password" << endl;
				validate = true;
				Sleep(SLEEP_TIME);
			}
			else if (temp == "0")
			{
				return;
			}
			else
			{
				cout << "Try again" << endl;
			}
		}
	}

	start:

	system("cls");

	int choice;

	cout << "-- VOLUME " << vol.getName() <<" MENU --" << endl << endl;

	cout << "1.\tImport new file" << endl;
	cout << "2.\tDirectory" << endl;
	cout << "3.\tSet/change password" << endl;
	cout << "4.\tDelete password" << endl;
	cout << "5.\tPrint volume info" << endl;
	cout << "6.\tFormat volume" << endl;
	cout << "0.\tReturn" << endl << endl;

	do
	{
		cout << "-> Your option: ";
		cin >> choice;
	} while (choice < 0 || choice > 6);

	system("cls");

	switch (choice)
	{
	case 0:
		return;
	case 1:
		cout << "-> Input file path: ";
		cin >> temp;

		cout << endl;

		cout << "Importing " << temp << "..." << endl << endl;

		if (vol.importFile(temp.c_str()))
		{
			cout << "Succeeded" << endl << endl;
		}
		else
		{
			cout << "Err: File in path does not exist or volume is full" << endl << endl;
		}

		Sleep(SLEEP_TIME);
		goto start;
	case 2:
		directoryMenu();
		goto start;
	case 3:
		cout << "Maximum password length is " << VOLUME_PASSWORD_SIZE << endl;
		cout << "* Enter 0 to return" << endl;
		
		input_password:
		cout << "-> Input new password: ";
		cin >> temp;

		if (temp.length() > VOLUME_PASSWORD_SIZE)
		{
			cout << "Err: Password is too long" << endl;
			goto input_password;
		}

		if (temp == "0")
		{
			goto start;
		}

		vol.changePassword(temp);
		break;
	case 4:
		cout << "Deleting password..." << endl << endl;
		Sleep(SLEEP_TIME);
		vol.changePassword("");
		cout << "Returning to main menu..." << endl << endl;
		Sleep(SLEEP_TIME);
		break;
	case 5:	
		cout << "-- Volume Info --" << endl << endl;
		vol.printDetail();
		cout << endl << endl;
		system("pause");
		goto start;
	case 6:
		cout << "Formatting volume..." << endl << endl;
		vol.format();
		cout << "Succeeded" << endl << endl;
		cout << "Returning to main menu..." << endl << endl;
		Sleep(SLEEP_TIME);
		break;
	}
}

void directoryMenu()
{
	system("cls");

	cout << "-- DIRECTORY MENU --" << endl << endl;
	cout << "* Choose file to proceed" << endl << endl;

	vol.loadDirectory();
	
	int fileCount = vol.getListSize();

	if (fileCount == 0)
	{
		cout << "Directory is empty" << endl;
		system("pause");
		return;
	}

	cout << "0.\tReturn" << endl << endl;

	vol.printFileList();

	int choice;

	cout << endl;

	do
	{
		cout << "-> Your option: ";
		cin >> choice;
	} while (choice < 0 || choice > fileCount);

	if (choice == 0)
	{
		return;
	}

	fileMenu(choice - 1);
}

void fileMenu(int fileIndex)
{
	system("cls");

	string password = vol.getFilePassword(fileIndex);

	bool validate = true;

	string temp;

	if (password != "")
	{
		validate = false;
		cout << "Your file is protected by password" << endl << endl;
		cout << "* Enter 0 to return" << endl;
		while (!validate)
		{
			cout << "-> Input password: ";
			cin >> temp;

			if (temp == password)
			{
				cout << endl << "Correct password" << endl;
				validate = true;
				Sleep(SLEEP_TIME);
			}
			else if (temp == "0")
			{
				return;
			}
			else
			{
				cout << "Try again" << endl;
			}
		}
	}
	
	start:

	system("cls");

	cout << "ACCESSING " << vol.getFileName(fileIndex) << endl << endl;

	cout << "1.\tExport" << endl;
	cout << "2.\tSet/change password" << endl;
	cout << "3.\tDelete password" << endl;
	cout << "0.\tReturn" << endl << endl;

	int choice;

	do
	{
		cout << "-> Your option: ";
		cin >> choice;
	} while (choice < 0 || choice > 3);
	
	system("cls");

	switch (choice)
	{
	case 0:
		return;
	case 1:
		cout << "Exporting file..." << endl << endl;
		if (vol.exportFile(fileIndex))
		{
			cout << "Succeeded" << endl << endl;
		}
		else
		{
			cout << "Failed" << endl << endl;
		}
		Sleep(SLEEP_TIME);
		break;
	case 2:
		cout << "Maximum password length is " << PASSWORD_ENTRY_SIZE << endl;
		cout << "* Enter 0 to return" << endl;

	input_password:
		cout << "-> Input new password: ";
		cin >> temp;

		if (temp.length() > PASSWORD_ENTRY_SIZE)
		{
			cout << "Err: Password is too long" << endl;
			goto input_password;
		}

		if (temp == "0")
		{
			break;
		}
		
		vol.editFilePassword(fileIndex, temp);
		break;
	case 3:
		cout << "Deleting password..." << endl << endl;
		Sleep(SLEEP_TIME);
		vol.editFilePassword(fileIndex, "");
		cout << "Succeeded";
		Sleep(SLEEP_TIME);
		break;
	}

	goto start;
}
