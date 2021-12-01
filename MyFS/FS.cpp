#include "FS.h"

Volume vol;
int sizeInMB;

void mainMenu()
{
	system("cls");
	int choice;
	cout << "1. Create new volume" << endl;
	cout << "2. Open existing volume" << endl;
	cout << "0. Exit" << endl << endl;
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
			cout << "Volume not existed" << endl << endl;
			system("pause");
			return;
		}
		else
		{
			cout << "Succeeded" << endl << endl;
		}
		break;
	}

	system("pause");

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
		while (!validate)
		{
			cout << "-> Input password: ";
			cin >> temp;

			if (temp == password)
			{
				cout << "Correct password" << endl;
				validate = true;
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

	cout << "1. Print directory" << endl;
	cout << "2. Set/ change password" << endl;
	cout << "3. Print volume info" << endl;
	cout << "4. Format volume" << endl;
	cout << "0. Return" << endl << endl;

	do
	{
		cout << "-> Your option: ";
		cin >> choice;
	} while (choice < 0 || choice > 4);

	system("cls");

	switch (choice)
	{
	case 0:
		return;
	case 1:
		directoryMenu();
		break;
	case 2:
		cout << "-> Input new password: ";
		cin >> temp;
		vol.changePassword(temp);
		break;
	case 3:	
		cout << "-- Volume Info --" << endl << endl;
		vol.printDetail();
		cout << endl << endl;
		system("pause");
		goto start;
	case 4:
		cout << "Formatting volume..." << endl << endl;
		vol.format();
		cout << "Succeeded" << endl << endl;
		break;
	}

	system("pause");
}

void directoryMenu()
{
	system("cls");

	int choice;

	cout << "0. Return" << endl << endl;
}

void fileMenu()
{
	system("cls");
	cout << "1. Export" << endl;
	cout << "2. Set/ change password" << endl;
	cout << "0. Return";
}
