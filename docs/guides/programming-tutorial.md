# OpenDoors programming tutorial

This tutorial preserves the complete programming walkthrough and example discussion from the OpenDoors 6.00 manual.

## Detailed reference

The compiler, linker, and BBS launch instructions describe the original 6.00 distribution. Use [Building OpenDoors](building.md) for current toolchains; the API workflow and programming discussion remain useful.

The OpenDoors programmer's manual is intended to serve as a complete tutorial, guide and reference to writing programs with OpenDoors. Chapter 1 of this manual, beginning on page 5, provides an introduction and overview of the features of OpenDoors. If you are unsure of what OpenDoors will do for you, begin with Chapter 1. Chapter 2, beginning on page 9, provides important information related to this evaluation copy of OpenDoors, and how to register your copy. Chapter 3 serves as a tutorial on OpenDoors and BBS door programming in general. Chapter 4 provides a reference to the OpenDoors API functions which you can use in your programs. Chapter 5 provides a reference to the "OpenDoors control structure", which gives you access to a wide array of information, and allows you to customize OpenDoor's appearance and behavior. Chapter 6 provides information on special OpenDoors features and advanced door programming topics. Among the subjects discussed in chapter 6 are the Win32 version of OpenDoors, configuration files, multi- node operation, RIP graphics, logfile support, defining custom door information file formats, and more.

Chapter 7 (which begins on page 242) gives instructions on troubleshooting programs written with OpenDoors, lists solutions to common difficulties, and has information about the many sources for OpenDoors support. If at any time you are having difficulty with OpenDoors, be sure to refer to this chapter for complete step-by-step instruction on tracing the source of your problem, and for solutions to common difficulties with OpenDoors. This chapter also directs you to some of the major sources of support, including information on the OpenDoors email conference, the OpenDoors support BBS, and how to get in touch with me.

You will also find many useful tools in this manual, which will no doubt come in useful while working with OpenDoors. Beginning on page 2 is a basic table of contents, showing you how the manual is organized, and helping you to locate general topics. At the end of the manual, beginning on page 267, is an index to help you locate more information on specific topics. The manual also includes a glossary, on page 256, which will help you in understanding new terms that you may come across while reading the manual. At the end of the manual, you will also find several useful sections, such as information on what is new in this version, information on how to contact me, and information about new OpenDoors features currently in the works.

You will likely want to print this manual, to make reading and reference while programming easier. To print this manual, simply type the following line from your DOS prompt. If you are worried about the size of this manual, you might consider using a utility that can print multiple pages of a text file on a single sheet of paper. Printing two manual pages per side of paper should certainly be legible, and even four-up would give you text about the size of average newspaper text. Printing on both sides, you should be able to fit the manual on about 34 sheets of paper (269/8 < 34).

### Compiling A Program With Opendoors

The process of compiling a program written with OpenDoors is very similar to that of compiling any other program. However, there are two additional steps which you must be sure to remember:

1.)  You must include the OPENDOOR.H header file.

2.)  You must link your program with the appropriate OpenDoors library file.

All programs written with OpenDoors, must "include" the OPENDOOR.H header file. If you have placed the OPENDOOR.H header file in the same directory as your program's source code, place the following line at the beginning of your .C or .CPP file(s):

```c
#include "opendoor.h"
```

If you have placed the OPENDOOR.H header file in the same directory as other standard header files (such as stdio.h), place the following line at the beginning of your .C or .CPP file(s):

```c
#include <opendoor.h>
```

In addition to including the OpenDoors header file in your source code modules, you must also "link" the appropriate OpenDoors library file with your program. The procedure for doing this depends upon which compiler you are using. The following sections describe how to link with the OpenDoors libraries using various compilers.

### Linking With Opendoors Using A Dos Compiler

This section describes how to link with the provided OpenDoors library files under a variety of DOS compilers. If you are using a compiler other than those described here, refer to your compiler's manual for information on how to link with third- party libraries.

If you are using Borland Turbo C 2.00 or earlier, you can cause your compiler to link your program with the OpenDoors library by creating a text file with a .PRJ extension. In this text file, you should list the names of your program's .C modules, along with the name of the appropriate OpenDoors library file, as listed in the table at the end of this section. You should then select this Project file from within the Turbo C IDE prior to compiling your program.

If you are using Turbo C++ or Borland C++, you can set your compiler to link your program with the OpenDoors library by creating a project file from within the IDE. To do this, choose the Open Project command from the Project menu, and enter the name for your new project file in the Load Project dialog box. Then add the names of your program's .C/.CPP modules, along with the name of the appropriate OpenDoors library file, by pressing [Insert] in the project window. When you return to Turbo C++ or Borland C++ again, you can work with the same project file by using the Open command from the Project menu.

If you are using any Microsoft C compiler, such as Quick C, Microsoft C or Visual C++, you can set your compiler to link your program with the OpenDoors library by creating a makefile. You can create a new project file from within Quick C by using the Set Program List option from the Make menu. You can do this from within Visual C++ by using the New command from the Project menu. You should add the names of your program's .C/.CPP source files, along with the name of the appropriate OpenDoors library file, to the newly create makefile.

There are several different DOS library files included with OpenDoors, each one for use with a different memory model. The following chart lists the library file names, along with their corresponding memory model. It is important that you use the library file which corresponds to the memory model you are using. Whenever you change your compiler to use a different memory model, it is important to rebuild all of your source files (using the "Build All" or "Rebuild All" command) in addition to changing the library that your program is being linked with. If you are unfamiliar with the concept of memory models, you should refer to your compiler's manuals. If you are unsure as to what memory model your compiler is currently using, check this setting in the compile options dialog box or command line reference information.

```c
  +------------------------------------------------+
  | Library     | Memory                           |
  | Filename    | Model                            |
  |-------------|----------------------------------|
  | ODOORS.LIB  | DOS small memory model library   |
  |             |                                  |
  | ODOORM.LIB  | DOS medium memory model library  |
  |             | (Available separately)           |
  |             |                                  |
  | ODOORC.LIB  | DOS compact memory model library |
  |             | (Available separately)           |
  |             |                                  |
  | ODOORL.LIB  | DOS large memory model library   |
  |             |                                  |
  | ODOORH.LIB  | DOS huge memory model library    |
  +------------------------------------------------+
```

To understand how to compile a program written with OpenDoors, it is a good idea to try compiling one of the example programs, such as ex_hello.c, that are included in the OpenDoors package.

### Linking With Opendoors Using A Windows Compiler

The Win32 version of OpenDoors resides in a DLL, ODOORS60.DLL. In order to use OpenDoors from a Win32 program, you will typically link to an import library (although it is also possible to use load-time dynamic linking through the use of LoadLibrary() and GetProcAddress()). The OpenDoors package includes a COFF-format import library for use Microsoft compilers, named ODOORW.LIB. If you are using a compiler that uses OMF-format object files, such as a Borland compiler, you will need to create your own version of the odoorw.lib import library, by using the implib utility provided with your compiler.

When compiling an OpenDoors program with a Windows compiler, be sure that either the WIN32 or __WIN32__ constant is defined. Microsoft and Borland compilers define one of these constants by default. However, if you are using a compiler from another company, you may need to explicitly configure your compiler to define one of these preprocessor constants.

If you are using Microsoft Visual C++ 2.0 or later, you can setup your compiler to link with the OpenDoors import library by creating a makefile (choose File|New|Project) and adding both your program's .C/.CPP source file(s) and the odoorw.lib import library to the project. When prompted for the Project type, choose "Application", not a "MFC AppWizard". If you are using Visual C++ 2.0, then you must manually edit the .mak file using a text editor. In this file, replace all occurrences of "/SUBSYSTEM:windows" with "/SUBSYSTEM:windows,4.0". This instructs the linker to create an executable file that is targeted for Windows 95. If you do not do this, some of the OpenDoors visual elements will not appear correctly. Later versions of Microsoft's compiler default to using "/SUBSYSTEM:windows,4.0", and so this step is no longer necessary with those compilers.

If you are using Borland C++ 4.50 or later, you must create an OpenDoors import library for ODOORS60.DLL before you can compile your first OpenDoors program. To do this, go to the directory where ODOORS60.DLL is located, move the original odoorw.lib to a backup location, and issue the command:

IMPLIB ODOORW.LIB ODOORS60.DLL

This will create a new import library (ODOORW.LIB) which you can then use with your compiler. To compile an OpenDoors program from the command line, issue the command:

BCC32 -tW your_program.c ODOORW.LIB

To compile an OpenDoors program from within the IDE, create a new project file, and add both your program's source file(s) and the OpenDoors import library to that project. If you are compiling from within the IDE, check the TargetExpert and be sure that you are using the multithreaded version of the the runtime libraries. By default, the Borland IDE compiles single- threaded, which will not work with OpenDoors.

Additional information on the Win32 version of OpenDoors is provided in chapter 6.

### Running A Door Program Written With Opendoors

This section provides information on how to run a BBS door program that has been written with OpenDoors. If you are using OpenDoors to write some other form of online software, the information provided here will apply to different degrees, depending on the nature of your program.

OpenDoors supports both local and remote modes. In the normal mode of operation, remote mode, your program's output will be displayed to both the local screen and the remote user's screen. To run your program in remote mode, you will usually set it up to run under some BBS package. However, for testing purposes, it is often convenient to run your program in local mode.

There are several ways to start your program in local mode. The first method is to place the example DORINFO1.DEF file in the same directory as your program. If your program uses the OpenDoors command line processing function, [`od_parse_cmd_line()`](../reference/api/od_parse_cmd_line.md), then you can start your program in local mode by simply specifying -local on your program's command line. For example, you can try the example program include with OpenDoors by issuing the command VOTEDOS -LOCAL (for the DOS version) or VOTEWIN -LOCAL (for the Windows 95/NT version). OpenDoors will also run in local mode if you set it up to run under a BBS package, and log into the BBS in local mode. When the BBS runs your door program, OpenDoors will automatically run in local mode.

To run your program in remote mode, you will probably want to run it under a BBS system. If you don't have a BBS package for testing purposes, you might want to obtain a popular BBS package such as Wildcat!, Maximus (which is free) or RemoteAccess.

### Running Dos-Based Door Programs

DOS BBS packages typically run door programs using one of two methods. Either the BBS package directly loads and executes the program, or it exits to a DOS batch file, which in turn executes the door program. In either case, the BBS package produces a door information file, common called a "drop file", which provides information to the door program such as the name of the current user. OpenDoors automatically supports the common drop file formats, including DORINFOx.DEF and DOOR.SYS.

### Running Windows 95/Nt Door Programs

This section provides information specific to running door programs that are compiled with the Win32 version of OpenDoors. Please feel free to include this information in your program's manual.

Since the Win32 version of OpenDoors resides in a DLL, ODOORS60.DLL, this file must be present on any system where your program will be run. Although Windows 95/NT will find this file if it is located in the same directory as your program's executable file, it is a good idea to install this DLL into the Windows system directory. This way, all programs using the Win32 version of OpenDoors can share the same copy of the DLL, reducing the amount of disk space that is used.

The required setup for a Windows 95/NT door will depend upon what BBS system it is being run under. If you the program is being run under a native Windows 95/NT BBS system, then ideally that BBS system will provide the ability to pass a live serial port handle to the door program, on the program's command line. Otherwise, you should run the door from a batch file, following the instructions provided below for running the program under a DOS-based BBS system. If the BBS system is able to pass a live Window communications handle on the door's command line, and you are using the OpenDoors standard command-line processing function ([`od_parse_cmd_line()`](../reference/api/od_parse_cmd_line.md)), then you can just setup the BBS to run the program directly, using the command line:

YourProgramName.exe -handle xxxxxxxxxx

where xxxxxxxxx is the serial port handle, in decimal format. You do not need to use the start command, nor the DTRON utility, and you do not have to change the COM<n>AutoAssign setting in the system.ini file.

If you are running the Win32 door program under a DOS-based BBS system, or a Windows-based BBS system that is unable to pass a live serial port handle to the door program, then follow these steps:

1.Add a line of the form "COM<n>AutoAssign=<x>" to the [386Enh] section of your system.ini file. Here, <n> specifies the serial port number that the BBS's modem is connected to, and <x> will usually be 0. For example, if your modem is connected to COM1, you would add a line such as "COM1AutoAssign=0" (sans quotes). You will then have to re- start your computer for this change to take effect. If you do not do this, the Windows-based door program will not be able to access the modem.

2.Setup the BBS software to run the Windows-based door program just as you would any other door program. You will probably want to do this from a batch file. The command line that runs the Windows program should be of the form:

start /w /m YourProgramName.exe [any command line parameters]

This will cause the Windows-based door program to start in minimized mode, and cause the calling MS-DOS session to wait until the Windows program exits before continuing. If you do not wish the program to be started in minimized mode, remove the /m from the command line. If you attempt to start the door program by calling it directly, rather than using the "start /w" command, the BBS software will immediately start again, cause it to attempt to run simultaneously with the door program.

3.After running the start command, use DTRON.EXE or a similar utility to re-enable DTR detection by the modem. Normally, this command line will be of the form:

dtron /port x /bps y

Where x is the serial port number (0 for COM1, 1 for COM2, etc.) and y is the locked bps rate. For example, if your serial port is locked at 38400 bps and is connected to COM2, you would use:

dtron /port 1 /bps 38400

For full information on the DTRON utility, simply type the command line:

dtron /help

You may freely redistribute the DTRON utility that is included in this package with your program.

Additional information on the Win32 version of OpenDoors, and further explanation of some of these steps, is provided in chapter 6.

### Basics Of Door Programming With Opendoors

This section provides a complete tutorial to the basics of writing BBS door programs using OpenDoors. If you are using OpenDoors to write other online software, much of this information will still be relevant.

In addition to reading this section, I would encourage you to look at the example programs included int the OpenDoors packages. These programs, which are described beginning on page 38, will give you a much better idea of what an OpenDoors program will look like. These programs can also serve as a great starting point for writing your own programs using OpenDoors.

Probably the best means of introduction to door programming with OpenDoors is by doing it yourself. As such, I strongly encourage you to try compiling and running the simple introduction program below. For instructions on compiling programs written with OpenDoors, see page 22.

DOS version:

```c
#include "opendoor.h"
```

```c
main()
{
   od_printf("Welcome to my first door program!\n\r");
   od_printf("Press a key to return to BBS!\n\r");
   od_get_key(TRUE);
   od_exit(0, FALSE);
}
```

Win32 version:

```c
#include "opendoor.h"
```

```c
int WINAPI WinMain(HINSTANCE hInstance,
   HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int nCmdShow)
{
   od_printf("Welcome to my first door program!\n\r");
   od_printf("Press a key to return to BBS!\n\r");
   od_get_key(TRUE);
   od_exit(0, FALSE);
}
```

Keep in mind that even this simple program will automatically have all of the door capabilities we have already mentioned. Notice the line that reads #include "opendoor.h". All programs written with OpenDoors must include the OPENDOOR.H header file in order to compile correctly. The first two lines in the main/WinMain function simply call the OpenDoors [`od_printf()`](../reference/api/od_printf.md) function. [`od_printf()`](../reference/api/od_printf.md) is similar to the printf() function that C programmers will already be familiar with. However, unlike printf(), the [`od_printf()`](../reference/api/od_printf.md) function sends the output to both the modem and the local screen. Notice that the lines of text displayed by the [`od_printf()`](../reference/api/od_printf.md) function end with a "\n\r" sequence, instead of the normal "\n". This is because the terminal emulation software that is running on the remote user's system usually requires both a carriage return and a line feed to correctly begin a new line. The next line in our example program is the OpenDoors single-key input function, [`od_get_key()`](../reference/api/od_get_key.md). The TRUE value causes OpenDoors to wait for a key to be pressed (again, either from remote or local keyboard) before returning. The last line of the main/WinMain function is a call to [`od_exit()`](../reference/api/od_exit.md). Any program using OpenDoors should call this function. For the time being, you can always use the (0, FALSE) parameters.

Once again, you are encouraged to try compiling and running this program, as described above. Congratulations, you have written your first door program! Feel free to make any changes to this program, and see what effects your changes have.

To simplify this example, separate versions of this program are shown for the DOS and Win32 versions of OpenDoors. However, you would typically write your program so that it could be compiled using either the DOS or Win32 versions of OpenDoors, by beginning the mainline function as follows:

#ifdef ODPLAT_WIN32 int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) #else int main(int argc, char *argv[]) #endif

In case you are not entirely familiar with the operation of door programs, we will now provide an introduction to the internals of a door's operation. Keep in mind that OpenDoors automatically carries out most of these tasks for you. When any door program starts up, one of the first things it must do is to read the door information file(s) (sometimes called a "drop file") passed to it by the BBS. When a user is on-line, and wishes to run a door, they will most likely select a command from a menu. At this point, the BBS system (such as RemoteAccess, Maximus, PC- Board or whatever), will create a file of information about the system, who is currently on-line, and so on. Various BBS packages produce various styles of door information files. OpenDoors automatically recognizes and reads a wide variety of door information file formats. As a result, your doors will be able to run on a almost any BBS system.

Fortunately, OpenDoors takes care of all the work involved in detecting and reading the door information file, and then initializing and communicating with the serial port for you. In order to carry out these tasks, along with setting up the status line, and so on, OpenDoors provides a function called [`od_init()`](../reference/api/od_init.md). If you do not explicitly call this function, the first call to any other OpenDoors function (such as the first time your door program outputs anything) will automatically cause the [`od_init()`](../reference/api/od_init.md) function to be called. As a result, upon the first call to an OpenDoors function, all of the initialization tasks for the door will automatically be carried out. However, there may be times when you will want your program to have access information about the user who is on-line, or carry out other actions which require [`od_init()`](../reference/api/od_init.md) to have been executed - prior to the point where you call any other OpenDoors functions. In this case, you will have to call [`od_init()`](../reference/api/od_init.md) yourself before you do any of these things.

OpenDoors provides you with a C/C++ structure, by the name of od_control, which allows you to access all the available information about the user who is on-line, the system your door is running on, and also allows you to adjust various OpenDoors parameters. Depending on what BBS system your door is running under, the actual information available from the od_control structure will vary. For more information on the od_control structure, see the section on the control structure, beginning on page 148.

Once the door has initialized itself, it will then begin communications with the user who is online. OpenDoors takes care of all communications, through its various input and display functions. When the door has finished, it will then write any information that has changed back to the door information file (if applicable), finish communicating with the modem, and return to the BBS. In OpenDoors, these shut-down operations are automatically performed you call the [`od_exit()`](../reference/api/od_exit.md) function. This function will terminate the door's activity, OPTIONALLY hang up on the user (allowing you to provide either return to BBS or logoff options for exiting), and then exit with the specified errorlevel.

One other important OpenDoors function that you should be aware of is the [`od_kernel()`](../reference/api/od_kernel.md) function. [`od_kernel()`](../reference/api/od_kernel.md) is the central OpenDoors control function, and is responsible for much of OpenDoor's updating of the status line, monitoring the carrier detect and user timeout status, responding to sysop function keys, and so on. The [`od_kernel()`](../reference/api/od_kernel.md) function is called automatically by OpenDoors, within the other OpenDoors functions. As a result, since most door programs will call some OpenDoors function on a regular basis, you will most often have no need to call the [`od_kernel()`](../reference/api/od_kernel.md) function yourself. However, if your door is going to perform some action, such as updating data files, during which it will not call any OpenDoors function for more than a few seconds, you should then call the [`od_kernel()`](../reference/api/od_kernel.md) function yourself. For more information on the [`od_kernel()`](../reference/api/od_kernel.md) function, see page 97.

For more information on the functions available from OpenDoors, or the control structure, see the corresponding sections in this manual.

### Tour Of A Sample Door Program: "Ex_Vote"

One of the best ways to see how OpenDoors works, and the potential that it has, is to look at the example programs included in the OpenDoors package. A brief description of each of these programs can be found on page 38. This section takes a closer look at one of the example programs, EX_VOTE.C. Unlike our simple example in the previous section, EX_VOTE.C is a much more complicated program, taking advantage of many of the advanced features of OpenDoors. Even if you do not understand everything that EX_VOTE.C does, you should be able to make use of various elements demonstrated here, in your own programs.

The OpenDoors package includes a two compiled versions of EX_VOTE. VOTEDOS.EXE is a plain-DOS program which can run under DOS, Windows or OS/2. VOTEWIN.EXE was compiled using the Win32 version of OpenDoors, and so it runs only on Windows 95/NT. The OpenDoors package also contains a sample door information file, DORINFO1.DEF. You can use this file to test any doors in local mode. If you wish to manually create your own DORINFO1.DEF file, you can do so very easily. The DORINFO1.DEF door information file is a simple text file which lists a different piece of information on each line, in the following format:

```c
+----------------------------------------------------------+
| LINE NUMBER | DESCRIPTION            | EXAMPLE           |
+-------------+------------------------+-------------------|
|     1       | Name of the BBS        | MY OWN BBS        |
|     2       | Sysop's first name     | BRIAN             |
|     3       | Sysop's last name      | PIRIE             |
|     4       | Com Port modem is on   | COM0              |
|     5       | Baud rate, etc.        | 0 BAUD,N,8,1      |
|     6       | Unused                 | 0                 |
|     7       | User's first name      | JOHN              |
|     8       | User's last name       | PUBLIC            |
|     9       | Caller's location      | OTTAWA, ON        |
|     10      | ANSI mode (0=off, 1=on)| 1                 |
|     11      | User's security level  | 32000             |
|     12      | User's time left       | 60                |
+----------------------------------------------------------+
```

Feel free to make any changes you wish to EX_VOTE.C, and recompile it. One of the most effective and enjoyable ways to learn OpenDoors is by experimenting. If you are a registered owner of OpenDoors, you may even distribute your own versions of this door. Also, you may find that EX_VOTE.C serves as a good framework for building your own door programs.

The EX_VOTE.C door behaves similarly to most other door programs, and will have a fair bit in common with any other door you write in OpenDoors. What you see in the output window is identical to what a remote user will be seeing. If the user has ANSI, AVATAR or RIP mode turned on, you will see the same colors as they do, and if they have screen clearing turned on, your screen will be cleared when theirs is. The status line at the bottom of the window will list the name of the user currently on-line (if you are using the sample DORINFO1.DEF file, the user's name will be "The Sysop"), the user's location, and the user's baud rate (0 if the door is operating in local mode). The local display also shows how much time the user has left, whether the user has paged the system operator for a chat, and other information.

There are a number of special commands that are only available to the system operator on the local keyboard. These commands allow the system operator to hang up on the user, adjust the amount of time the user may remain online, enter chat mode with the user, enter a DOS shell (in the DOS version), and so on. In the DOS version, help on these commands is available on the status line by pressing the [F9] key. In the Windows version, these commands are listed on the menu that appears at the top of the window.

Now, let us take a closer look at the actual source code for the EX_VOTE.C door. If you have not already printed out a copy of this manual, and possibly the EX_VOTE.C file as well, it would probably be a good idea to do so now.

Notice that near the top of the program, along with all the standard header files, the OPENDOOR.H file is included. This file must be included in all programs written under OpenDoors. If you are placing the OPENDOOR.H file in the same directory as the door you are compiling, simply include the line:

```c
#include "opendoor.h"
```

in your program.

The main()/WinMain() function of the EX_VOTE.C program has a for(;;) loop that repeatedly displays the main menu, obtains a choice from the user and responds to the command, until the user chooses to exit the program. Before the main menu is displayed, the screen is cleared by calling [`od_clr_scr()`](../reference/api/od_clr_scr.md). The [`od_clr_scr()`](../reference/api/od_clr_scr.md) function will clear both the local and remote screens, but only if the user has screen clearing enabled. Refer to page 57 for information on how to force the screen to be cleared, regardless of the user's screen clearing setting. The main menu is displayed using the [`od_printf()`](../reference/api/od_printf.md) function, one of the most common OpenDoors functions you will use. Next, [`od_get_answer()`](../reference/api/od_get_answer.md) is used to obtain a menu choice from the user from the specified set of keys. Next, a switch() statement is used to respond to the user's command appropriately. If the user presses the P key to page the system operator, [`od_page()`](../reference/api/od_page.md) is called. If the user chooses to return to the BBS, [`od_exit()`](../reference/api/od_exit.md) is called to terminate OpenDoor's activities and return control to the BBS. The FALSE parameter passed to [`od_exit()`](../reference/api/od_exit.md) indicates that OpenDoors should not disconnect (hangup) before exiting. If the user chooses to log off, EX_VOTE.C first confirms this action with the user, and then calls [`od_exit()`](../reference/api/od_exit.md) with the TRUE parameter. The numerical parameter passed to [`od_exit()`](../reference/api/od_exit.md) sets the errorlevel that OpenDoors will exit with.

In its ChooseQuestion() function, EX_VOTE.C uses the OpenDoors function [`od_get_key()`](../reference/api/od_get_key.md). This function is similar to the [`od_get_answer()`](../reference/api/od_get_answer.md) function that we have already seen. However, unlike [`od_get_answer()`](../reference/api/od_get_answer.md) which will wait until the user presses some key from the list of possibilities you provide, [`od_get_key()`](../reference/api/od_get_key.md) will allow the user to press any key. [`od_get_key()`](../reference/api/od_get_key.md) accepts a single parameter. If this parameter is TRUE, [`od_get_key()`](../reference/api/od_get_key.md) will wait for the user to press a key before returning. If this parameter is FALSE, [`od_get_key()`](../reference/api/od_get_key.md) will return immediately with a value of 0 if there are no keys waiting in the inbound buffer, and returning the next key if there are characters waiting.

In a number of places, EX_VOTE.C also uses the [`od_input_str()`](../reference/api/od_input_str.md) function. Unlike [`od_get_key()`](../reference/api/od_get_key.md) and [`od_get_answer()`](../reference/api/od_get_answer.md) which return a single character, [`od_input_str()`](../reference/api/od_input_str.md) allows the user to input and edit a string of many characters. You will only receive the string entered by the user after they press the enter key. [`od_input_str()`](../reference/api/od_input_str.md) accepts four parameters: the string where the user's input should be stored, the maximum number of characters to input, the minimum character value to accept and the maximum character value to accept.

Another new feature of OpenDoors that is used by EX_VOTE.C is the OpenDoors control structure, od_control. This global structure is documented in chapter 5 of this manual. The OpenDoors control structure allows you to access a wide variety of information about the user who is currently online, the BBS system your program is running on, and also allows you to control various OpenDoors settings. For example, EX_VOTE.C compares the current user name (od_control.od_user_name) with the name of the system operator (od_control.od_sysop_name) to determine whether it is the system operator who using the program.

EX_VOTE.C uses two data files, the first of which contains a record for every user, and the second of which contains a record for every question. EX_VOTE.C accesses these data files in a controlled manner in order to permit the program to be running simultaneously on multiple lines on a multi-node BBS system. When EX_VOTE.C needs to update a data file, it opens it for exclusive access, so that only one node can access the file at any given time. Since the data file could have been changed by another node since the time that EX_VOTE.C last read the file, it always reads a record, makes changes to it and then re-writes the record while it has the file open for exclusive access. It then closes the file as soon as possible after opening the file, in order to permit other nodes to once again access the file. Because EX_VOTE.C keeps track of which questions each user has voted on, along with the questions and results of voting on each question, its data file format is more complex than many door programs (although not as complex as others).

EX_VOTE.C also uses color. One of the easiest ways to use different colors in an OpenDoors program is to use the OpenDoor's print color-setting extensions. You can change the color of text display at any point in an [`od_printf()`](../reference/api/od_printf.md) format string using by enclosing the name of new display color in back quote characters (`, not '). For example:

od_printf("`red`This is in red `green`This is green\n\r");

Would cause the words "This is in red" to be displayed in red, and the words "This is in green" to be displayed in green.

EX_VOTE.C also takes advantage of a number of OpenDoors capabilities that you can optionally choose to include in your door programs. You will notice that there are a number of new lines at the beginning of the main() function, all of which change settings in the OpenDoors control structure. The line:

```c
od_control.od_config_file = INCLUDE_CONFIG_FILE;
```

causes the OpenDoors configuration file system to be included in your program. Using this system, OpenDoors automatically reads a configuration file that can be used by the system operator to change various program settings. Refer to the included door.cfg file for an example OpenDoors configuration file. In addition to the configuration file settings automatically supported by the configuration file system, you can also add your own configuration file settings. To do this, you simply supply OpenDoors with a callback function that it will call whenever it encounters an unrecognized keyword in the configuration file. The line:

```c
od_control.od_config_function = CustomConfigFunction;
```

Causes OpenDoors to call the function CustomConfigFunction() in EX_VOTE.C for this purpose. You will notice that the CustomConfigFunction() receives two parameters - the first is the unrecognized keyword, and the second is any parameters that follow the keyword in the configuration file. EX_VOTE.C checks for two special configuration file lines - one to set whether or

not users can add questions, and one to set whether or not users can view the results of a question before voting on it.

The next line in the main() function,

```c
od_control.od_mps = INCLUDE_MPS;
```

causes the OpenDoors "Multiple Personality System" to be included in program. This allows the sysop to choose from a number of status line / sysop function key "personalities" that mimic a number of different BBS systems, using the Personality setting in the configuration file.

The line:

```c
od_control.od_logfile = INCLUDE_LOGFILE;
```

causes the OpenDoors log file system to be included in the program. The OpenDoors log file system automatically records the date and time of program startup, exit and other major actions in the specified file. EX_VOTE.C also writes its own log file entries by calling the [`od_log_write()`](../reference/api/od_log_write.md) function.

EX_VOTE.C also provides the ability for the sysop to provide their own ASCII/ANSI/AVATAR/RIP files to be displayed in place of the normal main menu. EX_VOTE.C uses the [`od_hotkey_menu()`](../reference/api/od_hotkey_menu.md) function to display a VOTE.ASC/.ANS/.AVT/.RIP file for the main menu, if such a file exists. If the file is not available, the normal EX_VOTE.C menu is used instead. The [`od_hotkey_menu()`](../reference/api/od_hotkey_menu.md) function will automatically select the appropriate file (.ASC/.ANS/.AVT/.RIP) for the current display mode, and the user is able to make a menu choice at any time. If a menu choice is made before the menu is entirely displayed, the function will stop displaying the menu and return immediately.

### Other Example Programs Included With Opendoors

In addition to the EX_VOTE.C program, which is discussed in detail in the previous section, a number of other example programs are included with OpenDoors. These programs help to demonstrate what is possible with OpenDoors. They can also serve as excellent tools to help you learn OpenDoors. In addition, you are free to include any portions of any of these example programs in your own programs. Below is a summary of each of these example programs:

#### `EX_HELLO.C`

EX_HELLO.C     This an example of a very simple door program that displays a short message and prompts for the user to press a key. After the user presses a key, the door exits and control is returned to the main BBS software. Despite the fact that it only consists of a few lines of code, EX_HELLO remains a fully functional door program. For information on compiling an OpenDoors door program, see the section that begins on page 22.

#### `EX_CHAT.C`

EX_CHAT.C      This program is an example of a multi-window full-screen chat door written with OpenDoors. EX_CHAT demonstrates the ease of using sophisticated ANSI / AVATAR / RIP terminal features within OpenDoors programs. For instructions on how to compile this program, see the section that begins on page 22.

This program create two windows on the screen, separated by a bar with user name / sysop name information. This program permits communication between the local sysop and remote user by displaying the text typed by the user in one window, and the text typed by the sysop in the other window. When either person's typing reaches the bottom of the window, the contents of the window is scrolled up to provide more room for typing. Words are also wrapped when either typist reaches the end of a line. The advantage of a split-screen chat program is that it permits both sysop and user to type at the same time without difficulty. The chat function automatically invokes OpenDoor's internal chat mode if ANSI, AVATAR or RIP modes are not available. The display colors, window sizes and locations, and distance to scroll a window's contents are configurable by setting the appropriate variables, below. When the Sysop invokes a DOS shell, a pop-up window is displayed to indicate to the user that the door program has been suspended.

The chat feature of this program can also be easily integrated into other doors you write, and may be used to replace the existing OpenDoors line-oriented chat system.

#### `EX_MUSIC.C`

EX_MUSIC.C     This example door demonstrates how to play "ANSI" music and sound effects in an OpenDoors door. Included in this program is a function to send "ANSI" music to the remote system, and a function to text the remote system's ability to play "ANSI" music. You may use both of these functions in your own doors, if you wish to add music or sound effect capabilities. This program can be compiled by following the instructions that begin on page 22.

#### `EX_SKI.C`

EX_SKI.C       This is a simple but addictive online game that is written using OpenDoors. In this action game, the player must control a skier through a downhill slalom course. The user may turn the skier left or right, and the game ends as soon as the player skis outside the marked course. The game begins at an easy level, but quickly becomes more and more difficult as the course to be navigated becomes more and more narrow. The game maintains a list of players with high scores, and this list may be viewed from the main menu.

#### `EX_VOTE.C`

EX_VOTE.C      The EX_VOTE.C file contain the source code for the Vote example door, as is described beginning on page 38. The Vote example door allows users to vote on up to 200 different "polls", view the results of voting on each question, and optionally add their own questions for other users to answer.
