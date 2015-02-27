
#include <iostream>0
#include <windows.h>
using namespace std;
int main()
{
    ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", "/k javac C:/Users/Rikku-strife/scripts/Desktop/HelloWorld.java", NULL, SW_SHOW);
    ShellExecuteA(NULL, "open", "C:/WINDOWS/system32/cmd.exe", "/K java HelloWorld", "C:/Users/Rikku-strife/scripts/Desktop", SW_SHOW);
}
