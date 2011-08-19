![nonpareil for Mac OS X- HP calculator emulator](/nUxiscode/nonpareil-osx-powerpc/raw/master/readme-header.png "nonpareil for Mac OS X- HP calculator emulator")  
&nbsp;  

![Screenshot](http://www.bartosiak.org/nonpareil/index_files/nonpareil-screenshot-small.png "Screenshot")  

> #### Download _nonpareil for Mac OS X_ v. 0.17&nbsp;&nbsp;&nbsp; _PowerPC/Tiger/Leopard Patch 3_  
> ##### Read-to-run Universal binaries supporting *PowerPC* (and Intel) Macs that
> ##### run *Mac OS X 10.4 Tiger, Mac OS X 10.5 Leopard,* or (on Intel) later versions  
> Classic:&nbsp;&nbsp; 
[ HP-35 ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-35.app.zip)&nbsp;&nbsp; 
[ HP-45 ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-45.app.zip)&nbsp;&nbsp; 
[ HP-55 ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-55.app.zip)&nbsp;&nbsp; 
[ HP-80 ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-80.app.zip)  
> Woodstock:&nbsp;&nbsp; 
[ HP-21 ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-21.app.zip)&nbsp;&nbsp; 
[ HP-25 ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-25.app.zip)  
> Woodstock-spice:&nbsp;&nbsp; 
[ HP-32E ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-32e.app.zip)&nbsp;&nbsp; 
[ HP-33C ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-33c.app.zip)&nbsp;&nbsp; 
[ HP-34C ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-34c.app.zip)&nbsp;&nbsp; 
[ HP-37E ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-37e.app.zip)&nbsp;&nbsp; 
[ HP-38C ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-38c.app.zip)&nbsp;&nbsp; 
[ HP-38E ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-38e.app.zip)  
> Nut-Voyager:&nbsp;&nbsp; 
[ HP-11C ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-11c.app.zip)&nbsp;&nbsp; 
[ HP-12C ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-12c.app.zip)&nbsp;&nbsp; 
[ HP-15C ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-15c.app.zip)&nbsp;&nbsp; 
[ HP-16C ](/downloads/nUxiscode/nonpareil-osx-powerpc/nonpareil-16c.app.zip)  
> Source Code:&nbsp;&nbsp; [.zip](http://github.com/nUxiscode/nonpareil-osx-powerpc/zipball/master)
&nbsp;

[_nonpareil_](http://nonpareil.brouhaha.com/) is an emulator of, and micro-assembler for, vintage HP pocket calculators from the 1970s and early 1980s.  It is written by [Eric Smith](http://www.brouhaha.com/~eric/), and originally ran only on GTK+, found commonly on Linux and other GNU OSes.  Beginning in 2005, [Maciej Bartosiak](http://www.bartosiak.org/) has maintained a port of _nonpareil_  0.77 to Mac OS X and the modern Objective C-based Cocoa framework, which he calls [_nonpareil for Mac OS X_](http://www.bartosiak.org/nonpareil/).  It is currently on version 0.17 (at time of writing).

Initially, _nonpareil for Mac OS X_ supported Mac OS X 10.4+ running on PowerPC processors.  As Apple transitions away from PowerPC and pushes developers toward the App Store model requiring Intel-only Mac OS X 10.6 Snow Leopard or Mac OS X 10.7 Lion, they have silently removed, disabled, or hidden backwards compatibility even in minor maintenance releases of XCode, the Mac OS X development environment.  As such, it has become increasingly difficult to maintain a development environment and build configuration that builds a Universal binary supporting the PowerPC architecture and the requisite older versions of Mac OS X that will run on PowerPC.

Some time recently, support for Mac OS X 10.5 Leopard and 10.4 Tiger, and, therefore, for all PowerPC Macs, in _nonpareil for Mac OS X_ fell victim to this trend and [quit working](http://gist.github.com/1151949#gistcomment-45972) due to a change in an XCode update on the author's system.  Reversing this change would require the maintainer to downgrade XCode and perhaps even their operating system.  The maintainer of the port has indicated that they do not intend to continue to support PowerPC at this point, though the [_nonpareil for Mac OS X_ website](http://www.bartosiak.org/nonpareil/) has not been updated to reflect this.  Moreover, older releases of _nonpareil for Mac OS X_ are not available either in binary or source form on the maintainer's site.  **This leaves all PowerPC Mac users seeking to use _nonpareil for Mac OS X_ completely out in the cold.**

I have therefore modified _nonpareil for Mac OS X_ to reverse these inadvertent changes and restore compatibility with Mac OS X Tiger and Leopard, and, therefore, with PowerPC Macs in general.  This will allow **PowerPC Mac owners to again use _nonpareil for Mac OS X_**.  _To download binaries of this release, see the links above._  This has been tested only on a recent PowerPC G4 running Leopard, but I believe it should work on any 32-bit PowerPC or x86 machine supported by Mac OS X 10.4 Tiger and Mac OS X 10.5 Leopard, at least for now (see issue #1).

If you [have bug reports](/nUxiscode/nonpareil-osx-powerpc/issues) or [questions](/inbox/new/nUxiscode) relating specifically to compatibility with PowerPC machines running Mac OS X 10.4 Tiger or Mac OS X 10.5 Leopard, please report them here, not to the maintainer of _nonpareil to Mac OS X_.  For other questions and bug reports, please first contact [Maciej Bartosiak](mailto:mba@mac.com), the author and current maintainer of [_nonpareil for Mac OS X_](http://www.bartosiak.org/nonpareil/).  If you cannot resolve the issue through him, you are welcome to contact me using the GitHub [messaging](/inbox/new/nUxiscode) system or to [report the issue or bug](/nUxiscode/nonpareil-osx-powerpc/issues) here on GitHub.  I will maintain a for-upstream branch for the use of the upstream maintainer in that case.

The vast majority of the work to maintain the Mac OS X port of _nonpareil_ remains in the hands of [its author](http://www.bartosiak.org/); it has taken only minimal changes to restore (for now) the ability to use _nonpareil for OS X_ to PowerPC owners and other users of Mac OS X 10.4 Tiger and Mac OS X 10.5 Leopard.  **If you like _nonpareil for Mac OS X_, please [donate to its author](http://www.bartosiak.org/nonpareil/) to sponsor further development via PayPal on the [author's website](http://www.bartosiak.org/nonpareil/).**

All versions of _nonpareil_ are made available under the terms of the Free Software Foundation's [General Public License, Version 2](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html). 

![Mac 'Face' and Universal Logos](http://www.bartosiak.org/nonpareil/index_files/MacOSX_Universal_60px.gif "Mac 'Face' and Universal Logos")

-----------------------

```
    
    
    Copyright in included changes by nUxi_codes <code@nUxi.name> is
    hereby assigned to Maciej Bartosiak.
    
    
    Copyright (C) 2004-2006 Eric L. Smith <eric@brouhaha.com>
    Copyright (C) 2005-2011 Maciej Bartosiak <mba@mac.com>

    Nonpareil is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.  Note that I am not
    granting permission to redistribute or modify Nonpareil under the
    terms of any later version of the General Public License.
 
    Nonpareil is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with Nonpareil (in the file "COPYING"); if not, see
    <http://www.gnu.org/licenses/>.
    
     
```
