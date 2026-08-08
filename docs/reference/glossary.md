# Glossary

Terminology used by OpenDoors and the BBS software environment.

## Detailed reference

Definitions are retained in the context of the original manual. In particular, operating-system and modem examples reflect that period.

ANSI           ANSI is an acronym for "American National Standards Institute". One of the standards approved by ANSI is a terminal display protocol which allows (in this case), BBS software to perform certain display functions such as changing the color of displayed text, or moving the location of the cursor on the screen. The majority, though not all, BBS users use terminal software with ANSI capabilities. Any users that do not have graphics display capabilities, will be using ASCII mode, instead. The ANSI terminal protocol is sometimes referred to as "ANSI graphics". It is graphic in the sense that it provides more visual control than an ASCII TTY terminal does, but does not imply any support for bit-mapped nor vector graphics. Compare ASCII and AVATAR.

API            API is an acronym for "Application Program(er) Interface". An API is a set of well documented functions, variables and data types that you can use to access certain services from your program. When you write any C program that uses standard C library functions such as fopen() or strcpy(), you are using a sort of API. When you use OpenDoors functions such as [`od_printf()`](api/od_printf.md) or [`od_get_key()`](api/od_get_key.md), you are using functions that are part of the OpenDoors API. Operating systems provide their own APIs that allow programs to gain access to operating system features such as screen display, file I/O and communications. The API provided by Microsoft Windows 95 and Windows NT is called the Win32 API.

ASCII          ASCII (pronounced "ass-key") is an acronym for "American Standard Code for Information Interchange", and is a definition of a set of 128 letters, number and symbols, which can be displayed by computer systems. Also, when used within the domain of BBS software, ASCII mode is often used to refer to the lack of any more advanced display capabilities, such as ANSI or AVATAR. When ASCII mode is used, characters can only be displayed in standard Teletype (TTY) fashion, one after another. Also, color and cursor positioning functions are not available in ASCII mode. Compare ANSI and AVATAR.

AVATAR         AVATAR is an acronym for "Advanced Video Attribute Terminal Assembler and Recreator". AVATAR is a graphics display protocol, similar to ANSI. Like ANSI-graphics, AVATAR graphics allow functions such as cursor positioning, and color changing. However, AVATAR also offers many capabilities not available from ANSI, and performs the same functions as ANSI much more quickly. AVATAR graphics is less common than both ANSI or ASCII, but is becoming more popular as time goes by. Compare ASCII and ANSI.

BAUD           "baud" or "baud rate" are generally used as a synonym for "BPS".

BPS            BPS is an acronym for "Bits Per Second", and refers to the rate at which data is being sent over a communications medium. There are two important BPS rates which are relevant to OpenDoors. The serial port BPS rate (also called the DCE rate) is the speed at which the computer is communicating with the local modem. The connect speed, on the other hand, is the speed at which the local modem is communicating with the remote modem. The serial port speed must be at least as fast as the connection speed. Often the serial port speed will be locked at a fixed speed that is higher than the fastest possible connection speed of the modem. For example, the serial port might be locked at a speed of 38400 BPS, while the modem could be connected at 28,800, 14,400 or slower speeds. OpenDoors usually needs to know the serial port BPS rate in order to function correctly (as stored in [`od_control.baud`](control/connection.md#baud)). Under certain situations, OpenDoors will also be able to report the connection speed to you (as stored in [`od_control.od_connect_speed`](control/connection.md#od_connect_speed)), although OpenDoors does never requires this information to operate.

BIT-MAPPED     As with Boolean values, described below, bit mapped flags FLAGS          are used to indicate whether or not various conditions exist. (For example, whether or not a certain setting is enabled, or whether or not a particular event has occurred.) However, unlike Boolean variables, a single bit-mapped flag represents more than one of these TRUE/FALSE values. In fact, each bit (BInary Digit), which makes of the variable can be used to represent a separate TRUE/FALSE state. (ie, each bit maps to a particular piece of information, and hence the term "Bit Map").

For an example of using bit-mapped flags, let us take a case of a single "unsigned char" which contains three independent TRUE/FALSE values. We will call this variable user_info, and it will indicate whether or not a user has ANSI graphics, whether or not the user has screen clearing turned on, and whether or not the user has end-of-page "more" prompts enabled. Internally, the bits of the user_info variable will be as follows:

```c
      Bit:  7 6 5 4 3 2 1 0
                      | | |
                      | | +--- ANSI Graphics
                      | +----- Screen Clearing
                      +------- More prompts
```

In this case, we will have three constants which we define in order to simplify access to these bit-mapped flags, as follows:

#define ANSI_GRAPHICS        0x01 #define SCREEN_CLEARING      0x02 #define MORE_PROMPTS         0x04

Note that normally within OpenDoors, these constants will be defined for you, and you will have no need to know what their values are, nor in which bit which piece of information is stored.

Using bit-mapped flags, you are able to set or clear any of the individual flags, and check whether any of the flags are set, using these simple methods: (Not that a set flag is the equivalent of a Boolean value of "True", and a cleared flag is the equivalent of a Boolean value of "False".)

Set Flag:      variable |= FLAG_CONSTANT; Clear Flag:    variable &=~ FLAG_CONSTANT; Test Flag:     variable & FLAG_CONSTANT

Where "variable" is the name of the bit-mapped flag variable, and "FLAG_CONSTANT" is the pre-defined constant for the individual setting. To return to our example, you could turn on the user's ANSI graphics setting by using the line:

user_info |= ANSI_GRAPHICS;

and to turn off screen clearing you would:

user_info &=~ ANSI_GRAPHICS;

To perform an action (such as waiting for the user to press [Return]/[Enter]) only if "More" prompts are enabled, you would:

if(user_info & MORE_PROMPTS) { ...            /* Whatever you want */ }

BOOLEAN        Many of the variables used within OpenDoors contain a VALUES         "Boolean Value". A Boolean value is a two-state variable, who's states are referred to as "True" and "False'. If the variable contains a value of "True", it indicates that a certain condition is so, and if it contains a value of "False", it indicates that the condition is not so. For example, a Boolean variable "wait" might be used to indicate whether or not OpenDoors should wait for the user to press a key, or continue without waiting. In this case, a value of "True" would indicate that OpenDoors should wait, and a value of "False" would indicate that it should not wait.

Note that in the C programming language, there is no actual Boolean variable type - usually a char or an int are used to store Boolean values.

The constants TRUE and FALSE, as defined in the OPENDOOR.H file, are used to represent the two states of a Boolean value. Thus, to set a Boolean variable "wait" to the value of "True", you would use this line:

```c
wait=TRUE;
```

and to set the variable "wait" to "False", you would:

```c
wait=FALSE;
```

However, you SHOULD NOT test whether a Boolean variable is "True" or "False" by using the C compare (==) operator, as the value "True" will not always be the same numerical value. (Actually, the TRUE constant represents just one of many possible numerical values for "True"). Instead, to perform an action of the "wait" Boolean variable is "True", you would:

```c
if(wait)
{
     ...        /* Whatever you want */
}
```

and to perform an action if the "wait" Boolean variable is "False', you would:

```c
if(!wait)
{
     ...       /* Whatever you want */
}
```

For interest sake, Boolean values are named after the 19th century English mathematician, who studied formal logic, and created Boolean algebra - an algebra which deals with TRUE and FALSE values.

BPS            BPS is an acronym for "Bits Per Second". For our purposes here, the terms BPS and BAUD refer to the same thing.

CARRIER        The term "Carrier" or "Carrier Detect" refers to a signal which DETECT         most modems send to the computer, which indicates whether or not the modem is currently connected to (communicating with) another modem. The door driver module of OpenDoors, as with most other BBS software, uses the status of this carrier detect signal in order to know whether the user is still connected to the BBS computer. Thus, if the user hangs up, or if something goes wrong and the connection is lost, OpenDoors is able to detect this state, and exit to the BBS. The BBS will then also detect that the carrier signal has been "lost", and will reset itself, and then again be ready to accept calls.

CHAT MODE      The term "chat mode" refers to a means by which the sysop can communicate with a user of the BBS / door. During sysop chat, anything typed by the sysop will appear on the user's screen, and likewise, anything typed by the user will appear on the sysop's screen. Sysop chatting is available on both single and multi-line systems. Sysop chatting is initiated by the sysop, either at any time a user is online, or specifically in response to a sysop page.

COMPILE        "Compiling" refers to the process of converting the source code that you write for your program, into an executable file (such as a .EXE file) that an end user can use. The process of building an executable file is generally divided into two stages. In the first stage, called compiling, source files are converted to object files, often named .OBJ. In the second stage, called linking, one or more object files are combined, along with any library files, to produce the final executable file.

DLL            DLL is an acronym for "Dynamic Link Library". A dynamic link library is similar to a static library, in that it contains one or more functions that an application program can use. Unlike a static library, the code from a dynamic link library is not added to the program's executable file at link time. Instead, the dynamic link library exists as a separate file which must be loaded when the program is run. The Win32 version of OpenDoors resides in a DLL.

See also "Library".

DOOR           A "door" is a program that runs as part of a BBS system, but which is separate from the central BBS software (RemoteAccess, Maximus, QuickBBS, PC-Board, etc.) itself. A door provides additional features not built into the BBS software, such as on- line games, on-line shopping services, voting booths, match making systems, access to special files or messages, and much much more. Since the user also communicates with the door online, as they do with the BBS, it may not necessarily be obvious to the user that the door is even a separate entity from the central BBS software itself.

DOOR           Also referred to as a "drop file", "exit file", or "chain INFORMATION    file". The door information file is a file passed from the FILE           central BBS software to a door program, providing it with information about the user who is online, the BBS the door is running under, and the current modem connection. The door information file may also be used to pass changed information back to the BBS, such as the amount of time that the user has used in the door. OpenDoors takes care of all of the work involved in reading and writing the door information file for you, as described in the "Basics of Door Programming" section, in chapter 4. Examples of door information files supported by OpenDoors include: DOOR.SYS, EXITINFO.BBS, DORINFO?.DAT, SFDOORS.DAT, CALLINFO.BBS and CHAIN.TXT.

DTR            DTR is an acronym for "Data Terminal Ready". This is a signal that the computer sends to the modem, indicating that the computer is ready to send or receive information. Most modems are configured to hangup if the DTR signal is lowered. This is a convenient means of hanging up the modem, but cases problems under Windows 95, where the DTR signal is always lowered when a program closes the serial port.

ECHO           See "Local Echo".

FOSSIL         The FOSSIL driver, or simply FOSSIL, is a TSR program or DRIVER         device driver which OpenDoors can optionally make use of in order to communicate with the modem. The FOSSIL driver is loaded prior to starting up the BBS or your door, usually from the AUTOEXEC.BAT or CONFIG.SYS files. The two most commonly used FOSSIL drivers are X00 and BNU. (FOSSIL is an acronym for "Fido/Opus/SEAdog Standard Interface Layer", although it has now become the standard for nearly all BBS software.) FOSSIL drivers are also available for other specialized serial port hardware, such as the popular "DigiBoard" multi-port serial card.

IMPORT LIBRARY See "Library".

LIBRARY        A "library" or "library file" is a collection of precompiled functions and variables that can be used by other programs. All of the features, capabilities and functions of OpenDoors that you can make use of are contained within the OpenDoors library files. (Likewise, the C runtime library, consisting of the familiar functions such as fopen(), printf() and atoi(), is also contained within a library file.) For more information on the different OpenDoors library files, see the section that begins on page 22.

There are several different kinds of library files. A static library file is actually a collection of individual object files. When you compile a program that makes use of a static library file, only those portions of the library file that your program actually uses are linked into your program's executable (.EXE) file. Static library files can be identified by a .LIB extension. The DOS version of OpenDoors resides in a static library.

A dynamic link library, on the other hand, is not combined with the program's executable file. Instead dynamic link libraries exist in separate .DLL files that must also be present when the program is executed. The Win32 version of OpenDoors resides in a dynamic link library.

An import library is a small file that describes a dynamic link library. The most common way for a program to call functions in a dynamic link library requires that an import library be used a program link time.

See also "DLL".

LINK           "Linking" generally refers to the process of combining several object files into a final executable file, during which references to symbol names (such as [`od_printf()`](api/od_printf.md)) are resolved to the address of the corresponding object. See also "Compiling".

LOCAL MODE     The term "local mode" refers to a mode in which a BBS system or door program may operate. In local mode, the BBS/door behave as they would if a user were connected via modem to the BBS, except that all display and input is done simply on the BBS software, but not through the modem. Local mode allows the sysop or another person with direct access to the BBS computer to use the BBS/door software, either for their own user, or for testing that the software is running correctly. When programming door software, local mode can be very useful in testing and debugging the door, without requiring the door to be connected to a remote system. All doors written with OpenDoors automatically support local mode operation. Compare "Remote". LOCAL ECHO     The term "Local Echo" refers to a door displaying the same characters which are sent to the modem on the local screen ("Output Window"). This allows the sysop to view the same information that is sent to the user's system, in the same manner that it will appear on the user's screen.

LOCKED         (eg. "Locked Baud Rate", "Locked BPS Rate", "Locked Commport Speed", etc.) Usually, the communication port to which a modem is connected is set to transfer data at the same BPS rate as the rate at which the modem is communicating. However, many high speed modems allow very high speed data transfer by using built- in data compression methods. In this case, the actual rate of data transfer can easily exceed the true BPS rate of the connection. As a result, the BPS rate of the port is kept a single speed, faster than any of the true modem connections, in order to increase modem speed performance. This is referred to as locking the commport BPS rate. OpenDoors has full support for the use of locked BPS rates.

LOG FILE       A log file is a normal text file in which BBS software records all major activities that have taken place. As such, the log file permits the sysop, to review what activities have taken place on the BBS during the time which they have been away from the computer. A log file can be helpful in identifying system errors or crashes that have occurred, in alerting the sysop in the case that any users have been causing problems on the BBS, or in simply letting the sysop know who has called recently, and what when they did when they called.

MEMORY MODEL   C and C++ programs can be compiled under a variety of different memory models. Each memory model describes how data and program code are addressed in memory. When writing MS-DOS programs, you generally have the choice of six different memory models (named tiny, small, compact, medium, large and huge), each of which provides a different combination of the maximum sizes of data and code that your program may have. When writing Win32 programs, there is a single, flat 32-bit memory model that all programs use. This memory model allows a program to address (in theory) up to 4 gigabytes of data and code.

MODEM          A device connected to a computer which permits it to communicate with other computers, usually over standard telephone lines.

OBJECT FILE    An object file contains the compiled version of a source code file of a program. The source code file may be a .C file, .CPP file, .ASM file, .PAS file, .BAS file, or any number of other extensions associated with other programming languages. When any of these language's source code files are compiled, a .OBJ file is created containing information such as the executable code, and names of symbols (variables and functions) that are to be shared with other .OBJ files. In order to produce a .EXE file that may be executed, a process known as linking must be performed. During the link process, one or more object files composing your program are combined, along with the necessary code from any library files being used, in order to produce the final .EXE file.

ONLINE         In the case of BBS software and BBS door programs, the term online refers to the state of a user using the BBS. Usually, the user will be connected to the BBS from a remote location, using a modem. However, it is also possible that the user will be using the actual BBS computer, with the software operating in "local mode".

OUTPUT WINDOW  The local screen of the BBS on which BBS software is running is usually divided into two sections. At the bottom of the screen, there is often a one or two line status line, which displays information to the sysop about the BBS and the user who is currently online. The rest of the screen is usually an "output window", in which the information which is being displayed to the user, is also displayed on the local screen. In some cases, there will be no status line, in which case the entire screen will be the output window. Usually, the upper 23 lines of the screen in an OpenDoors door will be the output window, with the bottom two lines being the status line. However, it is possible to disable the OpenDoors status line, in which case the entire screen will be the output window. See also "Status Line"

PAGE           See "SYSOP PAGE"

PARAMETER      In the C programming language, many tasks are accomplished by calling functions. When a function is called, one or more pieces of information may be passed to a function, in the form of parameters. For example, a function used to set the foreground and background color of displayed text might accept two parameters, one for each of the two color settings. In this example, a function such as [`od_set_color()`](api/od_set_color.md), would be called as follows:

```c
od_set_color(D_GREEN,D_RED);
```

In this case, D_GREEN, the foreground color, is the first parameter, and D_RED, the background color, is the second parameter.

In C, parameters are enclosed in parentheses, ( and ), which are located after the name of the function to be called. Each parameter is then separated by a comma character. If a function does not accept any parameters, the parentheses will have nothing between them. (ie. [`od_clr_scr()`](api/od_clr_scr.md) ).

REGISTRATION   This is a demonstration version of OpenDoors, which may only be used under limited circumstances, for a limited period of time. If you wish to continue using OpenDoors after this "evaluation period", you must "register" it. For more information on registering OpenDoors, please see chapter 2 of this manual.

REMOTE         When used in reference to BBS software or door programs, the term remote is used to refer to a user or computer that is communicating with the BBS, for a distant location, by use of a modem. Compare "Local Mode"

RIP            "RIP", "RIPScrip" or "Remote Imaging Protocol" is a popular graphical terminal standard that is used with BBS systems. Unlike other terminal emulation standards, such as the ANSI and AVATAR modes supported by OpenDoors, RIP operates in bit mapped graphics mode, allowing features such as lines, circles and icons to be drawn on the remote screen. OpenDoors provides support for RIP graphics, although OpenDoors operates in text mode itself.

STATUS LINE    Usually, the bottom two lines of the screen, as displayed by an OpenDoors door, is devoted to a status line (although this status line may be turned off). This status line will display information about the user who is online, along with information about the current state of the BBS system, and a reference to the sysop function keys. See also "Local Window".

SYSOP          The term sysop is a short-form for "SYStem OPerator", and refers to the individual who is responsible for running and maintaining the BBS system. The sysop is usually the only person who has direct access to the local keyboard and computer on which the BBS, BBS utilities and BBS doors are running.

SYSOP CHAT     See "CHAT MODE".

SOURCE CODE    The term "source code" refers to the original file or files that where used to produce a library or executable program. The source code files contain the language statements or commands that are directly written by the programmer. These source code files are then compiled to produce an executable file that may be "run".

SYSOP PAGE     Sysop paging refers to the process whereby a user of the BBS system may call or page for the sysop's attention, when they wish to "chat" with the sysop, and can be thought of as being similar to the ringing of a telephone. When a user pages the sysop, the BBS system will produce some sort of sound, which the sysop may elect to respond to if they are within hearing range of the computer. The most common reasons for a user to page a sysop include the case that they are having difficulty with some aspect of the BBS, that they have a question, or if they are simply interested in having a friendly conversation with the sysop. Obviously, since the sysop may not wish to be disturbed by users paging at certain times (such as when they are in bed), most BBS software provides controls to allow you to control paging. These features might include the ability to set hours for each day of the week during which paging will be permitted, and the ability to temporarily override the ability of some or all callers to page the sysop.

USER           When applied to computers in general, the term user simply refers to any person using the computer hardware and software. However, when applied particularly to BBSes, the term user refers specifically to a person who calls the BBS, to carry out activities such as communicating via messages or chatting, uploading and downloading files, or using doors. Often, the term user is used in contrast with the term sysop. In this case, users are all of the people who call and user the BBS, other than the sysop themselves.

WIN32          Win32 is the name of the API that programs written to run under Microsoft Windows 95 and Microsoft Windows NT use to access operating system services. Win32 programs use a flat, 32-bit memory model and have access to advanced operating system services such as multithreading. Win32 programs cannot run under DOS nor OS/2. While some Win32 programs can run under Windows 3.x using the Win32s system, OpenDoors cannot since it requires multithreading services that are not provided by Win32s.
