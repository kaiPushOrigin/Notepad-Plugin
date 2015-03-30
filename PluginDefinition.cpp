//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <iostream>
#include <fstream>
//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
    
    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("Compile"), compileJavaFile , NULL, false);   
    setCommand(1, TEXT("Compile and Run"), compileAndRun, NULL, false);
	setCommand(2, TEXT("---"), NULL, NULL, false);
	setCommand(3, TEXT("JAR File Creator"), JARfc, NULL, false);
	setCommand(4, TEXT("Tab Checker"), helloDlg, NULL, false);
	setCommand(5, TEXT("Compare Files"), helloDlg, NULL, false);
	setCommand(6, TEXT("Connect to StackOverflow"), helloDlg, NULL, false);
	setCommand(7, TEXT("Connect to GitBash"), helloDlg, NULL, false);
	setCommand(8, TEXT("Beginner's Guide"), helloDlg, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void hello()
{
    // Open a new document
    ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;
    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

    // Say hello now :
    // Scintilla control has no Unicode mode, so we use (char *) here
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)"Hello, Notepad++!");
}

void helloDlg()
{
    ::MessageBox(NULL, TEXT("Hello, Notepad++!"), TEXT("Notepad++ Plugin Template"), MB_OK);
}

string getPath(string &fileName)
{
    SendMessageA(nppData._nppHandle, NPPM_SAVECURRENTFILE,0,0);
    wchar_t *fullPath = new wchar_t[MAX_PATH], *name = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)name);
    wstring ws = name;
    fileName = string(ws.begin(),ws.end());
    SendMessageA(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY,0,(LPARAM)fullPath);
    ws = fullPath;
    return string(ws.begin(),ws.end());
}    

string compile(string fileName, string path)
{
    string compileCommand = "/k cd " + path + " & javac " + fileName;
    string errorLog = path + "/log_" + fileName + ".txt"; 
	string compileRedirect = compileCommand + " 2> " + errorLog;
    ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileRedirect.c_str(), NULL, SW_HIDE);
    Sleep(3000);
    ifstream file(errorLog);
    file.seekg(0, file.end);
    long long fileLength = file.tellg();
    file.seekg(0, file.beg);
    if (fileLength == 0) 
    {
        ::MessageBox(NULL, TEXT("Compiling Successful"), TEXT("PESMU Message"), MB_OK);
        fileName.erase(fileName.end()-5, fileName.end());
    }
    else 
    {
        ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileCommand.c_str(), NULL, SW_SHOW);
    }
    file.close();
    remove(errorLog.c_str());
    return fileName;
}

void compileJavaFile()
{
    string fileName, path;
    path = getPath(fileName);
    compile(fileName,path);
}

void run(string fileName, string path)
{
    string runCommand = "/k cd " + path + " & java " + fileName;
    ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", runCommand.c_str(), NULL, SW_SHOW);
}

void compileAndRun()
{ 
    string fileName, path;
    path = getPath(fileName);
    fileName = compile(fileName,path);
    string compare = fileName;
    compare.erase(compare.begin(), compare.end()-5);
    if (strcmp(compare.c_str(),  ".java")  == 0) return;
    run(fileName,path);
}

void tabChecker()
{

}

void searchForClassFiles(string path, string *&fileNames, int &arraySize)
{
    WIN32_FIND_DATAA findFileData;
    path = path + "/*.class";
    HANDLE find = FindFirstFileA(path.c_str(), &findFileData) ;
    if (find == INVALID_HANDLE_VALUE) 
    {
        ::MessageBox(NULL, TEXT("No java class files found."), TEXT("PESMU message"), MB_OK);
    } 
    else
    {
        arraySize = 1;
        while (FindNextFileA(find, &findFileData) != 0)
        {
            arraySize++;
        }
        fileNames = new string[arraySize];
        find = FindFirstFileA(path.c_str(), &findFileData);
        fileNames[0] = findFileData.cFileName;
        for (int i = 1; i < arraySize; i++)
        {
            FindNextFileA(find, &findFileData);
            fileNames[i] = findFileData.cFileName;
        }
    }
}

void chooseClassFiles(string &mainClass, string names[], int arraySize)
{
    int width = 400, height = 500, xPos = (GetSystemMetrics(SM_CXSCREEN) - width)/2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - height)/2, buttonYPos = 30;
    string *newNames = new string[arraySize-1];//array to store class files , with main class file removed
    //create window
    HWND hwnd = CreateWindowA("BUTTON", "Choose main class file",  WS_OVERLAPPEDWINDOW | BS_GROUPBOX | WS_GROUP ,xPos, yPos, width, height, NULL, NULL, NULL, NULL);
    HWND *hwndButton = new HWND[arraySize];
    for (int i = 0; i < arraySize; i++)  
    {
        //add button to window, one for each file in array
        hwndButton[i] = CreateWindowA("BUTTON", names[i].c_str(),WS_VISIBLE |  WS_CHILD | BS_AUTOCHECKBOX, 10, buttonYPos, 350, 20, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),NULL);      
        buttonYPos += 30;
    }
    //button to close window, not complete
    HWND okButton = CreateWindowA("BUTTON", "OK",WS_VISIBLE |  WS_CHILD | WS_GROUP | BS_PUSHBUTTON, (width/2)-25, height-20, 50, 20, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),NULL); 
    
    ShowWindow(hwnd, SW_SHOW);
    //untested below
    for (int i = 0; i < arraySize; i++)
    {
        //checks each button for the one selected, saves selected file in string
        long state = SendMessage(hwndButton[i], BM_GETSTATE, 0, 0);
        if (state == BST_CHECKED)
        {
            mainClass = names[i];
            //stores all files except main class file into array
            int j;
            for (j = 0; j < i; j++)
            {
                newNames[j] = names[j];
            }
            for (int k = j+1; k < arraySize-1; k++)
            {
                newNames[k] = names[k+1];
            }
        }

    }
    //create new window,same as above, with checkboxes
    HWND checkHwnd = CreateWindowA("BUTTON", "Choose class files to include",  WS_OVERLAPPEDWINDOW | BS_GROUPBOX | WS_GROUP ,xPos, yPos, width, height, NULL, NULL, NULL, NULL);
    HWND *hwndCheckButton = new HWND[arraySize];
    for (int i = 0; i < arraySize-1; i++)  
    {
        hwndCheckButton[i] = CreateWindowA("BUTTON", newNames[i].c_str(),WS_VISIBLE |  WS_CHILD | BS_AUTOCHECKBOX, 10, buttonYPos, 350, 20, hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),NULL);      
        buttonYPos += 30;
    }
}

void JARfc()
{
    string fileName, path, mainClass;
    path = getPath(fileName);
    string *fileNames = NULL;
    int arraySize = 0;
    searchForClassFiles(path, fileNames, arraySize);
    chooseClassFiles(mainClass, fileNames, arraySize);
}

void compareFiles()
{
	vector<wstring> vps;
	vector<TCHAR*> wps;
	int n = SendMessage(nppData._nppHandle,NPPM_GETNBOPENFILES,0,ALL_OPEN_FILES);
	if( n > 0 && n <128 )
	{
		wps.resize(n);
		for( int i = 0;i < n;i++ )
		{
			wstring ws;
			ws.resize(512);
			vps.push_back(ws);
			wps[i] = (TCHAR*)vps[i].c_str();
		}
		SendMessage(nppData._nppHandle,NPPM_GETOPENFILENAMES,(LPARAM)&wps[0],n);
	}
	wstring ab = vps[0].c_str();
	wstring cd = vps[1].c_str();
	string file1 = string(ab.begin(),ab.end());
	string file2 = string(cd.begin(),cd.end());
	string concate = "/k fc /n " + file1 + " " + file2;
    ShellExecuteA(NULL,"open", "C:/WINDOWS/system32/cmd.exe",concate.c_str(),NULL,SW_SHOW);
}

void cnctStckOvrflw()
{

}

void cnctgtbsh()
{

}

void bgnnrsGd()
{

}
