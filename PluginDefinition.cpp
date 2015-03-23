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

// currently not updating fileName, not being called because of that, code it get path is 
// included in  compileJavaFile and compileAndRun functions
string getPath(string fileName)
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
    string tmpFile = path + "/tmpFile.txt";
    ifstream file(tmpFile);
    // command to redirect compiler message into file
    string compileRedirect = compileCommand + " 2> " + tmpFile;
    // command to delete tmpfile
    tmpFile = "DEL /F /S /Q /A \"" + tmpFile + "\"";
    ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileRedirect.c_str(), NULL, SW_HIDE);
    file.seekg(0, file.end);
    int fileLength = file.tellg();
    file.seekg(0, file.beg);
    ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", tmpFile.c_str(), NULL, SW_HIDE);
    //if file is empty, compile works
    if (fileLength == 0) 
    {
        ::MessageBox(NULL, TEXT("Compiling Successful"), TEXT("Compiler Message"), MB_OK);
        fileName.erase (fileName.end()-5, fileName.end());
    }
    //otherwise show message
    else 
    {
        ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", compileCommand.c_str(), NULL, SW_SHOW);
    }
    return fileName;
}

void compileJavaFile()
{
    string fileName, path;
    //everything bellow is in getPath
    SendMessageA(nppData._nppHandle, NPPM_SAVECURRENTFILE,0,0);
    wchar_t *fullPath = new wchar_t[MAX_PATH], *name = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)name);
    wstring ws = name;
    fileName = string(ws.begin(),ws.end());
    SendMessageA(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY,0,(LPARAM)fullPath);
    ws = fullPath;
    path = string(ws.begin(),ws.end());
    //everything above is in getPath()
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
    //everything below is in getPath()
    SendMessageA(nppData._nppHandle, NPPM_SAVECURRENTFILE,0,0);
    wchar_t *fullPath = new wchar_t[MAX_PATH], *name = new wchar_t[MAX_PATH];
    SendMessageA(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)name);
    wstring ws = name;
    fileName = string(ws.begin(),ws.end());
    SendMessageA(nppData._nppHandle, NPPM_GETCURRENTDIRECTORY,0,(LPARAM)fullPath);
    ws = fullPath;
    path = string(ws.begin(),ws.end());
    //everything above is in getPath()
    fileName = compile(fileName,path);
    string compare = fileName;
    compare.erase(compare.begin(), compare.end()-5);
    if (strcmp(compare.c_str(),  ".java")  == 0) return;
    run(fileName,path);
}

void tabChecker()
{

}

void JARfc()
{
    HWND hwnd;
    hwnd = CreateWindowA("BUTTON", "Choose class files",BS_CHECKBOX, CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, NULL, NULL, NULL, NULL);
    ShowWindow(hwnd, SW_SHOW);
}

void compareFiles()
{

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
