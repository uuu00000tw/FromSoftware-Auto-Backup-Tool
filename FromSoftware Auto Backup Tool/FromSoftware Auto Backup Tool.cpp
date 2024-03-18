// FromSoftware Auto Backup Tool - oniichi
// Release # 1.0
// Changelog:
// Added all FromSoftware games to map

#include <iostream>
#include <map>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <string>
#include <Windows.h>
#include <algorithm>

using namespace std;

std::map<std::string, std::string> GameCodes{
	{ "1", "Dark Souls"},
	{ "1R", "Dark Souls: Remastered"},
	{ "2", "Dark Souls II"},
	{ "2S", "Dark Souls II: Scholar of the First Sin"},
	{ "3", "Dark Souls 3"},
	{ "E", "Elden Ring"},
	{ "S", "Sekiro: Shadows Die Twice"},
	{ "A", "Armored Core VI: FIRES OF RUBICON"}
};

// To avoid lowercase letters
string ToUpper(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}

int main()
{
	// ------------------------------------------------------------------------------------------------
	// Declare variables used within the application
	// ------------------------------------------------------------------------------------------------

	string filePath;								// file path
	string desPath;									// destination for the backups
	int numberSaves;								// number of saves
	int interval;									// how often to backup saves
	char pathbuffer[100];							// holds where the saves are currently saved when read in from the ini file
	char backupbuffer[100];							// holds where the backups are to be saved from the ini file
	char currSave[100];								// holds where the current save is at in the counter
	int totalSaves;									// total number of saves
	string DSSAVE = "DRAKS0005.sl2";                // Name of initital save if Dark Souls I
	string DSRSAVE = "DRAKS0005.sl2";               // Name of initital save if Dark Souls I: Remastered
	string DS2SAVE = "DARKSII0000.sl2";             // Name of initital save if Dark Souls Ⅱ
	string DS2SOFSSAVE = "DS2SOFS0000.sl2";         // Name of initital save if Dark Souls Ⅱ: Scholar of the First Sin
	string DS3SAVE = "DS30000.sl2";                 // Name of initial save if Dark Souls III
	string ERSAVE = "ER0000.sl2";                   // Name of initial save if Elden Ring
	string SEKIRORSAVE = "S0000.sl2";				// Name of initial save if Sekiro: Shadows Die Twice
	string AC6ERSAVE = "AC60000.sl2";				// Name of initial save if Armored Core VI: FIRES OF RUBICON
	string GAMESAVE;
	string BACKUP;
	string fsgame;							// Which Dark Souls are you running
	string fsgamename;
	string dsVersion;
	string deletionBackup;							// Backup to get rid of when over the limit
	int deletionNum;								// Number of the backup to get rid of when deleted		
	char deletionNumber[100];						// Number of the backup to get rid of when deleted (for the deletion step)
	char currentSaveNum[999];						// Number of saves present
	bool debug = false;								// Debug information

	// ------------------------------------------------------------------------------------------------
	// load settings from the ini file
	// ------------------------------------------------------------------------------------------------
	debug = GetPrivateProfileIntA("FSABT", "debug", 0, ".\\settings.ini");							// Turn on debug information
	if (debug)
		cout << " ** DEBUG MODE IS CURRENTLY ENABLED ** " << endl;
	std::cout << R"(
	   ##     ##   ##  ######    #####            ######     ##       ####   ###  ##  ##   ##  ######
	  ####    ##   ##  # ## #   ##   ##            ##  ##   ####     ##  ##   ##  ##  ##   ##   ##  ##
	 ##  ##   ##   ##    ##     ##   ##            ##  ##  ##  ##   ##        ## ##   ##   ##   ##  ##
	 ##  ##   ##   ##    ##     ##   ##            #####   ##  ##   ##        ####    ##   ##   #####
	 ######   ##   ##    ##     ##   ##            ##  ##  ######   ##        ## ##   ##   ##   ##
	 ##  ##   ##   ##    ##     ##   ##            ##  ##  ##  ##    ##  ##   ##  ##  ##   ##   ##
	 ##  ##    #####    ####     #####            ######   ##  ##     ####   ###  ##   #####   ####   
                                                                                                       
	)" << endl;
	while (fsgame.empty()) {

		cout << "FromSoftware Auto Backup Tool V1.0" << endl;
		cout << "-------------------------------------" << endl;
		for (const std::pair<string, string>& pair : GameCodes) {
			const string& code = pair.first;
			const string& name = pair.second;
			cout << code << ": " << name << endl;
		}
		cout << "-------------------------------------" << endl;
		cout << "Please type the code: ";
		cin >> fsgame;
		fsgame = ToUpper(fsgame);

		while (GameCodes.find(fsgame) == GameCodes.end()) {
			cout << "ERROR: Wrong game code!!" << endl;
			cout << "Please enter the game code again: ";
			cin >> fsgame;
			fsgame = ToUpper(fsgame);
		}
		fsgamename = GameCodes[fsgame];
	}

	numberSaves = GetPrivateProfileIntA("FSABT", "max_saves", 5, ".\\settings.ini");		// Get the # of saves
	interval = GetPrivateProfileIntA("FSABT", "interval_minutes", 10, ".\\settings.ini");	// Get the frequency of saves																	

	if (fsgame == "1")
	{
		//Dark Souls
		GetPrivateProfileStringA("FSABT", "save_path_ds", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_ds", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsaves1", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "DRAKS0005 BACKUP - ";
		GAMESAVE = DSSAVE;
	}
	else if (fsgame == "1R")
	{
		//Dark Souls: Remastered
		GetPrivateProfileStringA("FSABT", "save_path_dsr", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_dsr", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsaves1R", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "DRAKS0005 BACKUP - ";
		GAMESAVE = DSRSAVE;
	}
	else if (fsgame == "2")
	{
		//Dark Souls 2
		GetPrivateProfileStringA("FSABT", "save_path_ds2", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_ds2", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsaves2", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "DARKSII0000 BACKUP - ";
		GAMESAVE = DS2SAVE;
	}
	else if (fsgame == "2S")
	{
		//Dark Souls 2: Scholar of the First Sin
		GetPrivateProfileStringA("FSABT", "save_path_ds2S", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_ds2S", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsaves2S", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "DS2SOFS0000 BACKUP - ";
		GAMESAVE = DS2SOFSSAVE;
	}
	else if (fsgame == "3")
	{
		//Dark Souls 3
		GetPrivateProfileStringA("FSABT", "save_path_ds3", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_ds3", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsaves3", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "DS30000 BACKUP - ";
		GAMESAVE = DS3SAVE;
	}
	else if (fsgame == "E")
	{
		//Elden Ring"
		GetPrivateProfileStringA("FSABT", "save_path_ER", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_ER", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsavesE", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "ER0000 BACKUP - ";
		GAMESAVE = ERSAVE;
	}
	else if (fsgame == "S")
	{
		//Sekiro: Shadows Die Twice
		GetPrivateProfileStringA("FSABT", "save_path_S", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_S", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsavesS", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "S0000 BACKUP - ";
		GAMESAVE = SEKIRORSAVE;
	}
	else if (fsgame == "A")
	{
		//Armored Core VI: FIRES OF RUBICON
		GetPrivateProfileStringA("FSABT", "save_path_AC6", "failed", pathbuffer, 100, ".\\settings.ini");       // Get the path for the backups
		GetPrivateProfileStringA("FSABT", "backup_path_AC6", "failed", backupbuffer, 100, ".\\settings.ini");   // Get the path for the backups
		totalSaves = GetPrivateProfileIntA("FSABT", "totalsavesA", 0, ".\\settings.ini");                   // Get the number that the saves are currently on
		BACKUP = "AC60000 BACKUP - ";
		GAMESAVE = AC6ERSAVE;
	};

	// ------------------------------------------------------------------------------------------------
	// Initital Screen
	// ------------------------------------------------------------------------------------------------

	cout << "-------------------------------------" << endl;
	cout << "Your Current Settings:" << endl << endl;
	cout << dsVersion << endl;
	cout << "Total Amount of Saves to Keep: " << numberSaves << endl;
	cout << "Frequency of Backups: Every " << interval << " minutes" << endl;
	cout << "-------------------------------------" << endl;
	cout << "Path to back up files to: " << endl;
	cout << backupbuffer << endl;
	cout << "-------------------------------------" << endl << endl;
	if (debug)
		cout << " ** DEBUG: Last backup # was " << totalSaves << endl;

	// ------------------------------------------------------------------------------------------------
	// Create Initial Backup (assuming there are no backups to begin with)
	// ------------------------------------------------------------------------------------------------
	filePath = pathbuffer + GAMESAVE;
	//desPath = backupbuffer + BACKUP + "0.sl2"; not needed?
	if (CreateDirectoryA(backupbuffer, NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		// Directory created / exists
		cout << "Back up directory detected! Commencing backups..." << endl;                
	}
	else {
		// Directory wasn't created
		cout << "Didn't create directory";		
	}

	// ------------------------------------------------------------------------------------------------
	// Create backups / Delete Old Files
	// ------------------------------------------------------------------------------------------------

	int counter = totalSaves;
	int waitTime = 60000 * interval;											// Converts time from the ini to the appropriate format
	while (true) {
		Sleep(waitTime);														// Wait the appropriate time before creating another backup
		counter++;																// Increase the counter for keeping track of the current backup
		cout << "Back up # " << counter << " has been created." << endl;
		_itoa_s(counter, currSave, 10);
		desPath = backupbuffer + BACKUP + currSave + ".sl2";
		CopyFileA(filePath.c_str(), desPath.c_str(), false);					// Creates the backup
		
		_itoa_s(counter, currentSaveNum, 10);
		if (fsgame == "1")
			WritePrivateProfileStringA("FSABT", "totalsaves1", currentSaveNum, ".\\settings.ini");
		if (fsgame == "1R")
			WritePrivateProfileStringA("FSABT", "totalsaves1R", currentSaveNum, ".\\settings.ini");
		if (fsgame == "2")
			WritePrivateProfileStringA("FSABT", "totalsaves2", currentSaveNum, ".\\settings.ini");
		if (fsgame == "2S")
			WritePrivateProfileStringA("FSABT", "totalsaves2S", currentSaveNum, ".\\settings.ini");
		if (fsgame == "3")
			WritePrivateProfileStringA("FSABT", "totalsaves3", currentSaveNum, ".\\settings.ini");
		if (fsgame == "E")
			WritePrivateProfileStringA("FSABT", "totalsavesE", currentSaveNum, ".\\settings.ini");
		if (fsgame == "S")
			WritePrivateProfileStringA("FSABT", "totalsavesS", currentSaveNum, ".\\settings.ini");
		if (fsgame == "A")
			WritePrivateProfileStringA("FSABT", "totalsavesA", currentSaveNum, ".\\settings.ini");
		if (debug) {
			//CopyFile debug
			if (CopyFileA(filePath.c_str(), desPath.c_str(), false)) {
				cout << " ** Backup File #" << counter << " has been created" << endl;
			}
			else {
				DWORD error = GetLastError();
				cout << " ** Backup #" << counter << " Failed：" << error << endl;
				cout << " ** filePath = " << filePath << endl;
				cout << " ** desPath = " << desPath << endl;
				//GetLastError error code: https://www.pnpon.com/article/detail-31.html
			}
			cout << " ** DEBUG: totalSaves being written to file: " << currentSaveNum << endl;
		}
		//cout << "-----DEBUG INFORMATION: CURRENT SAVE STEP: " << currentSaveNum << endl;
		if (counter > numberSaves) {													// Deletion of old saves
			deletionNum = counter - numberSaves;										// Select the oldest save in the folder
			_itoa_s(deletionNum, deletionNumber, 10);									// Convert to the appropriate formation
			deletionBackup = backupbuffer + BACKUP + deletionNumber + ".sl2";			// Create the file name that needs to be deleted
			cout << "Old backup has been removed (#" << deletionNumber << ")" << endl;
			DeleteFileA(deletionBackup.c_str());										// Deletes the oldest save
		}

	}

	// end
	return 0;
}

