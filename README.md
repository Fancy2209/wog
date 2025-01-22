2D BOY's Boy Framework

### Overview

This is a trimmed down and slightly modified version of the framework used in the creation of World of Goo.  It is provided as is, without any license or warranty.  If it destroys your life in any way, don't come crying to us.  It has bugs, it's a little hacky in places, and it is severly lacking in style and functionality in some areas.  This code is unsupported, so please don't send us questions about it. Like a bad horoscope, it's for entertainment purposes only. There's a new section in our forums called Boy Framework where you can discuss this framework with others.  
  
On a more positive note, it does fulfill one basic need well:  it was build with rapid prototyping of 2D games in mind and tries to minimize the amount of non-game setup work required to get something running.  All the stuff that 99.9% of 2D games have in common has been pulled out into this reusable framework.  It can render 2D graphics, read input from a variety of devices, and play sounds.  It includes a resource manager for loading/unloading resources, a persistence layer for saving game state, and even a file access abstraction.  Two demos are included.  One illustrates a minimalistic use of the framework (demo1) and the other is a quick remake of the classic arcade game Asteroids made in 12 hours (demo2).  
  
We hope this inspires people to play around with new ideas and saves them the annoyance of having to reinvent the wheel.  If you make something cool, we encourage you to post it to our forum and share the joy!  

### Setup Instructions

1.  Unzip framework.zip into a new folder (I'll call this folder from this point on)
2.  Download irrKlang version 1.1.0 from here: [http://www.ambiera.com/irrklang/downloads.html](http://www.ambiera.com/irrklang/downloads.html). Newer versions will probably work too, but you'd have to update the paths in your project if you want to use one. IMPORTANT NOTE: irrKlang is not free if you intend to use it for commercial purposes.
3.  Unzip irrKlang-1.1.0.zip into /libs. It will create a direcory called irrKlang-1.1.0.
4.  Copy <root>/libs/irrKlang-1.1.0/bin/win32-visualStudio/irrKlang.dll into <root>/demo1 and <root>/demo2 (or add the above directory to your path so that the demos can find the irrKlang dll)
5.  Run <root>/libs/dxsdk/200806/Redist/DXSetup.exe. This will install the DirectX runtime version required by the framework. Alternatively, you can just find yourself a copy of d3dx9\_38.dll and put a copy of it in the demo1 and demo2 directories.
6.  Start up the Visual Studio solution <root>/2dboy.sln (requires Visual Studio 2008)
7.  Build and run!

### Misc Notes

*   You can create your own fonts for use in your games.  the font file format is that used by the Popcap frameowork, which comes with a font builder tool.  The resource manifest (resource.xml) also uses a simpliefied version of the format available in the Popcap Framework.
*   Demo1 requires a dualshock style game controller (it simply does nothing without one)
*   There is no documentation for this framework.  There are inline comments that should help you if you're looking at the innards.