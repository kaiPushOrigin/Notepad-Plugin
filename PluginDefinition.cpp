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

#include <iostream>
#include <windows.h>
#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <fstream>
#include <stdlib.h>
using namespace std;

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
	setCommand(0, TEXT("Compile"), compile, NULL, false);
    setCommand(1, TEXT("Compile and Run"), compileAndRun, NULL, false);
	setCommand(2, TEXT("---"), NULL, NULL, false);
	setCommand(3, TEXT("JAR File Creator"), helloDlg, NULL, false);
	setCommand(4, TEXT("Tab Checker"), helloDlg, NULL, false);
	setCommand(5, TEXT("Compare Files"), helloDlg, NULL, false);
	setCommand(6, TEXT("Connect to StackOverflow"),cnctStckOvrflw, NULL, false);
	setCommand(7, TEXT("Connect to GitBash"), cnctgtbsh, NULL, false);
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

string getPath()
{
    SendMessageA(nppData._nppHandle, NPPM_SAVECURRENTFILE,0,0);
    wchar_t *fullPath = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY,0,(LPARAM)fullPath);
    wstring fP = fullPath;
    return string(fP.begin(),fP.end());
}

string getFileName()
{
	wchar_t *name = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)name);
    wstring ws = name;
    return string(ws.begin(),ws.end());
}

void compile()
{
	//Save Current Document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_SAVE);
	
	//get the path of the document
	string fileName, path;
	path = getPath();
	//get the name of the document
	fileName = getFileName();

	//making a log file
	string log = "log_" + fileName + ".txt" ;
	string logPath = path + "//"+ log;

	//combine the strings together to make one command
	string compileAndRedirCommand = "/k cd " + path + " & javac " + fileName + " 2> " + log;
	string compileCommand = "/k cd " + path + " & javac " + fileName;

	//Open Command Prompt and pipe program to JDK and redirect output to log file
	ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileAndRedirCommand.c_str(), NULL, SW_HIDE);
	
	//check log file, if empty, compiling success, else fail and show errors
	ifstream file(logPath);
	Sleep(3000);
	file.seekg(0, file.end);
	long long fileLength = file.tellg();
	file.seekg(0, file.beg);
	if (fileLength == 0) 
    {
        ::MessageBox(NULL, TEXT("Compiling Successful"), TEXT("Compiler Message"), MB_OK);
    }
    //otherwise show message
    else
    {
		::MessageBox(NULL, TEXT("Compiling Not Successful"), TEXT("Compiler Message"), MB_OK);
        ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileCommand.c_str(), NULL, SW_SHOW);	
    }
	file.close();
	remove(logPath.c_str());
}

void compileAndRun()
{
	//Save Current Document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_SAVE);
	//Use Compile
	compile();
	//get parameters for run
	string fileName,path;
	path = getPath();
	fileName = getFileName();
	Sleep(500);
	//make the string to run the file
	int length = fileName.length();
	length = length - 5;
	fileName.erase(fileName.begin()+length,fileName.end());

	string runCommand = "/k cd " + path + " & java " + fileName;
	ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", runCommand.c_str(), NULL, SW_SHOW);

}

void tabChecker()
{

}

void JARfc()
{

}

void compareFiles()
{

}

void cnctStckOvrflw()
{
	ShellExecuteA(NULL,"open","www.stackoverflow.com",NULL,NULL,SW_SHOW);
}

void cnctgtbsh()
{
	ShellExecuteA(NULL,"open","www.github.com",NULL,NULL,SW_SHOW);
}

void bgnnrsGd()
{

}