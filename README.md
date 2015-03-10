# Notepad-Plugin
So in order for us to get started, I made a little list so that you guys can get upto speed in a jiffy.

1. Download the template from http://docs.notepad-plus-plus.org/index.php/Plugin_Development_Quick_Start_Guide
2. Unzip the file anywhere you want, just as long as you dont get the files mixed up.
3. Download the 2 files from this repo(PluginDefintions.cpp and PluginDefinitions.h) and replace the ones in your folder. (They have all the necessary changes added to them)
4. Open the PluginDefintions.cpp file and go all the way to the bottom. Put your code where appropriate.(If you're just starting for the first time, skip this and just go to the next step)
5. Before you compile, go to your projects Properties, and under General-> Configuration Properties -> Character set, change it from Multi-Byte Character Set to Unicode Character set.
6. Build the project. It should succeed.(Right above the last line, the .dll file name is specified)
7. Go back to your folders and go to the ANSI Code folder. There should be a .dll file waiting. Copy and paste it under your Notepad++ installation, under plugins. (Only the dll file for now, we can add the config and about one later as we develop)
