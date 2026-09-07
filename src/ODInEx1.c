/* OpenDoors Online Software Programming Toolkit
 * (C) Copyright 1991 - 1999 by Brian Pirie.
 *
 * Oct-2001 door32.sys/socket modifications by Rob Swindell (www.synchro.net)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
 *        File: ODInEx1.c
 *
 * Description: Performs OpenDoors initialization and shutdown operations
 *              (od_init() and od_exit()), including drop file I/O. This
 *              module is broken into two files, ODInEx1.c and ODInEx2.c.
 *
 *   Revisions: Date          Ver   Who  Change
 *              ---------------------------------------------------------------
 *              Oct 13, 1994  6.00  BP   New file header format.
 *              Oct 19, 1994  6.00  BP   Default paging hours 8:00-22:00.
 *              Oct 21, 1994  6.00  BP   Further isolated com routines.
 *              Oct 29, 1994  6.00  BP   Properly read dorinfo?.def BBS name.
 *              Oct 31, 1994  6.00  BP   Only use dorinfo?.def /w exitinfo.bbs.
 *              Dec 09, 1994  6.00  BP   Standardized coding style.
 *              Dec 31, 1994  6.00  BP   Remove call to _mt_init().
 *              Dec 31, 1994  6.00  BP   Add call to ODPlatInit().
 *              Jul 30, 1995  6.00  BP   Split up od_init().
 *              Nov 11, 1995  6.00  BP   Removed register keyword.
 *              Nov 14, 1995  6.00  BP   Added include of odscrn.h.
 *              Nov 14, 1995  6.00  BP   New default for od_colour_char is 0.
 *              Nov 16, 1995  6.00  BP   Removed oddoor.h, added odcore.h.
 *              Nov 17, 1995  6.00  BP   Use new input queue mechanism.
 *              Nov 23, 1995  6.00  BP   Moved Pascal conversion to odutil.c.
 *              Dec 21, 1995  6.00  BP   Add ability to use already open port.
 *              Dec 22, 1995  6.00  BP   Added od_connect_speed.
 *              Dec 30, 1995  6.00  BP   Added ODCALL for calling convention.
 *              Dec 30, 1995  6.00  BP   Only use comm idle func under DOS.
 *              Jan 01, 1996  6.00  BP   Added od_disable_dtr.
 *              Jan 01, 1996  6.00  BP   Raise DTR after opening serial port.
 *              Jan 02, 1996  6.00  BP   Use printf() in ODInitError().
 *              Jan 03, 1996  6.00  BP   Display connect speed with %lu.
 *              Jan 19, 1996  6.00  BP   Don't use atexit() under Win32.
 *              Jan 19, 1996  6.00  BP   Make ODInitError() a shared function.
 *              Jan 20, 1996  6.00  BP   Prompt for user name if force_local.
 *              Jan 23, 1996  6.00  BP   Added od_exiting and OD_TEXTMODE.
 *              Jan 31, 1996  6.00  BP   Added DIS_NAME_PROMPT.
 *              Jan 31, 1996  6.00  BP   Support new SFDOORS.DAT format.
 *              Feb 02, 1996  6.00  BP   Added RA 2.50 EXITINFO.BBS support.
 *              Feb 06, 1996  6.00  BP   Added od_silent_mode.
 *              Feb 08, 1996  6.00  BP   Recognize SFSYSOP.DAT.
 *              Feb 09, 1996  6.00  BP   Correctly translate RA 2.x sex field.
 *              Feb 09, 1996  6.00  BP   Made default outbound buffer 3072.
 *              Feb 19, 1996  6.00  BP   Changed version number to 6.00.
 *              Feb 20, 1996  6.00  BP   Added bParsedCmdLine.
 *              Feb 21, 1996  6.00  BP   Don't override command line options.
 *              Feb 21, 1996  6.00  BP   Change od_always_clear default to on.
 *              Feb 23, 1996  6.00  BP   Changed default DTR disable string.
 *              Mar 03, 1996  6.10  BP   Begin version 6.10.
 *              Mar 06, 1996  6.10  BP   Added TRIBBS.SYS support.
 *              Mar 06, 1996  6.10  BP   Initial support for Door32 interface.
 *              Mar 13, 1996  6.10  BP   Added od_local_win_col.
 *              Mar 17, 1996  6.10  BP   Reset text color after local login.
 *              Mar 19, 1996  6.10  BP   MSVC15 source-level compatibility.
 *              Apr 08, 1996  6.10  BP   Store local login name in user_handle.
 *              Jan 13, 1997  6.10  BP   Fixes for Door32 support.
 *              Oct 19, 2001  6.20  RS   Added door32.sys and socket support.
 *              Aug 10, 2003  6.23  SH   *nix support
 */

#define BUILDING_OPENDOORS

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "OpenDoor.h"
#ifdef ODPLAT_NIX
#include <locale.h>
#include <limits.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif
#include "ODStr.h"
#include "ODCore.h"
#include "ODGen.h"
#include "ODScrn.h"
#include "ODVScrn.h"
#include "ODInQue.h"
#include "ODKrnl.h"
#include "ODMulti.h"
#include "ODInEx.h"
#include "ODRsv.h"
#include "ODUtil.h"
#ifdef ODPLAT_WIN32
#include "ODFrame.h"
#include "ODRes.h"
#endif /* ODPLAT_WIN32 */

/* Default size of local/remote combined keyboard buffer. */
#define DEFAULT_EVENT_QUEUE_SIZE    256


/* Local private helper functions. */
static void ODInitReadExitInfo(void);
static void ODInitPartTwo(void);
static BOOL ODInitReadSFDoorsDAT(void);
static void ODInitPartTwo(void);
static BOOL ODFramingIsEightBit(const char *pszFraming);


/* Private variables. */
static BYTE btRAStatusToSet = 0;
static BOOL bUserEightBit = FALSE;
static BOOL bTelnetSocket = FALSE;
#ifndef ODPLAT_WIN32
static BOOL bPreset = TRUE;
#endif /* !ODPLAT_WIN32 */
static char szIFTemp[256];
#ifdef ODPLAT_WIN32
static char szWindowsStartupUserName[sizeof(od_control.user_name)];
static BOOL bWindowsStartupCancelled;
#endif

/* Whether a drop-file framing field explicitly specifies eight data bits. */
static BOOL ODFramingIsEightBit(const char *pszFraming)
{
   while(isspace((unsigned char)*pszFraming))
      ++pszFraming;
   return(pszFraming[0] == '8' || strstr(pszFraming, ",8,") != NULL);
}

/* Configuration file keywords. */
static char *apszConfigText[] =
{
   "Node",
   "BBSDir",
   "DoorDir",
   "LogFileName",
   "DisableLogging",
   "SundayPagingHours",
   "MondayPagingHours",
   "TuesdayPagingHours",
   "WednesdayPagingHours",
   "ThursdayPagingHours",
   "FridayPagingHours",
   "SaturdayPagingHours",
   "MaximumDoorTime",
   "SysopName",
   "SystemName",
   "SwappingDisable",
   "SwappingDir",
   "SwappingNoEMS",
   "LockedBPS",
   "SerialPort",
   "CustomFileName",
   "CustomFileLine",
   "InactivityTimeout",
   "PageDuration",
   "ChatUserColour",
   "ChatSysopColour",
   "FileListTitleColour",
   "FileListNameColour",
   "FileListSizeColour",
   "FileListDescriptionColour",
   "FileListOfflineColour",
   "Personality",
   "NoFossil",
   "PortAddress",
   "PortIRQ",
   "ReceiveBuffer",
   "TransmitBuffer",
   "PagePromptColour",
   "LocalMode",
   "PopupMenuTitleColour",
   "PopupMenuBorderColour",
   "PopupMenuTextColour",
   "PopupMenuKeyColour",
   "PopupMenuHighlightColour",
   "PopupMenuHighKeyColour",
   "NoFIFO",
   "FIFOTriggerSize",
   "DisableDTR",
   "NoDTRDisable",
};

/* Custom drop file keywords. */
static char *apszConfigLines[] =
{
   "Ignore",
   "ComPort",
   "FossilPort",
   "ModemBPS",
   "LocalMode",
   "UserName",
   "UserFirstName",
   "UserLastName",
   "Alias",
   "HoursLeft",
   "MinutesLeft",
   "SecondsLeft",
   "ANSI",
   "AVATAR",
   "PagePausing",
   "ScreenLength",
   "ScreenClearing",
   "Security",
   "City",
   "Node",
   "SysopName",
   "SysopFirstName",
   "SysopLastName",
   "SystemName",
   "RIP",
};

/* Logfile messages. */
static char *apszLogMessages[] = 
{
   "Carrier lost, exiting door",
   "System operator terminating call, exiting door",
   "User's time limit expired, exiting door",
   "User keyboard inactivity time limit exceeded, exiting door",
   "System operator returning user to BBS, exiting door",
   "Exiting door with errorlevel %d",
   "Invoking operating system shell",
   "Returning from operating system shell",
   "User paging system operator",
   "Entering sysop chat mode",
   "Terminating sysop chat mode",
   "%s entering door",
   "Reason for chat: %s",
   "Exiting door",
};

/* Color name strings. */
static char *apszColorNames[] =
{
   "BLACK",
   "BLUE",
   "GREEN",
   "CYAN",
   "RED",
   "MAGENTA",
   "YELLOW",
   "WHITE",
   "BROWN",
   "GREY",
   "BRIGHT",
   "FLASHING",
};

/* Array of door information (drop) file names to search for. */
static char *apszDropFileNames[] = 
{
   "exitinfo.bbs",
   "dorinfo1.def",
   "chain.txt",
   "sfdoors.dat",
   "door.sys",
   "callinfo.bbs",
   "sfmain.dat",
   "sffile.dat",
   "sfmess.dat",
   "sfsysop.dat",
   "tribbs.sys",
   "door32.sys",
};

/* Array of door information (drop) file numbers 
 * (corresponding to apszDropFileNames) 
 */
enum {
   FOUND_EXITINFO_BBS,
   FOUND_DORINFO1_DEF,
   FOUND_CHAIN_TXT,
   FOUND_SFDOORS_DAT,
   FOUND_DOOR_SYS,
   FOUND_CALLINFO_BBS,
   FOUND_SFMAIN_DAT,
   FOUND_SFFILE_DAT,
   FOUND_SFMESS_DAT,
   FOUND_SFSYSOP_DAT,
   FOUND_TRIBBS_SYS,
   FOUND_DOOR32_SYS,
};

#define FOUND_NONE -1


/* Global variables. */
WORD wODNodeNumber = 65535U;
BOOL bIsCoSysop;
BOOL bIsSysop;
char *apszDropFileInfo[25];
BYTE btExitReason = 0;
DWORD dwForcedBPS = 1;
INT nForcedPort = -1;
DWORD dwFileBPS;
char szDropFilePath[120];
char szExitinfoBBSPath[120];
INT16 nInitialElapsed;
char *szOriginalDir = NULL;
BYTE btDoorSYSLock = 0;
time_t nStartupUnixTime;
INT16 nInitialRemaining;
BOOL bBBSDevSession;
BOOL bBBSDevDeadlineSet;
time_t nBBSDevDeadline;
INT nBBSDevComMethod;
BOOL bBBSDevOpenHandleSet;
char *pszBBSDevStorage;
char *pszBBSDevUserID;
BOOL bSysopNameSet = FALSE;
char szForcedSysopName[40];
BOOL bSystemNameSet = FALSE;
char szForcedSystemName[40];
BOOL bUserFull = FALSE;
BOOL bCalledFromConfig = FALSE;
tRA2ExitInfoRecord *pRA2ExitInfoRecord = NULL;
tExitInfoRecord *pExitInfoRecord = NULL;
tExtendedExitInfo *pExtendedExitInfo = NULL;
struct _pcbsys *pPCBoardSysRecord = NULL;
struct _userssyshdr *pUserSysHeader = NULL;
struct _userssysrec *pUserSysRecord = NULL;
BOOL bPreOrExit = FALSE;
BOOL bRAStatus;
BOOL bPromptForUserName = FALSE;
BOOL bParsedCmdLine = FALSE;
WORD wPreSetInfo = 0;
#ifdef ODPLAT_WIN32
tODThreadHandle hFrameThread;
#endif /* ODPLAT_WIN32 */

static char *
safe_strcpy(char *dst, const char *src, size_t sz)
{
	size_t len = strlen(src);
	if (len >= sz)
		len = sz - 1;
	memcpy(dst, src, len);
	dst[len] = 0;
	return dst;
}

static char *
safe_strcat(char *dst, const char *src, size_t sz)
{
	size_t olen = strlen(dst);
	size_t remain;
	size_t len;

	if (olen >= sz)
		return dst;
	remain = sz - olen;
	len = strlen(src);
	if (len >= remain)
		len = remain - 1;
	memcpy(&dst[olen], src, len);
	dst[olen + len] = 0;
	return dst;
}


/* ----------------------------------------------------------------------------
 * od_set_port()
 *
 * Records an explicit communications-port selection before initialization.
 *
 * Parameters: nPort - Zero-based port number.
 *
 *     Return: TRUE on success, or FALSE if the value is invalid or OpenDoors
 *             has already been initialized.
 */
ODAPIDEF BOOL ODCALL od_set_port(INT nPort)
{
   TRACE(TRACE_API, "od_set_port()");

   if(!ODSyncPublicCallAllowed()) return(FALSE);

   if(bODInitialized || nPort < 0 || nPort > 255)
   {
      od_control.od_error = ERR_PARAMETER;
      return(FALSE);
   }

   od_control.port = (INT16)nPort;
   nForcedPort = nPort;
   return(TRUE);
}

/* ----------------------------------------------------------------------------
 * od_get_user_8bit()
 *
 * Reports whether the caller's connection supports eight-bit character data.
 */
ODAPIDEF BOOL ODCALL od_get_user_8bit(void)
{
   TRACE(TRACE_API, "od_get_user_8bit()");

   if(!ODSyncPublicCallAllowed()) return(FALSE);
   return(bUserEightBit);
}

/* ----------------------------------------------------------------------------
 * od_get_user_id()
 *
 * Returns the opaque BBSDEV user key or a deterministic legacy identifier.
 * Legacy identifiers are <number>:<name>, with an empty number component
 * when the selected format does not supply one. The handle is used when the
 * real-name field is empty.
 */
ODAPIDEF const char * ODCALL od_get_user_id(void)
{
   static char szUserID[48];
   const char *pszName;
   BOOL bHasNumber = FALSE;
   unsigned nPosition = 0;

   szUserID[0] = '\0';
   if(!ODSyncPublicCallAllowed())
      return(szUserID);
   if(od_control.od_info_type == BBSDEVDRP)
      return(pszBBSDevUserID == NULL ? szUserID : pszBBSDevUserID);

   bHasNumber = TRUE;
   if(od_control.od_info_type < EXITINFO)
      bHasNumber = FALSE;
   if(od_control.od_info_type > DOOR32SYS)
      bHasNumber = FALSE;
   if(od_control.od_info_type == CALLINFO)
      bHasNumber = FALSE;
   if(od_control.od_info_type == DOORSYS_DRWY)
      bHasNumber = FALSE;
   if(od_control.od_info_type == CUSTOM)
      bHasNumber = od_control.user_num != 0;

   pszName = od_control.user_name[0] != '\0'
      ? od_control.user_name : od_control.user_handle;
   if(!bHasNumber && pszName[0] == '\0')
      return(szUserID);
   if(bHasNumber)
   {
      char achReverse[5];
      unsigned nDigits = 0;
      unsigned nValue = od_control.user_num;
      do
      {
         achReverse[nDigits++] = (char)('0' + nValue % 10);
         nValue /= 10;
      } while(nValue != 0);
      while(nDigits != 0)
         szUserID[nPosition++] = achReverse[--nDigits];
   }
   szUserID[nPosition++] = ':';
   while(*pszName != '\0')
      szUserID[nPosition++] = *pszName++;
   szUserID[nPosition] = '\0';
   return(szUserID);
}

/* ----------------------------------------------------------------------------
 * od_set_user_8bit()
 *
 * Records whether the caller's connection supports eight-bit character data.
 */
ODAPIDEF BOOL ODCALL od_set_user_8bit(BOOL bEightBit)
{
   TRACE(TRACE_API, "od_set_user_8bit()");

   if(!ODSyncPublicCallAllowed()) return(FALSE);
   bUserEightBit = bEightBit ? TRUE : FALSE;
   return(TRUE);
}

/* ----------------------------------------------------------------------------
 * ODBBSDevLanguageTagValid()                       *** PRIVATE FUNCTION ***
 *
 * Validates the RFC 5646 Language-Tag grammar, including grandfathered and
 * private-use tags and the uniqueness requirements for variants/extensions.
 */
static BOOL ODBBSDevLanguageTagValid(const char *pszLanguage)
{
   static const char * const apszGrandfathered[] = {
      "art-lojban", "cel-gaulish", "en-GB-oed", "i-ami", "i-bnn",
      "i-default", "i-enochian", "i-hak", "i-klingon", "i-lux",
      "i-mingo", "i-navajo", "i-pwn", "i-tao", "i-tay", "i-tsu",
      "no-bok", "no-nyn", "sgn-BE-FR", "sgn-BE-NL", "sgn-CH-DE",
      "zh-guoyu", "zh-hakka", "zh-min", "zh-min-nan", "zh-xiang"
   };
   size_t nPosition = 0;
   size_t nVariantStart = 0;
   unsigned nExtlangs = 0;
   unsigned nIndex;
   BOOL bFirst = TRUE;
   BOOL bExtlangOpen = FALSE;
   BOOL bScriptOpen = TRUE;
   BOOL bRegionOpen = TRUE;
   BOOL bHaveVariant = FALSE;
   BOOL bPrivateUse = FALSE;
   BOOL bPrivateValue = FALSE;
   BOOL bExtension = FALSE;
   BOOL bExtensionValue = FALSE;
   BYTE abSingletonSeen[36] = {0};

   if(pszLanguage == NULL || pszLanguage[0] == '\0')
      return(FALSE);
   for(nIndex = 0; nIndex < DIM(apszGrandfathered); ++nIndex)
   {
      if(stricmp(pszLanguage, apszGrandfathered[nIndex]) == 0)
         return(TRUE);
   }

   while(pszLanguage[nPosition] != '\0')
   {
      size_t nStart = nPosition;
      size_t nLength = 0;
      BOOL bAlpha = TRUE;
      BOOL bDigit = TRUE;

      while(pszLanguage[nPosition] != '\0'
         && pszLanguage[nPosition] != '-')
      {
         char current = pszLanguage[nPosition++];
         if((current >= 'A' && current <= 'Z')
            || (current >= 'a' && current <= 'z'))
         {
            bDigit = FALSE;
         }
         else if(current >= '0' && current <= '9')
            bAlpha = FALSE;
         else
            return(FALSE);
         if(++nLength > 8)
            return(FALSE);
      }
      if(nLength == 0)
         return(FALSE);
      if(pszLanguage[nPosition] == '-')
      {
         ++nPosition;
         if(pszLanguage[nPosition] == '\0')
            return(FALSE);
      }

      if(bFirst)
      {
         bFirst = FALSE;
         if(nLength == 1
            && (pszLanguage[nStart] == 'x' || pszLanguage[nStart] == 'X'))
         {
            bPrivateUse = TRUE;
            continue;
         }
         if(!bAlpha || nLength < 2)
            return(FALSE);
         bExtlangOpen = nLength <= 3;
         continue;
      }

      if(bPrivateUse)
      {
         bPrivateValue = TRUE;
         continue;
      }

      if(bExtension)
      {
         if(nLength >= 2)
         {
            bExtensionValue = TRUE;
            continue;
         }
         if(!bExtensionValue)
            return(FALSE);
         bExtension = FALSE;
      }

      if(bExtlangOpen)
      {
         if(nExtlangs < 3 && nLength == 3 && bAlpha)
         {
            ++nExtlangs;
            continue;
         }
         bExtlangOpen = FALSE;
      }

      if(bScriptOpen)
      {
         bScriptOpen = FALSE;
         if(nLength == 4 && bAlpha)
            continue;
      }
      if(bRegionOpen)
      {
         BOOL bRegion = FALSE;
         bRegionOpen = FALSE;
         if(nLength == 2 && bAlpha)
            bRegion = TRUE;
         if(nLength == 3 && bDigit)
            bRegion = TRUE;
         if(bRegion)
            continue;
      }

      {
         BOOL bVariant = nLength >= 5;
         if(nLength == 4 && pszLanguage[nStart] <= '9')
         {
            bVariant = TRUE;
         }
         if(bVariant)
         {
            if(bHaveVariant)
            {
               size_t nPrevious = nVariantStart;
               while(nPrevious < nStart)
               {
                  size_t nPreviousLength = 0;
                  size_t nCompare;
                  BOOL bSame = TRUE;
                  while(pszLanguage[nPrevious + nPreviousLength] != '-')
                  {
                     ++nPreviousLength;
                  }
                  if(nPreviousLength == nLength)
                  {
                     for(nCompare = 0; nCompare < nLength; ++nCompare)
                     {
                        if((((unsigned char)pszLanguage[nPrevious + nCompare])
                           & 0xdf)
                           != (((unsigned char)pszLanguage[nStart + nCompare])
                           & 0xdf))
                        {
                           bSame = FALSE;
                           break;
                        }
                     }
                     if(bSame)
                        return(FALSE);
                  }
                  nPrevious += nPreviousLength + 1;
               }
            }
            else
            {
               bHaveVariant = TRUE;
               nVariantStart = nStart;
            }
            continue;
         }
      }

      if(nLength == 1)
      {
         char chSingleton = pszLanguage[nStart];
         unsigned nSingleton;
         if(chSingleton == 'x' || chSingleton == 'X')
         {
            bPrivateUse = TRUE;
            bPrivateValue = FALSE;
            continue;
         }
         if(chSingleton <= '9')
            nSingleton = (unsigned)(chSingleton - '0');
         else
         {
            chSingleton = (char)(chSingleton & 0xdf);
            nSingleton = (unsigned)(chSingleton - 'A') + 10;
         }
         if(abSingletonSeen[nSingleton])
            return(FALSE);
         abSingletonSeen[nSingleton] = TRUE;
         bExtension = TRUE;
         bExtensionValue = FALSE;
         continue;
      }
      return(FALSE);
   }

   if(bPrivateUse && !bPrivateValue)
      return(FALSE);
   if(bExtension && !bExtensionValue)
      return(FALSE);
   return(TRUE);
}

/* ----------------------------------------------------------------------------
 * ODBBSDevCopyText()                                *** PRIVATE FUNCTION ***
 *
 * Copies UTF-8 display text into a fixed legacy field without leaving a
 * partial multibyte character at the end of the destination.
 */
static void ODBBSDevCopyText(char *pszDestination, size_t nDestination,
   const char *pszSource)
{
   size_t nCopy = 0;

   if(nDestination == 0)
      return;
   while(pszSource[nCopy] != '\0' && nCopy + 1 < nDestination)
   {
      pszDestination[nCopy] = pszSource[nCopy];
      ++nCopy;
   }
   if(pszSource[nCopy] != '\0'
      && ((unsigned char)pszSource[nCopy] & 0xc0) == 0x80)
   {
      while(nCopy != 0
         && ((unsigned char)pszSource[nCopy] & 0xc0) == 0x80)
      {
         --nCopy;
      }
   }
   pszDestination[nCopy] = '\0';
}

/* ----------------------------------------------------------------------------
 * ODBBSDevAbsolutePath()                            *** PRIVATE FUNCTION ***
 */
static BOOL ODBBSDevAbsolutePath(const char *pszPath)
{
   if(pszPath == NULL)
      return(FALSE);
   if(pszPath[0] == '\0')
      return(FALSE);
#ifdef ODPLAT_NIX
   return(pszPath[0] == '/');
#else
   if((pszPath[0] >= 'A' && pszPath[0] <= 'Z')
      || (pszPath[0] >= 'a' && pszPath[0] <= 'z'))
   {
      if(pszPath[1] == ':')
      {
         if(pszPath[2] == '\\')
            return(TRUE);
         if(pszPath[2] == '/')
            return(TRUE);
      }
   }
   if(pszPath[0] == '\\' || pszPath[0] == '/')
   {
      if(pszPath[1] == '\\')
         return(TRUE);
      if(pszPath[1] == '/')
         return(TRUE);
   }
   return(FALSE);
#endif
}

#define BBSDEV_REJECT() do { bValid = FALSE; goto finished; } while(0)

/* ----------------------------------------------------------------------------
 * ODInitReadBBSDevDropFile()                         *** PRIVATE FUNCTION ***
 *
 * Reads and validates the version 1 core of BBSDEV.DRP, then maps the fields
 * represented by the legacy OpenDoors control structure. Newer version 1
 * minor revisions may append fields, which are ignored.
 */
BOOL ODInitReadBBSDevDropFile(const char *pszPath)
{
   FILE *pfFile = NULL;
   char *pszFile = NULL;
   char *apszLine[19];
   size_t nCapacity = 512;
   size_t nSize = 0;
   size_t nIndex;
   unsigned nLines = 0;
   int ch;
   BOOL bValid = TRUE;
   BOOL bVersionZero;
   DWORD dwWidth = 0;
   DWORD dwHeight = 0;
   DWORD_PTR dwNativeValue = 0;
   DWORD_PTR dwNativeMaximum = (DWORD_PTR)-1;
   int nMode = kComMethodUnspecified;
   BOOL bLocal = FALSE;
   BOOL bHasNativeValue = FALSE;
   BOOL bNamedSysop = FALSE;
   BOOL bNamedCoSysop = FALSE;

   if(pszBBSDevStorage != NULL)
   {
      free(pszBBSDevStorage);
      pszBBSDevStorage = NULL;
      pszBBSDevUserID = NULL;
   }

   if(!ODBBSDevAbsolutePath(pszPath))
      return(FALSE);
#ifdef ODPLAT_NIX
   dwNativeMaximum = (DWORD_PTR)INT_MAX;
#endif

   pfFile = fopen(pszPath, "rb");
   if(pfFile == NULL)
      return(FALSE);
   pszFile = (char *)malloc(nCapacity);
   if(pszFile == NULL)
      BBSDEV_REJECT();

   while((ch = fgetc(pfFile)) != EOF)
   {
      if(ch == 0)
         BBSDEV_REJECT();
      if(nSize + 1 >= nCapacity)
      {
         char *pszLarger;
         size_t nLarger;
         if(nCapacity >= 4096)
            BBSDEV_REJECT();
         nLarger = nCapacity * 2;
         pszLarger = (char *)realloc(pszFile, nLarger);
         if(pszLarger == NULL)
            BBSDEV_REJECT();
         pszFile = pszLarger;
         nCapacity = nLarger;
      }
      pszFile[nSize++] = (char)ch;
   }
   if(ferror(pfFile) || nSize == 0 || pszFile[nSize - 1] != '\n')
      BBSDEV_REJECT();
   pszFile[nSize] = '\0';

   if(nSize >= 3)
   {
      if((unsigned char)pszFile[0] == 0xef)
      {
         if((unsigned char)pszFile[1] == 0xbb)
         {
            if((unsigned char)pszFile[2] == 0xbf)
               BBSDEV_REJECT();
         }
      }
   }

   apszLine[0] = pszFile;
   for(nIndex = 0; nIndex < nSize; ++nIndex)
   {
      if(pszFile[nIndex] == '\r')
      {
         if(pszFile[nIndex + 1] != '\n')
            BBSDEV_REJECT();
      }
      else if(pszFile[nIndex] == '\n')
      {
         if(nIndex != 0 && pszFile[nIndex - 1] == '\r')
            pszFile[nIndex - 1] = '\0';
         else
            pszFile[nIndex] = '\0';
         ++nLines;
         if(nLines < 19)
            apszLine[nLines] = pszFile + nIndex + 1;
      }
   }
   if(nLines < 19)
      BBSDEV_REJECT();

   /* Validate UTF-8, prohibited controls, and field-edge whitespace. */
   for(nIndex = 0; nIndex < 19; ++nIndex)
   {
      const unsigned char *p = (const unsigned char *)apszLine[nIndex];
      DWORD dwFirst = 0;
      DWORD dwLast = 0;
      BOOL bFirst = TRUE;
      while(*p != '\0')
      {
         DWORD dwCode;
         unsigned nTrail;
         if(*p < 0x80)
         {
            dwCode = *p++;
            nTrail = 0;
         }
         else if(*p >= 0xc2 && *p <= 0xdf)
         {
            dwCode = *p++ & 0x1f;
            nTrail = 1;
         }
         else if(*p >= 0xe0 && *p <= 0xef)
         {
            unsigned char lead = *p;
            if(p[1] == '\0' || p[2] == '\0'
               || (p[1] & 0xc0) != 0x80 || (p[2] & 0xc0) != 0x80)
               BBSDEV_REJECT();
            if((lead == 0xe0 && p[1] < 0xa0)
               || (lead == 0xed && p[1] >= 0xa0))
               BBSDEV_REJECT();
            dwCode = *p++ & 0x0f;
            nTrail = 2;
         }
         else if(*p >= 0xf0 && *p <= 0xf4)
         {
            unsigned char lead = *p;
            if(p[1] == '\0' || p[2] == '\0' || p[3] == '\0')
               BBSDEV_REJECT();
            if((p[1] & 0xc0) != 0x80 || (p[2] & 0xc0) != 0x80
               || (p[3] & 0xc0) != 0x80)
               BBSDEV_REJECT();
            if((lead == 0xf0 && p[1] < 0x90)
               || (lead == 0xf4 && p[1] >= 0x90))
               BBSDEV_REJECT();
            dwCode = *p++ & 0x07;
            nTrail = 3;
         }
         else
         {
            BBSDEV_REJECT();
         }
         while(nTrail-- != 0)
         {
            if((*p & 0xc0) != 0x80)
               BBSDEV_REJECT();
            dwCode = (dwCode << 6) | (*p++ & 0x3f);
         }
         if(dwCode < 0x20 || (dwCode >= 0x7f && dwCode <= 0x9f))
            BBSDEV_REJECT();
         if(bFirst)
         {
            dwFirst = dwCode;
            bFirst = FALSE;
         }
         dwLast = dwCode;
      }
      if(!bFirst)
      {
         if(dwFirst == 0x20 || dwFirst == 0xa0 || dwFirst == 0x1680
            || (dwFirst >= 0x2000 && dwFirst <= 0x200a))
            BBSDEV_REJECT();
         if(dwFirst == 0x2028 || dwFirst == 0x2029 || dwFirst == 0x202f
            || dwFirst == 0x205f || dwFirst == 0x3000)
            BBSDEV_REJECT();
         if(dwLast == 0x20 || dwLast == 0xa0 || dwLast == 0x1680
            || (dwLast >= 0x2000 && dwLast <= 0x200a))
            BBSDEV_REJECT();
         if(dwLast == 0x2028 || dwLast == 0x2029 || dwLast == 0x202f
            || dwLast == 0x205f || dwLast == 0x3000)
            BBSDEV_REJECT();
      }
   }

   /* Version 1 accepts later minor revisions and their appended lines. */
   if(apszLine[0][0] != '1') BBSDEV_REJECT();
   if(apszLine[0][1] != '.') BBSDEV_REJECT();
   if(apszLine[0][2] == '\0') BBSDEV_REJECT();
   if(apszLine[0][2] == '0' && apszLine[0][3] != '\0') BBSDEV_REJECT();
   for(nIndex = 2; apszLine[0][nIndex] != '\0'; ++nIndex)
   {
      if(apszLine[0][nIndex] < '0') BBSDEV_REJECT();
      if(apszLine[0][nIndex] > '9') BBSDEV_REJECT();
   }
   bVersionZero = strcmp(apszLine[0], "1.0") == 0;
   if(bVersionZero && nLines != 19)
      BBSDEV_REJECT();

   /* Required text fields. */
   if(apszLine[3][0] == '\0' || apszLine[4][0] == '\0'
      || apszLine[11][0] == '\0' || apszLine[12][0] == '\0')
      BBSDEV_REJECT();
   if(apszLine[13][0] == '\0' || apszLine[14][0] == '\0'
      || apszLine[15][0] == '\0')
      BBSDEV_REJECT();

   /* Width and height are positive 16-bit decimal values. */
   for(nIndex = 0; apszLine[5][nIndex] != '\0'; ++nIndex)
   {
      if(apszLine[5][nIndex] < '0') BBSDEV_REJECT();
      if(apszLine[5][nIndex] > '9') BBSDEV_REJECT();
      dwWidth = dwWidth * 10 + (apszLine[5][nIndex] - '0');
      if(dwWidth > 65535L)
         BBSDEV_REJECT();
   }
   for(nIndex = 0; apszLine[6][nIndex] != '\0'; ++nIndex)
   {
      if(apszLine[6][nIndex] < '0') BBSDEV_REJECT();
      if(apszLine[6][nIndex] > '9') BBSDEV_REJECT();
      dwHeight = dwHeight * 10 + (apszLine[6][nIndex] - '0');
      if(dwHeight > 65535L)
         BBSDEV_REJECT();
   }
   if(dwWidth == 0 || dwHeight == 0 || apszLine[5][0] == '0'
      || apszLine[6][0] == '0')
   {
      BBSDEV_REJECT();
   }
   if((apszLine[7][0] != 'Y' && apszLine[7][0] != 'N')
      || apszLine[7][1] != '\0') BBSDEV_REJECT();
   if((apszLine[8][0] != 'Y' && apszLine[8][0] != 'N')
      || apszLine[8][1] != '\0') BBSDEV_REJECT();
   if((apszLine[18][0] != 'Y' && apszLine[18][0] != 'N')
      || apszLine[18][1] != '\0') BBSDEV_REJECT();

   /* CTerm revision: one or more canonical unsigned decimal components. */
   if(apszLine[9][0] != '\0')
   {
      BOOL bComponentStart = TRUE;
      for(nIndex = 0; apszLine[9][nIndex] != '\0'; ++nIndex)
      {
         char current = apszLine[9][nIndex];
         if(current == '.')
         {
            if(bComponentStart)
               BBSDEV_REJECT();
            bComponentStart = TRUE;
         }
         else if(current < '0')
            BBSDEV_REJECT();
         else if(current > '9')
            BBSDEV_REJECT();
         else if(bComponentStart && current == '0'
            && apszLine[9][nIndex + 1] != '\0'
            && apszLine[9][nIndex + 1] != '.')
         {
            BBSDEV_REJECT();
         }
         else
            bComponentStart = FALSE;
      }
      if(bComponentStart)
         BBSDEV_REJECT();
   }

   /* Restricted RFC 3339 UTC deadline and valid Gregorian calendar date. */
   if(apszLine[10][0] != '\0')
   {
      INT nYear, nMonth, nDay, nHour, nMinute, nSecond, nMonthDays;
      INT nCalendarYear, nCalendarMonth;
      long nDays = 0;
      long nSecondOfDay;
      BOOL bLeapYear = FALSE;
      if(apszLine[10][20] != '\0' || apszLine[10][4] != '-'
         || apszLine[10][7] != '-' || apszLine[10][10] != 'T')
         BBSDEV_REJECT();
      if(apszLine[10][13] != ':' || apszLine[10][16] != ':'
         || apszLine[10][19] != 'Z')
         BBSDEV_REJECT();
      for(nIndex = 0; nIndex < 20; ++nIndex)
      {
         if(nIndex == 4) continue;
         if(nIndex == 7) continue;
         if(nIndex == 10) continue;
         if(nIndex == 13) continue;
         if(nIndex == 16) continue;
         if(nIndex == 19) continue;
         if(apszLine[10][nIndex] < '0') BBSDEV_REJECT();
         if(apszLine[10][nIndex] > '9') BBSDEV_REJECT();
      }
      nYear = (apszLine[10][0] - '0') * 1000
         + (apszLine[10][1] - '0') * 100
         + (apszLine[10][2] - '0') * 10 + apszLine[10][3] - '0';
      nMonth = (apszLine[10][5] - '0') * 10 + apszLine[10][6] - '0';
      nDay = (apszLine[10][8] - '0') * 10 + apszLine[10][9] - '0';
      nHour = (apszLine[10][11] - '0') * 10 + apszLine[10][12] - '0';
      nMinute = (apszLine[10][14] - '0') * 10 + apszLine[10][15] - '0';
      nSecond = (apszLine[10][17] - '0') * 10 + apszLine[10][18] - '0';
      if(nMonth < 1 || nMonth > 12)
         BBSDEV_REJECT();
      if(nYear % 4 == 0)
      {
         if(nYear % 100 != 0)
            bLeapYear = TRUE;
         else if(nYear % 400 == 0)
            bLeapYear = TRUE;
      }
      nMonthDays = 31;
      if(nMonth == 2)
      {
         nMonthDays = bLeapYear ? 29 : 28;
      }
      else if(nMonth == 4) nMonthDays = 30;
      else if(nMonth == 6) nMonthDays = 30;
      else if(nMonth == 9) nMonthDays = 30;
      else if(nMonth == 11) nMonthDays = 30;
      if(nDay < 1 || nDay > nMonthDays || nHour > 23
         || nMinute > 59 || nSecond > 59)
      {
         BBSDEV_REJECT();
      }
      if(nYear >= 1970)
      {
         const BYTE abMonthDays[12] = {
            31,28,31,30,31,30,31,31,30,31,30,31
         };
         nCalendarYear = nYear - 1;
         nDays = (long)(nYear - 1970) * 365L;
         nDays += nCalendarYear / 4 - 1969 / 4;
         nDays -= nCalendarYear / 100 - 1969 / 100;
         nDays += nCalendarYear / 400 - 1969 / 400;
         for(nCalendarMonth = 1; nCalendarMonth < nMonth;
            ++nCalendarMonth)
         {
            nDays += abMonthDays[nCalendarMonth - 1];
            if(nCalendarMonth == 2 && bLeapYear)
               ++nDays;
         }
         nDays += nDay - 1;
         nSecondOfDay = (long)nHour * 3600L
            + (long)nMinute * 60L + nSecond;
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
         if(nDays > (2147483647L - nSecondOfDay) / 86400L)
            nBBSDevDeadline = (time_t)2147483647L;
         else
            nBBSDevDeadline = (time_t)(nDays * 86400L + nSecondOfDay);
#else
         nBBSDevDeadline = (time_t)nDays * (time_t)86400
            + (time_t)nSecondOfDay;
#endif
      }
      else
         nBBSDevDeadline = (time_t)0;
      bBBSDevDeadlineSet = TRUE;
   }
   else
   {
      bBBSDevDeadlineSet = FALSE;
   }

   /* OpenDoors can faithfully represent IBM437 and UTF-8 terminal data. */
   if(stricmp(apszLine[11], "IBM437") != 0
      && stricmp(apszLine[11], "UTF-8") != 0)
   {
      BBSDEV_REJECT();
   }

   if(!ODBBSDevLanguageTagValid(apszLine[12]))
      BBSDEV_REJECT();

   /* Access and node values are named roles or canonical uint64 strings. */
   if(strcmp(apszLine[16], "sysop") == 0)
      bNamedSysop = TRUE;
   else if(strcmp(apszLine[16], "cosysop") == 0)
      bNamedCoSysop = TRUE;
   else
   {
      size_t nDigits = 0;
      while(apszLine[16][nDigits] >= '0')
      {
         if(apszLine[16][nDigits] > '9') break;
         ++nDigits;
      }
      if(apszLine[16][nDigits] != '\0') BBSDEV_REJECT();
      if(nDigits == 0) BBSDEV_REJECT();
      if(nDigits > 1)
         if(apszLine[16][0] == '0') BBSDEV_REJECT();
      if(nDigits > 20) BBSDEV_REJECT();
      if(nDigits == 20)
         if(strcmp(apszLine[16], "18446744073709551615") > 0)
            BBSDEV_REJECT();
   }
   {
      size_t nDigits = 0;
      while(apszLine[17][nDigits] >= '0')
      {
         if(apszLine[17][nDigits] > '9') break;
         ++nDigits;
      }
      if(apszLine[17][nDigits] != '\0') BBSDEV_REJECT();
      if(nDigits == 0) BBSDEV_REJECT();
      if(nDigits > 1)
         if(apszLine[17][0] == '0') BBSDEV_REJECT();
      if(nDigits > 20) BBSDEV_REJECT();
      if(nDigits == 20)
         if(strcmp(apszLine[17], "18446744073709551615") > 0)
            BBSDEV_REJECT();
   }

   /* Communications type and platform support. */
   if(strcmp(apszLine[1], "local") == 0)
   {
      if(apszLine[2][0] != '\0')
         BBSDEV_REJECT();
      bLocal = TRUE;
#ifdef ODPLAT_NIX
      nMode = kComMethodStdIO;
#endif
   }
   else if(strcmp(apszLine[1], "stdio") == 0)
   {
      if(apszLine[2][0] != '\0')
         BBSDEV_REJECT();
#ifdef ODPLAT_NIX
      nMode = kComMethodStdIO;
#else
      BBSDEV_REJECT();
#endif
   }
   else if(strcmp(apszLine[1], "socket") == 0
      || strcmp(apszLine[1], "serial") == 0
      || strcmp(apszLine[1], "winserial") == 0)
   {
      for(nIndex = 0; apszLine[2][nIndex] != '\0'; ++nIndex)
      {
         unsigned nDigit;
         if(apszLine[2][nIndex] < '0') BBSDEV_REJECT();
         if(apszLine[2][nIndex] > '9') BBSDEV_REJECT();
         if(nIndex == 0 && apszLine[2][0] == '0'
            && apszLine[2][1] != '\0') BBSDEV_REJECT();
         nDigit = (unsigned)(apszLine[2][nIndex] - '0');
         if(dwNativeValue > (dwNativeMaximum - nDigit) / 10)
            BBSDEV_REJECT();
         dwNativeValue = dwNativeValue * 10 + nDigit;
      }
      if(nIndex == 0)
         BBSDEV_REJECT();
      bHasNativeValue = TRUE;
      if(strcmp(apszLine[1], "socket") == 0)
      {
#if defined(ODPLAT_NIX) || defined(ODPLAT_WIN32)
         nMode = kComMethodSocket;
#else
         BBSDEV_REJECT();
#endif
      }
      else if(strcmp(apszLine[1], "serial") == 0)
      {
#ifdef ODPLAT_NIX
         nMode = kComMethodStdIO;
#else
         BBSDEV_REJECT();
#endif
      }
      else
      {
#ifdef ODPLAT_WIN32
         nMode = kComMethodWin32;
#else
         BBSDEV_REJECT();
#endif
      }
   }
   else if(strcmp(apszLine[1], "uart") == 0)
   {
      unsigned nAddress = 0;
      unsigned nIRQ;
      if(apszLine[2][0] == '\0') BBSDEV_REJECT();
      if(apszLine[2][4] != ',') BBSDEV_REJECT();
      if(apszLine[2][5] < '0') BBSDEV_REJECT();
      if(apszLine[2][5] > '9') BBSDEV_REJECT();
      if(apszLine[2][6] != '\0')
      {
         if(apszLine[2][6] < '0') BBSDEV_REJECT();
         if(apszLine[2][6] > '9') BBSDEV_REJECT();
         if(apszLine[2][7] != '\0') BBSDEV_REJECT();
      }
      for(nIndex = 0; nIndex < 4; ++nIndex)
      {
         char current = apszLine[2][nIndex];
         unsigned digit;
         if(current >= '0' && current <= '9')
            digit = current - '0';
         else if(current >= 'A' && current <= 'F')
            digit = current - 'A' + 10;
         else
            BBSDEV_REJECT();
         nAddress = nAddress * 16 + digit;
      }
      nIRQ = (unsigned)(apszLine[2][5] - '0');
      if(apszLine[2][6] != '\0')
         nIRQ = nIRQ * 10 + (unsigned)(apszLine[2][6] - '0');
      if(nIRQ > 15)
         BBSDEV_REJECT();
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
      od_control.od_com_address = (INT16)nAddress;
      od_control.od_com_irq = (BYTE)nIRQ;
      nMode = kComMethodUART;
#else
      BBSDEV_REJECT();
#endif
   }
   else if(strcmp(apszLine[1], "fossil") == 0)
   {
      unsigned nPort = 0;
      for(nIndex = 0; apszLine[2][nIndex] != '\0'; ++nIndex)
      {
         if(apszLine[2][nIndex] < '0') BBSDEV_REJECT();
         if(apszLine[2][nIndex] > '9') BBSDEV_REJECT();
         if(nIndex == 0 && apszLine[2][0] == '0'
            && apszLine[2][1] != '\0') BBSDEV_REJECT();
         nPort = nPort * 10 + (unsigned)(apszLine[2][nIndex] - '0');
         if(nPort > 254)
            BBSDEV_REJECT();
      }
      if(nIndex == 0)
         BBSDEV_REJECT();
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
      od_control.port = (INT16)nPort;
      nMode = kComMethodFOSSIL;
#else
      BBSDEV_REJECT();
#endif
   }
   else
      BBSDEV_REJECT();

   ODBBSDevCopyText(od_control.user_name, sizeof(od_control.user_name),
      apszLine[3]);
   ODBBSDevCopyText(od_control.user_handle, sizeof(od_control.user_handle),
      apszLine[3]);
   ODBBSDevCopyText(od_control.system_name, sizeof(od_control.system_name),
      apszLine[14]);
   ODBBSDevCopyText(od_control.sysop_name, sizeof(od_control.sysop_name),
      apszLine[15]);
   od_control.user_screenwidth = (BYTE)(dwWidth > 255 ? 255 : dwWidth);
   od_control.user_screen_length = (WORD)dwHeight;
   od_control.user_ansi = apszLine[7][0] == 'Y';
   od_control.user_rip = apszLine[8][0] == 'Y';
   od_control.user_timelimit = bBBSDevDeadlineSet ? 32767 : 0;
   od_control.od_silent_mode = apszLine[18][0] == 'N'
      ? TRUE : od_control.od_silent_mode;
   od_control.od_force_local = bLocal;
   od_control.od_use_socket = nMode == kComMethodSocket;
   od_control.od_open_handle = dwNativeValue;
   bBBSDevOpenHandleSet = bHasNativeValue;
   nBBSDevComMethod = nMode;
   bIsSysop = bNamedSysop;
   bIsCoSysop = bNamedCoSysop;
   if(!bNamedSysop && !bNamedCoSysop)
   {
      DWORD dwAccess = 0;
      for(nIndex = 0; apszLine[16][nIndex] != '\0' && dwAccess < 65535L;
         ++nIndex)
      {
         dwAccess = dwAccess * 10 + (apszLine[16][nIndex] - '0');
         if(dwAccess > 65535L) dwAccess = 65535L;
      }
      od_control.user_security = (WORD)dwAccess;
   }
   {
      DWORD dwNode = 0;
      for(nIndex = 0; apszLine[17][nIndex] != '\0' && dwNode < 65535L;
         ++nIndex)
      {
         dwNode = dwNode * 10 + (apszLine[17][nIndex] - '0');
         if(dwNode > 65535L) dwNode = 65535L;
      }
      od_control.od_node = (WORD)dwNode;
   }
   od_control.od_cp437_to_utf8_out = stricmp(apszLine[11], "UTF-8") == 0;
   od_set_user_8bit(TRUE);
#ifdef ODPLAT_NIX
   od_control.baud = 1L;
#else
   od_control.baud = bLocal ? 0L : 1L;
#endif
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   if(nMode == kComMethodUART)
      od_control.od_no_fossil = TRUE;
   if(nMode == kComMethodUART || nMode == kComMethodFOSSIL)
      od_control.od_disable |= DIS_BPS_SETTING;
#elif defined(ODPLAT_NIX)
   if(nMode == kComMethodStdIO && bHasNativeValue)
      od_control.od_disable |= DIS_BPS_SETTING;
#endif
   od_control.od_info_type = BBSDEVDRP;
   bBBSDevSession = TRUE;
   pszBBSDevStorage = pszFile;
   pszBBSDevUserID = apszLine[4];
   pszFile = NULL;

finished:
   fclose(pfFile);
   if(pszFile != NULL)
      free(pszFile);
   if(!bValid)
   {
      bBBSDevSession = FALSE;
      bBBSDevDeadlineSet = FALSE;
      bBBSDevOpenHandleSet = FALSE;
      nBBSDevComMethod = kComMethodUnspecified;
   }
   return(bValid);
}

#undef BBSDEV_REJECT

/* ----------------------------------------------------------------------------
 * od_init()
 *
 * Starts up OpenDoors. Initializes various members of od_control, reads the
 * BBS door information (drop file), initializes the serial port and carries
 * out other operations that must be done at initialization time. May be
 * explicitly called by the user, or called as a result of the first call to
 * some other OpenDoors API function.
 *
 * Parameters: none
 *
 *     Return: void
 */
ODAPIDEF void ODCALL od_init(void)
{
   BYTE btCount;
   tODResult Result;
   FILE *pfDropFile=NULL;
   char *pointer;
   INT nFound = FOUND_NONE;
#ifdef _WIN32
   char *fbuf[sizeof(float)];
   volatile float *forcefloats = (void*)fbuf;

   *forcefloats=1.1;
#endif

   /* Log function entry if running in trace mode. */
   TRACE(TRACE_API, "od_init()");

   if(eODLifecycleState >= kODLifecycleExitPending)
   {
      od_control.od_error = ERR_GENERALFAILURE;
      return;
   }

   /* If a callback function is active, then don't do anything. */
   if(bIsCallbackActive) return;

   /* If we are not being called from within ODConfigInit(). */
   if(!bCalledFromConfig)
   {
      /* If OpenDoors has already been initialized, then return without */
      /* doing anything.                                                */
      if(bODInitialized) return;

      bTelnetSocket = FALSE;
      bBBSDevSession = FALSE;
      bBBSDevDeadlineSet = FALSE;
      bBBSDevOpenHandleSet = FALSE;
      nBBSDevComMethod = kComMethodUnspecified;

      Result = ODSyncSessionInitialize();
      if(Result != kODRCSuccess)
      {
         od_control.od_error = ERR_GENERALFAILURE;
         ODInitError("Unable to initialize OpenDoors synchronization.");
         return;
      }

      /* Otherwise, set the initialized flag so that od_init() won't be */
      /* run again.                                                     */
      bODInitialized = TRUE;
      eODLifecycleState = kODLifecycleInitializing;
      ODSessionTimeInitialize();

      /* Initialize program name string. */
      if(od_control.od_prog_name[0] == '\0')
      {
         strcpy(od_control.od_prog_name, OD_VER_SHORTNAME);
      }

      /* Initialize color name strings in od_control. */
      for(btCount = 0; btCount < DIM(apszColorNames); ++btCount)
      {
         if(!*od_control.od_color_names[btCount])
         {
            strcpy(od_control.od_color_names[btCount],
               apszColorNames[btCount]);
         }
      }

      /* Initialize custom drop file strings in od_control. */
      for(btCount = 0; btCount < LINES_SIZE; ++btCount)
      {
         if(!*od_control.od_cfg_lines[btCount])
         {
            strcpy(od_control.od_cfg_lines[btCount], apszConfigLines[btCount]);
         }
      }

      /* Initialize configuration keyword strings in od_control. */
      for(btCount = 0; btCount < TEXT_SIZE; ++btCount)
      {
         if(!*od_control.od_cfg_text[btCount])
         {
            strcpy(od_control.od_cfg_text[btCount], apszConfigText[btCount]);
         }
      }

      /* Enable the DOS-style multiple personality system where the local
       * presenter supports it. The Windows GUI keeps its native frame. */
#ifdef OD_PERSONALITY_SUPPORT
      if(od_control.od_mps != NULL
#ifdef ODPLAT_WIN32
         && ODPlatGetWindowsSubsystem() == kODWindowsSubsystemConsole
#endif
         )
      {
         (*(OD_COMPONENT_CALLBACK *)od_control.od_mps)();
      }
#endif /* OD_PERSONALITY_SUPPORT */

      /* If baud rate has been set in od_control, then remember the forced */
      /* rate for later use.                                               */
      if(od_control.baud != 0)
      {
         dwForcedBPS = od_control.baud;
      }

      /* If the serial port number has already been set in od_control, then */
      /* remember the forced port number for later use.                     */
      if(od_control.port != 0)
      {
         nForcedPort = od_control.port;
      }

      /* If the configuration file system has been installed, then allow   */
      /* the configuration file initialization routine take over           */
      /* initialization from here. Once it has read the configuration file */
      /* it will call od_init() again, at which time od_init() will carry  */
      /* on from this point in the code.                                  */
      if(od_control.config_file != NULL)
      {
         (*(OD_COMPONENT_CALLBACK *)od_control.config_file)();
         if(!bODInitialized)
            return;
         eODLifecycleState = kODLifecycleActive;
         if(bODExitRequestedDuringInitialization)
         {
            INT nSavedErrorLevel = nODPendingExitErrorLevel;
            BOOL bSavedTermCall = bODPendingExitTermCall;
            bODExitRequestedDuringInitialization = FALSE;
            od_exit(nSavedErrorLevel, bSavedTermCall);
         }
         return;
      }
   }

   /* Initialize the OpenDoors platform-specific utility functions. */
   ODPlatInit();

   /* Store the time of door startup. */
   time(&nStartupUnixTime);

   /* Allocate the set of strings that are used to store certain parts */
   /* of drop files for rewriting at od_exit() time.                   */
   for(btCount = 0; btCount < DIM(apszDropFileInfo); ++btCount)
   {
      if((apszDropFileInfo[btCount] = (char *)malloc(81)) == NULL)
      {
malloc_error:
         ODInitError("Insufficient memory available to start up program.");
         exit(od_control.od_errorlevel[1]);
      }
   }

   /* Determine the current node number. */ 
   if((pointer=getenv("TASK")) != NULL)
   {
      od_control.od_node = atoi(pointer);
   }
   else if((pointer=getenv("SBBSNNUM")) != NULL)
   {
      od_control.od_node = atoi(pointer);
   }
   else if(wODNodeNumber != 65535U)
   {
      od_control.od_node = wODNodeNumber;
   }
   else if(od_control.od_node == 0)
   {
      od_control.od_node = 1;
   }

   /* If a custom drop file format is not being used, then set certain   */
   /* od_control members to their default values. In the case where      */
   /* a custom drop file format is being used, these values will already */
   /* have been set, and so we don't want to touch them.                 */
   if(od_control.od_info_type != CUSTOM)
   {
      od_control.user_avatar = FALSE;
      od_control.user_rip = FALSE;
      od_control.user_attribute = 0x06;
      if(od_control.user_screen_length == 0)
         od_control.user_screen_length = 23;
      if(od_control.user_screenwidth == 0)
         od_control.user_screenwidth = 80;
      od_control.od_page_pausing = TRUE;
      od_control.od_page_len = 15;
   }
   else
   {
      /* When a custom drop file is being used, there are certain      */
      /* variables that we must initialize if they weren't already set */
      /* when the custom drop file was read.                           */
      if(od_control.user_timelimit == 0) od_control.user_timelimit = 60;
#ifdef ODPLAT_NIX
      if(od_control.port == -1) od_control.baud = 1L;
#else
      if(od_control.port == -1) od_control.baud = 0L;
#endif
   }

   /* Setup inbound local/remote buffer. */
   if(ODInQueueAlloc(&hODInputQueue, od_control.od_in_buf_size == 0 ?
      DEFAULT_EVENT_QUEUE_SIZE : od_control.od_in_buf_size) != kODRCSuccess)
   {
      goto malloc_error;
   }

   /* Enable user's keyboard by default. */
   od_control.od_user_keyboard_on = TRUE;

   /* If door information (drop) file reading has been disabled, then */
   /* don't attempt to read any drop file.                            */
   if(od_control.od_disable & DIS_INFOFILE)
   {
      od_control.od_info_type = NO_DOOR_FILE;
   }

   /* BBSDEV.DRP uses an explicit full path supplied by the launcher. */
   else if((pointer = getenv("BBSDEV_DRP")) != NULL)
   {
      if(!ODInitReadBBSDevDropFile(pointer))
      {
         ODInitError("Unable to read or validate BBSDEV.DRP.");
         exit(od_control.od_errorlevel[1]);
      }
      od_control.od_info_type = BBSDEVDRP;
   }

   /* Otherwise, if the local mode override has been explicitly asked for, */
   /* setup od_control for default local mode operation.                   */
   else if(od_control.od_force_local)
   {
force_local:
      /* No door information file is being used. */
      od_control.od_info_type = NO_DOOR_FILE;
#ifdef ODPLAT_NIX
      if(strstr(setlocale(LC_ALL, ""), "UTF-8"))
         od_control.od_cp437_to_utf8_out = TRUE;
#endif

      /* Operate in local mode. */
#ifdef ODPLAT_NIX
      od_control.baud = 1L;
#else
      od_control.baud = 0L;
#endif

      if(!bParsedCmdLine)
      {
         /* Enable ANSI mode. */
         od_control.user_ansi = TRUE;

         /* Default to 60 minutes of time available. */
         od_control.user_timelimit = 60;
      }

      /* Choose the appropriate system name. */
      if(bSystemNameSet)
      {
         strcpy(od_control.user_location, szForcedSystemName);
      }
      else if(od_control.system_name[0] != '\0')
      {
         strcpy(od_control.user_location, od_control.system_name);
      }
      else
      {
         strcpy(od_control.user_location, "Unknown Location");
      }
   }

   /* If drop file reading isn't disable, if a custom format drop file */
   /* hasn't already been read and automatic local mode hasn't been    */
   /* specified, then attempt to find and read a standard drop file.   */
   else if(od_control.od_info_type != CUSTOM)
   {
      /* Generate the name of the dorinfox.def file for this node. */
      if(od_control.od_node > 35)
      {
         apszDropFileNames[1] = (char *)"dorinfo1.def";
      }
      else if(od_control.od_node > 9)
      {
         sprintf(szIFTemp, "dorinfo%c.def", od_control.od_node + 55);
         apszDropFileNames[1] = (char *)szIFTemp;
      }
      else
      {
         sprintf(szIFTemp, "dorinfo%d.def", od_control.od_node);
         apszDropFileNames[1] = (char *)szIFTemp;
      }

      nFound = FOUND_NONE;

      if(!ODFileAccessMode(od_control.info_path, 4))
      {
         /* Check for a DORINFOx.DEF filename. */
         if(ODStringHasTail(od_control.info_path, ".def") &&
            strlen(od_control.info_path) >= strlen(apszDropFileNames[2]) &&
            strnicmp((char *)&od_control.info_path +
            (strlen(od_control.info_path) - 12), "dorinfo", 7) == 0)
         {
            nFound = FOUND_DORINFO1_DEF;
            strcpy(szDropFilePath, od_control.info_path);
         }
         else
         {
            /* Check filenames other than DORINFOx.DEF */
            for(btCount = 0; btCount < DIM(apszDropFileNames); ++btCount)
            {
               if(ODStringHasTail(od_control.info_path,
                  apszDropFileNames[btCount]))
               {
                  strcpy(szDropFilePath, od_control.info_path);
                  nFound = btCount;
                  break;
               }
            }
         }
      }

      /* Search for a door information file. */
      if(nFound == FOUND_NONE)
      {
         nFound = ODSearchForDropFile((char **)&apszDropFileNames,
            DIM(apszDropFileNames), (char *)&szDropFilePath,
            (char *)&szExitinfoBBSPath);
      }

      if(nFound == FOUND_EXITINFO_BBS)
      {
         od_control.od_info_type = NO_DOOR_FILE;
         ODInitReadExitInfo();
         if(od_control.od_info_type == NO_DOOR_FILE)
         {
            goto DropFileFail;
         }

         ODMakeFilename(szODWorkString, szExitinfoBBSPath, "dorinfo1.def",
            sizeof(szExitinfoBBSPath));
         if((pfDropFile = fopen(szODWorkString, "r")) == NULL)
         {
            goto DropFileFail;
         }

         goto read_dorinfox;
      }

      else if(nFound==FOUND_DORINFO1_DEF)
      {
          /* Open DORINFO?.DEF. */
          if((pfDropFile = fopen(szDropFilePath, "r")) == NULL) goto DropFileFail;

          /* Set door type to DORINFO. */
          od_control.od_info_type = DORINFO1;

read_dorinfox:
          /* If not able to read first line. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL)
          {
             goto DropFileFail;
          }

          if(szIFTemp[strlen(szIFTemp) - 1] == '\n')
          {
            szIFTemp[strlen(szIFTemp) - 1] = '\0';
          }
          if(szIFTemp[strlen(szIFTemp) - 1] == '\r')
          {
            szIFTemp[strlen(szIFTemp) - 1] = '\0';
          }
          strncpy(od_control.system_name, szIFTemp, 39);
          od_control.system_name[39] = '\0';

          /* get sysop name from DORINFO1.DEF */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          ODStringToName(szIFTemp);
          safe_strcpy(od_control.sysop_name, szIFTemp, 19);

                                          /* get sysop's last name */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(szIFTemp);
          if(strlen(szIFTemp))
          {
             strcat(od_control.sysop_name," ");
             safe_strcat(od_control.sysop_name,szIFTemp,19);
          }
                                   /* get com port that modem is connected to */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.port=szIFTemp[3]-'1';
                                          /* determine BPS rate of connection */
          if(fgets((char *)apszDropFileInfo[0],80,pfDropFile)==NULL) goto DropFileFail;
#ifdef ODPLAT_NIX
          od_control.baud= (od_control.port == -1) ? 1 : atol((char *)apszDropFileInfo[0]);
#else
          od_control.baud= (od_control.port == -1) ? 0 : atol((char *)apszDropFileInfo[0]);
#endif

          if(fgets((char *)apszDropFileInfo[1],80,pfDropFile)==NULL) goto DropFileFail;
          od_set_user_8bit(ODFramingIsEightBit(apszDropFileInfo[1]));

                                          /* get user's first name */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(szIFTemp);
          safe_strcpy(od_control.user_name,szIFTemp,17);
                                          /* get user's last name */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(szIFTemp);
          if(strlen(szIFTemp))
          {
             strcat(od_control.user_name," ");
             safe_strcat(od_control.user_name,szIFTemp,17);
          }
                                          /* get user's location */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(szIFTemp);
          safe_strcpy(od_control.user_location,szIFTemp,25);
                                          /* get ANSI mode settings */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(szIFTemp[0]=='0') od_control.user_ansi=FALSE;
          else od_control.user_ansi=TRUE;
                                          /* get user security level */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_security=atoi(szIFTemp);
                                          /* get time left in door */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_timelimit=atoi(szIFTemp);
          fclose(pfDropFile);
       }
                                       /* Read CHAIN.TXT */
       else if(nFound==FOUND_CHAIN_TXT)
       {
          if((pfDropFile=fopen(szDropFilePath,"r"))==NULL) goto DropFileFail;

          od_control.od_info_type=CHAINTXT;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_num=atoi(szIFTemp);

          if(fgets((char *)&od_control.user_handle,35,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(od_control.user_handle);

          if(fgets((char *)&od_control.user_name,35,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(od_control.user_name);

          if(fgets((char *)&od_control.user_callsign,12,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(od_control.user_callsign);

          if(fgets((char *)apszDropFileInfo[0],80,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_sex=szIFTemp[0];

          if(fgets((char *)apszDropFileInfo[1],80,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          safe_strcpy(od_control.user_lastdate,szIFTemp,8);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_screenwidth=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_screen_length=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_security=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          bIsSysop=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          bIsCoSysop=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_ansi=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;    /* non-zero if remote */

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_timelimit=atoi(szIFTemp);
          od_control.user_timelimit/=60;

          if(fgets((char *)apszDropFileInfo[3],80,pfDropFile)==NULL) goto DropFileFail;

          if(fgets((char *)apszDropFileInfo[4],80,pfDropFile)==NULL) goto DropFileFail;

          if(fgets((char *)apszDropFileInfo[5],80,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(strcmp(szIFTemp,"KB")==0)
          {
#ifdef ODPLAT_NIX
             od_control.baud=1;
#else
             od_control.baud=0;
#endif
          }
          else
          {
             od_control.baud=atol(szIFTemp);
          }

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.port=atoi(szIFTemp)-1;

          if(fgets((char *)apszDropFileInfo[6],80,pfDropFile)==NULL) goto DropFileFail;

          if(fgets((char *)&od_control.user_password,15,pfDropFile)==NULL) goto DropFileFail;
          ODStringToName(od_control.user_password);

          if(fgets((char *)apszDropFileInfo[2],80,pfDropFile)==NULL) goto DropFileFail;
          if(fgets((char *)apszDropFileInfo[7],80,pfDropFile)==NULL) goto DropFileFail;
          if(fgets((char *)apszDropFileInfo[8],80,pfDropFile)==NULL) goto DropFileFail;
          if(fgets((char *)apszDropFileInfo[9],80,pfDropFile)==NULL) goto DropFileFail;
          if(fgets((char *)apszDropFileInfo[10],80,pfDropFile)==NULL) goto DropFileFail;
          if(fgets((char *)apszDropFileInfo[11],80,pfDropFile)==NULL) goto DropFileFail;
          if(fgets((char *)apszDropFileInfo[12],80,pfDropFile)==NULL) goto DropFileFail;
          od_set_user_8bit(ODFramingIsEightBit(apszDropFileInfo[12]));

          fclose(pfDropFile);
       }

       else if(nFound == FOUND_SFDOORS_DAT 
			 || nFound == FOUND_SFMAIN_DAT 
			 || nFound == FOUND_SFFILE_DAT 
			 || nFound == FOUND_SFMESS_DAT
          || nFound == FOUND_SFSYSOP_DAT)
       {
          od_control.od_info_type = SFDOORSDAT;

          if(!ODInitReadSFDoorsDAT())
          {
             goto DropFileFail;
          }
       }

       else if(nFound==FOUND_DOOR_SYS)
       {
          if((pfDropFile=fopen(szDropFilePath,"r"))==NULL) goto DropFileFail;

          /* Read line 1. */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;

          if(szIFTemp[0]=='C' && szIFTemp[1]=='O' && szIFTemp[2]=='M' && szIFTemp[4]==':')
          {                            /* GAP style DOOR.SYS */
             od_control.od_info_type=DOORSYS_GAP;

             od_control.port=szIFTemp[3]-'1';

             /* Check for COM0:STDIO */
             if(!strncmp(szIFTemp,"COM0:STDIO",10))
                od_control.od_com_method=COM_STDIO;

             /* Check for COM0:SOCKET### */
             if(!strncmp(szIFTemp,"COM0:SOCKET",11)) {
                od_control.od_com_method=COM_SOCKET;
		od_control.od_use_socket = TRUE;
                od_control.od_open_handle=atoi(szIFTemp+11);
             }

             /* Check for COM0:HANDLE### */
             if(!strncmp(szIFTemp,"COM0:HANDLE",11)) {
                od_control.od_com_method=COM_WIN32;
                od_control.od_open_handle=atoi(szIFTemp+11);
             }

             /* Read line 2. */
             if(fgets((char *)apszDropFileInfo[0], 80, pfDropFile) == NULL)
             {
               goto DropFileFail;
             }
             od_control.od_connect_speed = atol(apszDropFileInfo[0]);

             /* Read line 3. */
             if(fgets((char *)apszDropFileInfo[1],80,pfDropFile)==NULL) goto DropFileFail;
             od_set_user_8bit(ODFramingIsEightBit(apszDropFileInfo[1]));

             /* Read line 4. */
             if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
             od_control.od_node = atoi(szIFTemp);

             /* Read line 5. */
             if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
             strupr(szIFTemp);
             if(strchr(szIFTemp, 'N') != NULL)
             {
                btDoorSYSLock = 1;
                od_control.baud = atol(apszDropFileInfo[0]);
             }
             else if(strchr(szIFTemp, 'Y') != NULL)
             {
                btDoorSYSLock = 2;
                od_control.baud = 19200;
             }
             else
             {
                od_control.baud = atol(szIFTemp);
             }

#ifdef ODPLAT_NIX
             if(od_control.port == -1) od_control.baud = 1L;
#else
             if(od_control.port == -1) od_control.baud = 0L;
#endif

             /* Read line 6. */
             if(fgets((char *)apszDropFileInfo[3],80,pfDropFile)==NULL) goto DropFileFail;

             /* Read line 7. */
             if(fgets((char *)apszDropFileInfo[4],80,pfDropFile)==NULL) goto DropFileFail;

             /* Read line 8. */
             if(fgets((char *)apszDropFileInfo[5],80,pfDropFile)==NULL) goto DropFileFail;

             /* Read line 9. */
             if(fgets((char *)apszDropFileInfo[22],80,pfDropFile)==NULL) goto DropFileFail;

             /* Read line 10. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             szIFTemp[35]='\0';
             ODStringToName(szIFTemp);
             strcpy(od_control.user_name,szIFTemp);

             /* Read line 11. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             szIFTemp[25]='\0';
             if(szIFTemp[strlen(szIFTemp)-1]=='\n') szIFTemp[strlen(szIFTemp)-1]='\0';
             if(szIFTemp[strlen(szIFTemp)-1]=='\r') szIFTemp[strlen(szIFTemp)-1]='\0';
             strcpy(od_control.user_location,szIFTemp);

             /* Read line 12. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             szIFTemp[15]='\0';
             ODStringToName(szIFTemp);
             strcpy(od_control.user_homephone,szIFTemp);

             /* Read line 13. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             szIFTemp[15]='\0';
             ODStringToName(szIFTemp);
             strcpy(od_control.user_dataphone,szIFTemp);

             /* Read line 14. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             szIFTemp[15]='\0';
             if(szIFTemp[strlen(szIFTemp)-1]=='\n') szIFTemp[strlen(szIFTemp)-1]='\0';
             if(szIFTemp[strlen(szIFTemp)-1]=='\r') szIFTemp[strlen(szIFTemp)-1]='\0';
             strcpy(od_control.user_password,szIFTemp);

             /* Read line 15. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_security=atoi(szIFTemp);

             /* Read line 16. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_numcalls=atoi(szIFTemp);

             /* Read line 17. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             szIFTemp[15]='\0';
             ODStringToName(szIFTemp);
             strcpy(od_control.user_lastdate,szIFTemp);

             /* Read line 18. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;

             /* Read line 19. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_timelimit=atoi(szIFTemp);

             /* Read line 20. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             strupr(szIFTemp);
             if(!strcmp(szIFTemp,"RIP"))
             {
                od_control.user_rip=TRUE;
                od_control.user_ansi=TRUE;
             }
             else if(szIFTemp[0]=='G')
             {
                od_control.user_rip=FALSE;
                od_control.user_ansi=TRUE;
             }
             else
             {
                od_control.user_rip=FALSE;
                od_control.user_ansi=FALSE;
             }

             /* Read line 21. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_screen_length=atoi(szIFTemp);

             /* Read line 22. */
             if(fgets((char *)apszDropFileInfo[8],80,pfDropFile)==NULL) goto DropFileFail;

             /* Read line 23. */
             if(fgets((char *)apszDropFileInfo[9],80,pfDropFile)==NULL) goto DropFileFail;
             if(apszDropFileInfo[9][strlen(apszDropFileInfo[9])-1]!='\n')
             {
                int ch;
                apszDropFileInfo[9][strlen(apszDropFileInfo[9])-1]='\n';
                do
                {
                   ch = fgetc(pfDropFile);
                } while(ch != '\n' && ch != EOF);
             }

             /* Read line 24. */
again:
             if(fgets((char *)apszDropFileInfo[10],80,pfDropFile)==NULL) goto DropFileFail;
             if(strchr(apszDropFileInfo[10],',')!=NULL) goto again;

             /* Read line 25. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             szIFTemp[15]='\0';
             ODStringToName(szIFTemp);
             strcpy(od_control.user_subdate,szIFTemp);

             /* Read line 26. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_num=atoi(szIFTemp);

             /* Read line 27. */
             if(fgets((char *)apszDropFileInfo[6],80,pfDropFile)==NULL) goto DropFileFail;

             /* Read line 28. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_uploads=atoi(szIFTemp);

             /* Read line 29. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_downloads=atoi(szIFTemp);

             /* Read line 30. */
             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_todayk=atoi(szIFTemp);

             /* Read line 31. */
             if(fgets((char *)apszDropFileInfo[21],80,pfDropFile)==NULL) goto DropFileFail;


                                        /* Beginning of extending DOOR.SYS data */
             /* Read line 32. */
             if(fgets((char *)apszDropFileInfo[7],80,pfDropFile)==NULL)
                apszDropFileInfo[7][0] = '\0';
             /* Read line 33. */
             if(fgets((char *)apszDropFileInfo[11],80,pfDropFile)==NULL)
                apszDropFileInfo[11][0] = '\0';
             /* Read line 34. */
             if(fgets((char *)apszDropFileInfo[12],80,pfDropFile)==NULL)
                apszDropFileInfo[12][0] = '\0';
             /* Read line 35. */
             if(fgets((char *)apszDropFileInfo[13],80,pfDropFile)==NULL)
                apszDropFileInfo[13][0] = '\0';
             /* Read line 36. */
             if(fgets((char *)apszDropFileInfo[14],80,pfDropFile)!=NULL)
             {
                /* Read line 37. */
                strncpy(od_control.user_birthday,apszDropFileInfo[7],8);
                od_control.user_birthday[8]='\0';

                /* Read line 38. */
                strncpy(od_control.sysop_name,apszDropFileInfo[13],39);
                od_control.sysop_name[39]='\0';
                ODStringToName(od_control.sysop_name);

                /* Read line 39. */
                strncpy(od_control.user_handle,apszDropFileInfo[14],35);
                od_control.user_handle[35]='\0';
                ODStringToName(od_control.user_handle);

                /* Read line 40. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                strncpy(od_control.event_starttime, szIFTemp, 5);
                od_control.event_starttime[5] = '\0';

                /* Read line 41. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                if(szIFTemp[0]=='y' || szIFTemp[0]=='Y')
                   od_control.user_error_free=TRUE;
                else
                   od_control.user_error_free=FALSE;

                /* Read line 42. */
                if(fgets((char *)apszDropFileInfo[7],80,pfDropFile)==NULL) goto finished;
                /* Read line 43. */
                if(fgets((char *)apszDropFileInfo[13],80,pfDropFile)==NULL) goto finished;
                /* Read line 44. */
                if(fgets((char *)apszDropFileInfo[14],80,pfDropFile)==NULL) goto finished;
                /* Read line 45. */
                if(fgets((char *)apszDropFileInfo[15],80,pfDropFile)==NULL) goto finished;
                /* Read line 46. */
                if(fgets((char *)apszDropFileInfo[16],80,pfDropFile)==NULL) goto finished;

                /* Read line 47. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                strncpy(od_control.user_logintime,szIFTemp,5);
                od_control.user_logintime[5]='\0';

                /* Read line 48. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                strncpy(od_control.user_lasttime,szIFTemp,5);
                od_control.user_lasttime[5]='\0';

                /* Read line 49. */
                if(fgets((char *)apszDropFileInfo[18],80,pfDropFile)==NULL) goto finished;

                /* Read line 50. */
                if(fgets((char *)apszDropFileInfo[19],80,pfDropFile)==NULL) goto finished;

                /* Read line 51. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                od_control.user_upk=atoi(szIFTemp);

                /* Read line 52. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                od_control.user_downk=atoi(szIFTemp);

                /* Read line 53. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                strncpy(od_control.user_comment,szIFTemp,79);
                od_control.user_comment[79]='\0';
                if(od_control.user_comment[strlen(od_control.user_comment)-1]=='\n')
                   od_control.user_comment[strlen(od_control.user_comment)-1]='\0';
                if(od_control.user_comment[strlen(od_control.user_comment)-1]=='\r')
                   od_control.user_comment[strlen(od_control.user_comment)-1]='\0';

                /* Read line 54. */
                if(fgets((char *)apszDropFileInfo[20],80,pfDropFile)==NULL) goto finished;

                /* Read line 55. */
                if(fgets(szIFTemp,255,pfDropFile)==NULL) goto finished;
                od_control.user_messages=atoi(szIFTemp);


                od_control.od_info_type=DOORSYS_WILDCAT;
             }
          }

          else                            /* DoorWay style DOOR.SYS */
          {
             od_control.od_info_type=DOORSYS_DRWY;

             szIFTemp[35]='\0';
             ODStringToName(szIFTemp);
             strcpy(od_control.user_name,szIFTemp);

             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.port=szIFTemp[0]-'1';

             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             if(od_control.port==-1)
             {
#ifdef ODPLAT_NIX
                od_control.baud=1L;
#else
                od_control.baud=0L;
#endif
             }
             else
             {
                od_control.baud=atol(szIFTemp);
             }

             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             od_control.user_timelimit=atoi(szIFTemp);

             if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
             if(szIFTemp[0]=='G')
             {
                od_control.user_ansi=TRUE;
             }
             else
             {
                od_control.user_ansi=FALSE;
             }
          }
finished:
          fclose(pfDropFile);
       }

       else if(nFound==FOUND_CALLINFO_BBS)
       {
          if((pfDropFile=fopen(szDropFilePath,"r"))==NULL) goto DropFileFail;

          od_control.od_info_type=CALLINFO;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          szIFTemp[35]='\0';
          ODStringToName(szIFTemp);
          strcpy(od_control.user_name,szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          szIFTemp[25]='\0';
          ODStringToName(szIFTemp);
          strcpy(od_control.user_location,szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_security=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_timelimit=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(szIFTemp[0]=='M')
          {
             od_control.user_ansi=FALSE;
          }
          else
          {
             od_control.user_ansi=TRUE;
          }

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          szIFTemp[15]='\0';
          ODStringToName(szIFTemp);
          strcpy(od_control.user_password,szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          szIFTemp[15]='\0';
          ODStringToName(szIFTemp);
          strcpy(od_control.user_homephone,szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_screen_length=atoi(szIFTemp);

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.port=szIFTemp[3]-'1';

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;

          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.baud=atol(szIFTemp);

          fclose(pfDropFile);
       }
       else if(nFound == FOUND_TRIBBS_SYS)
       {
          /* We have found reading a TRIBBS.SYS file. */

          /* Attempt to open the file. */
          if((pfDropFile = fopen(szDropFilePath, "r")) == NULL)
          {
             goto DropFileFail;
          }

          /* Record the drop file type being used. */
          od_control.od_info_type = TRIBBSSYS;

          /* Read line 1: User's number. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_num = atoi(szIFTemp);

          /* Read line 2: User's name. */
          if(fgets((char *)&od_control.user_name, 35, pfDropFile) == NULL)
          {
             goto DropFileFail;
          }
          ODStringToName(od_control.user_name);

          /* Read line 3: User's password. */
          if(fgets((char *)&od_control.user_password, 15, pfDropFile) == NULL)
          {
             goto DropFileFail;
          }

          /* Read line 4: User's security level. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_security = atoi(szIFTemp);

          /* Read line 5: "Y" for expert mode, "N" for novice mode. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_expert = (szIFTemp[0] == 'Y');

          /* Read line 6: "Y" for ANSI, "N" for TTY. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_ansi = (szIFTemp[0] == 'Y');

          /* Read line 7: Minutes remaining for this call. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_timelimit = atoi(szIFTemp);

          /* Read line 8: User's phone number. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          szIFTemp[15] = '\0';
          ODStringToName(szIFTemp);
          strcpy(od_control.user_homephone, szIFTemp);

          /* Read line 9: User's location. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          ODStringToName(szIFTemp);
          safe_strcpy(od_control.user_location, szIFTemp, 25);

          /* Read line 10: User's birthday. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          strncpy(od_control.user_birthday, szIFTemp, 8);
          od_control.user_birthday[8] = '\0';
          od_control.user_birthday[2] = '-';
          od_control.user_birthday[5] = '-';

          /* Read line 11: Node number. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.od_node = atoi(szIFTemp);

          /* Read line 12: Serial port. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.port = atoi(szIFTemp) - 1;

          /* Read line 13: Baud rate. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.od_connect_speed =
             (od_control.port == -1) ? 0 : atol(szIFTemp);

          /* Read line 14: Locked baud rate or 0 for unlocked port. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.baud = atol(szIFTemp);
          if(od_control.baud == 0)
          {
             od_control.baud = od_control.od_connect_speed;
          }

          /* Read line 15: "Y" for RTS/CTS handshaking, "N" for none. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.od_com_flow_control = (szIFTemp[0] == 'Y')
             ? COM_RTSCTS_FLOW : COM_NO_FLOW;

          /* Read line 16: "Y" for error free connection, "N" otherwise. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_error_free = (szIFTemp[0] == 'Y');

          /* Read line 17: BBS name. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          ODStringCopy(od_control.system_name, szIFTemp, 40);
          ODStringToName(od_control.system_name);

          /* Read line 18: Sysop's name. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          ODStringCopy(od_control.sysop_name, szIFTemp, 40);
          ODStringToName(od_control.sysop_name);

          /* Read line 19: User's alias. */
          if(fgets((char *)&od_control.user_handle, 35, pfDropFile) == NULL)
          {
             goto DropFileFail;
          }
          ODStringToName(od_control.user_handle);

          /* Read line 20: "Y" for RIP mode, "N" for no RIP mode. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_rip = (szIFTemp[0] == 'Y');

          /* Now, close the TRIBBS.SYS file. */
          fclose(pfDropFile);
       }
       else if(nFound == FOUND_DOOR32_SYS)
       {
          /* We have found a DOOR32.SYS file. */

          /* Attempt to open the file. */
          if((pfDropFile = fopen(szDropFilePath, "r")) == NULL)
          {
             goto DropFileFail;
          }

          /* Record the drop file type being used. */
          od_control.od_info_type = DOOR32SYS;

          /* Read line 1: Comm type. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          switch(atoi(szIFTemp)) {
				case 0: /* local */
					od_control.od_force_local = TRUE;	/* Is this the right way? */
					break;
				case 1: /* serial */
					break;
				case 2: /* telnet */
					od_control.od_use_socket = TRUE;
					bTelnetSocket = TRUE;
					break;
			 }
			 /* Read line 2: Comm or Socket handle. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.od_open_handle	= atoi(szIFTemp);

			 /* Read line 3: Baud rate */
          if(fgets((char *)apszDropFileInfo[0],80,pfDropFile)==NULL) goto DropFileFail;
#ifdef ODPLAT_NIX
          od_control.baud= (od_control.port == -1) ? 1 : atol((char *)apszDropFileInfo[0]);
#else
          od_control.baud= (od_control.port == -1) ? 0 : atol((char *)apszDropFileInfo[0]);
#endif

			 /* Read line 4: BBS Software name and version - unused. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;

          /* Read line 5: User's number. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_num = atoi(szIFTemp);

          /* Read line 6: User's real name. */
          if(fgets((char *)&od_control.user_name, 35, pfDropFile) == NULL)
          {
             goto DropFileFail;
          }
          ODStringToName(od_control.user_name);

          /* Read line 7: User's alias. */
          if(fgets((char *)&od_control.user_handle, 35, pfDropFile) == NULL)
          {
             goto DropFileFail;
          }
          ODStringToName(od_control.user_handle);

          /* Read line 8: User's security level */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
          od_control.user_security=atoi(szIFTemp);

          /* Read line 9: Minutes remaining for this call. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.user_timelimit = atoi(szIFTemp);

          /* Read line 10: User's Terminal type  */
          if(fgets(szIFTemp,255,pfDropFile)==NULL) goto DropFileFail;
    		 od_control.user_ansi=TRUE;
			 switch(atoi(szIFTemp)) {
					case 0: /* ASCII */
						od_control.user_ansi=FALSE;
						break;
					case 1: /* ANSI */
						od_control.user_ansi=TRUE;
						break;
					case 2: /* AVATAR */
						od_control.user_avatar=TRUE;
						break;
					case 3: /* RIP */
						od_control.user_rip=TRUE;
						break;
			 }

          /* Read line 11: Current Node number. */
          if(fgets(szIFTemp, 255, pfDropFile) == NULL) goto DropFileFail;
          od_control.od_node = atoi(szIFTemp);

          /* Now, close the DOOR32.SYS file. */
          fclose(pfDropFile);
       }
       else
       {
DropFileFail:
			 if(pfDropFile!=NULL)	/* Let's not leave the file open */
				 fclose(pfDropFile);

          od_control.od_info_type = NO_DOOR_FILE;

          if(od_control.od_no_file_func != NULL)
          {
             (*od_control.od_no_file_func)();
          }

          /* Check whether force local has been turned on by the no */
          /* file function.                                         */
          if(od_control.od_force_local)
          {
             goto force_local;
          }

          /* Exit with failure if the no file function has not read a */
          /* door information file itself.                            */
          if(od_control.od_info_type == NO_DOOR_FILE
            && (wPreSetInfo & PRESET_REQUIRED) != PRESET_REQUIRED)
          {
             ODInitError("Unable to read door information (drop) file.");
             exit(od_control.od_errorlevel[1]);
          }
       }
   }

   ODInitPartTwo();
   if(!bODInitialized)
      return;
   if(!bCalledFromConfig)
   {
      eODLifecycleState = kODLifecycleActive;
      if(bODExitRequestedDuringInitialization)
      {
         INT nSavedErrorLevel = nODPendingExitErrorLevel;
         BOOL bSavedTermCall = bODPendingExitTermCall;
         bODExitRequestedDuringInitialization = FALSE;
         od_exit(nSavedErrorLevel, bSavedTermCall);
      }
   }
}


/* ----------------------------------------------------------------------------
 * ODInitReadSFDoorsDAT()                              *** PRIVATE FUNCTION ***
 *
 * Reads an SFDOORS.DAT format drop file.
 *
 * Parameters: none
 *
 *     Return: TRUE on success or FALSE on failure.
 */
static BOOL ODInitReadSFDoorsDAT(void)
{
   FILE *pfDropFile;
   long nLoginMinutes;
   unsigned int nLoginMinutePart;
   unsigned int nLoginHourPart;

   if((pfDropFile=fopen(szDropFilePath,"r"))==NULL) return(FALSE);

   /* Line 1: User number. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_num=atoi(szIFTemp);

   /* Line 2: User name. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   szIFTemp[35]='\0';
   ODStringToName(szIFTemp);
   strcpy(od_control.user_name,szIFTemp);

   /* Line 3: User password. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   szIFTemp[15]='\0';
   ODStringToName(szIFTemp);
   strcpy(od_control.user_password,szIFTemp);

   /* Line 4: Unused. */
   if(fgets((char *)apszDropFileInfo[0],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 5: Modem <-> Serial port bps rate. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.baud=atol(szIFTemp);

   /* Line 6: Serial port number. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.port=atoi(szIFTemp)-1;

   /* Line 7: User's time remaining. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_timelimit=atoi(szIFTemp);

   /* Line 8: Unused. */
   if(fgets((char *)apszDropFileInfo[13],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 9: Unused. */
   if(fgets((char *)apszDropFileInfo[14],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 10: User's ANSI mode setting. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   strupr(szIFTemp);
   od_control.user_ansi=(szIFTemp[0]=='T');

   /* Line 11: User's security level. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_security=atoi(szIFTemp);

   /* Line 12: User's upload count. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_uploads=atoi(szIFTemp);

   /* Line 13: User's download count. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_downloads=atoi(szIFTemp);

   /* Line 14: Unused. */
   if(fgets((char *)apszDropFileInfo[1],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 15: User's login time. */
   if(fgets((char *)apszDropFileInfo[2],80,pfDropFile)==NULL) goto ReadFailure;
   nLoginMinutes = atol((char *)apszDropFileInfo[2]);
   if(nLoginMinutes < 0)
      nLoginMinutes = 0;
   else if(nLoginMinutes > 5999)
      nLoginMinutes = 5999;
   nLoginMinutePart = (unsigned int)(nLoginMinutes % 60);
   nLoginHourPart = (unsigned int)(nLoginMinutes / 60);
   sprintf(od_control.user_logintime, "%02u:%02u",
      nLoginHourPart, nLoginMinutePart);

   /* Line 16: Unused. */
   if(fgets((char *)apszDropFileInfo[3],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 17: Sysop next flag. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   strupr(szIFTemp);
   od_control.sysop_next=(szIFTemp[0]=='T');

   /* Line 18: Unused. */
   if(fgets((char *)apszDropFileInfo[4],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 19: Unused. */
   if(fgets((char *)apszDropFileInfo[5],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 20: Unused. */
   if(fgets((char *)apszDropFileInfo[6],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 21: Error free connection flag. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   strupr(szIFTemp);
   od_control.user_error_free=(szIFTemp[0]=='T');

   /* Line 22: Current message area. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_msg_area=atoi(szIFTemp);

   /* Line 23: Current file area. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_file_area=atoi(szIFTemp);

   /* Line 24: Current node number. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.od_node=atoi(szIFTemp);

   /* Line 25: Unused. */
   if(fgets((char *)apszDropFileInfo[10],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 26: Unused. */
   if(fgets((char *)apszDropFileInfo[11],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 27: Unused. */
   if(fgets((char *)apszDropFileInfo[12],80,pfDropFile)==NULL) goto ReadFailure;

   /* Line 28: Kilobytes downloaded today. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_todayk=atoi(szIFTemp);

   /* Line 29: Kilobytes uploaded in total. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_upk=atoi(szIFTemp);

   /* Line 30: Kilobytes downloaded in total. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   od_control.user_downk=atoi(szIFTemp);

   /* Line 31: User's home phone number. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   szIFTemp[15]='\0';
   ODStringToName(szIFTemp);
   strcpy(od_control.user_homephone,szIFTemp);

   /* Line 32: User's home location. */
   if(fgets(szIFTemp,255,pfDropFile)==NULL) goto ReadFailure;
   szIFTemp[25]='\0';
   ODStringToName(szIFTemp);
   strcpy(od_control.user_location,szIFTemp);

   /* Line 33: Unused. */
   if(fgets((char *)apszDropFileInfo[15],80,pfDropFile)==NULL)
   {
      apszDropFileInfo[15][0]='\0';
   }

   /* Line 34: RIP mode. */
   if(fgets(szIFTemp,255,pfDropFile) != NULL)
   {
      strupr(szIFTemp);
      od_control.user_rip = (szIFTemp[0] == 'T');
   }

   /* line 35: User wants chat. */
   if(fgets(szIFTemp,255,pfDropFile) != NULL)
   {
      strupr(szIFTemp);
      od_control.user_wantchat = (szIFTemp[0] == 'T');
   }

   /* Line 36: Unused. */
   if(fgets((char *)apszDropFileInfo[17],80,pfDropFile)==NULL)
   {
      apszDropFileInfo[17][0]='\0';
   }

   /* Line 37: IRQ number. */
   if(fgets(szIFTemp,255,pfDropFile) != NULL)
   {
      od_control.od_com_irq = atoi(szIFTemp);
   }

   /* Line 38: Serial port address. */
   if(fgets(szIFTemp,255,pfDropFile) != NULL)
   {
      od_control.od_com_address = atoi(szIFTemp);
   }

   /* Line 39: Unused. */
   if(fgets((char *)apszDropFileInfo[18],80,pfDropFile)==NULL)
   {
      apszDropFileInfo[18][0]='\0';
   }

   fclose(pfDropFile);

   return(TRUE);

ReadFailure:
   fclose(pfDropFile);
   return(FALSE);
}


/* ----------------------------------------------------------------------------
 * ODInitReadExitInfo()                                *** PRIVATE FUNCTION ***
 *
 * Reads an EXITINFO.BBS format drop file.
 *
 * Parameters: none
 *
 *     Return: void
 */
static void ODInitReadExitInfo(void)
{
   DWORD dwFileSize;
   FILE *pfDropFile;
   BYTE btCount;

   od_control.od_extended_info=FALSE;
   od_control.od_ra_info=FALSE;

   /* Try to open EXITINFO.BBS. */
   ODMakeFilename(szODWorkString, szExitinfoBBSPath, "exitinfo.bbs",
      sizeof(szExitinfoBBSPath));
   if((pfDropFile = fopen(szODWorkString, "rb")) != NULL)
   {
      dwFileSize = ODFileSize(pfDropFile);

      if(dwFileSize >= 2363)
      {
         if((pRA2ExitInfoRecord=malloc(sizeof(tRA2ExitInfoRecord)))!=NULL)
         {
            if(fread(pRA2ExitInfoRecord,1,2363,pfDropFile)==2363)
            {
               ODExitInfoRA2Endian(pRA2ExitInfoRecord, TRUE);
               od_control.od_ra_info=TRUE;
               od_control.od_extended_info=TRUE;
               od_control.od_info_type=RA2EXITINFO;

               od_control.baud=(long)pRA2ExitInfoRecord->baud;
               od_control.system_calls=pRA2ExitInfoRecord->num_calls;
               ODStringPascalToC(od_control.system_last_caller,pRA2ExitInfoRecord->last_caller,35);
               ODStringPascalToC(od_control.system_last_handle,pRA2ExitInfoRecord->sLastHandle,35);
               ODStringPascalToC(od_control.timelog_start_date,pRA2ExitInfoRecord->start_date,8);
               memcpy(od_control.timelog_busyperhour,
                  pRA2ExitInfoRecord->busyperhour,
                  sizeof(od_control.timelog_busyperhour));
               memcpy(od_control.timelog_busyperday,
                  pRA2ExitInfoRecord->busyperday,
                  sizeof(od_control.timelog_busyperday));
               ODStringPascalToC(od_control.user_name,pRA2ExitInfoRecord->name,35);
               ODStringPascalToC(od_control.user_location,pRA2ExitInfoRecord->location,25);
               ODStringPascalToC(od_control.user_org,pRA2ExitInfoRecord->organisation,50);
               for(btCount=0;btCount<3;++btCount)
                  ODStringPascalToC(od_control.user_address[btCount],pRA2ExitInfoRecord->address[btCount],50);
               ODStringPascalToC(od_control.user_handle,pRA2ExitInfoRecord->handle,35);
               ODStringPascalToC(od_control.user_comment,pRA2ExitInfoRecord->comment,80);
               od_control.user_pwd_crc=pRA2ExitInfoRecord->password_crc;
               ODStringPascalToC(od_control.user_dataphone,pRA2ExitInfoRecord->dataphone,15);
               ODStringPascalToC(od_control.user_homephone,pRA2ExitInfoRecord->homephone,15);
               ODStringPascalToC(od_control.user_lasttime,pRA2ExitInfoRecord->lasttime,5);
               ODStringPascalToC(od_control.user_lastdate,pRA2ExitInfoRecord->lastdate,8);
               od_control.user_attribute=pRA2ExitInfoRecord->attrib;
               od_control.user_attrib2=pRA2ExitInfoRecord->attrib2;
               memcpy(&od_control.user_flags,&pRA2ExitInfoRecord->flags,14);
               od_control.user_security=pRA2ExitInfoRecord->sec;
               od_control.user_lastread=pRA2ExitInfoRecord->lastread;
               memcpy(&od_control.user_numcalls,&pRA2ExitInfoRecord->nocalls,29);
               od_control.user_group=pRA2ExitInfoRecord->group;
               memcpy(&od_control.user_combinedrecord,&pRA2ExitInfoRecord->combinedrecord,200);
               ODStringPascalToC(od_control.user_firstcall,pRA2ExitInfoRecord->firstcall,8);
               ODStringPascalToC(od_control.user_birthday,pRA2ExitInfoRecord->birthday,8);
               ODStringPascalToC(od_control.user_subdate,pRA2ExitInfoRecord->subdate,8);
               od_control.user_screenwidth=pRA2ExitInfoRecord->screenwidth;
               od_control.user_language=pRA2ExitInfoRecord->language;
               od_control.user_date_format=pRA2ExitInfoRecord->dateformat;
               ODStringPascalToC(od_control.user_forward_to,pRA2ExitInfoRecord->forwardto,35);
               memcpy(&od_control.user_msg_area,&pRA2ExitInfoRecord->msgarea,15);
               od_control.user_sex = (pRA2ExitInfoRecord->sex == 1) ? 'M' : 'F';
               od_control.user_attrib3=pRA2ExitInfoRecord->btAttribute3;
               ODStringPascalToC(od_control.user_password,pRA2ExitInfoRecord->sPassword,15);
               od_control.event_status=pRA2ExitInfoRecord->status;
               ODStringPascalToC(od_control.event_starttime,pRA2ExitInfoRecord->starttime,5);
               memcpy(&od_control.event_errorlevel,&pRA2ExitInfoRecord->errorlevel,3);
               ODStringPascalToC(od_control.event_last_run,pRA2ExitInfoRecord->lasttimerun,8);
               memcpy(&od_control.user_netmailentered,&pRA2ExitInfoRecord->netmailentered,2);
               ODStringPascalToC(od_control.user_logintime,pRA2ExitInfoRecord->logintime,5);
               ODStringPascalToC(od_control.user_logindate,pRA2ExitInfoRecord->logindate,8);
               memcpy(&od_control.user_timelimit,&pRA2ExitInfoRecord->timelimit,6);
               memcpy(&od_control.user_num,&pRA2ExitInfoRecord->userrecord,8);
               ODStringPascalToC(od_control.user_timeofcreation,pRA2ExitInfoRecord->timeofcreation,5);
               od_control.user_logon_pwd_crc=pRA2ExitInfoRecord->logonpasswordcrc;
               od_control.user_wantchat=pRA2ExitInfoRecord->wantchat;
               od_control.user_deducted_time=pRA2ExitInfoRecord->deducted_time;
               for(btCount=0;btCount<50;++btCount)
                  ODStringPascalToC(od_control.user_menustack[btCount],pRA2ExitInfoRecord->menustack[btCount],8);
               od_control.user_menustackpointer=pRA2ExitInfoRecord->menustackpointer;
               memcpy(&od_control.user_error_free,&pRA2ExitInfoRecord->error_free,3);
               ODStringPascalToC(od_control.user_emsi_crtdef,pRA2ExitInfoRecord->emsi_crtdef,40);
               ODStringPascalToC(od_control.user_emsi_protocols,pRA2ExitInfoRecord->emsi_protocols,40);
               ODStringPascalToC(od_control.user_emsi_capabilities,pRA2ExitInfoRecord->emsi_capabilities,40);
               ODStringPascalToC(od_control.user_emsi_requests,pRA2ExitInfoRecord->emsi_requests,40);
               ODStringPascalToC(od_control.user_emsi_software,pRA2ExitInfoRecord->emsi_software,40);
               memcpy(&od_control.user_hold_attr1,&pRA2ExitInfoRecord->hold_attr1,3);
               ODStringPascalToC(od_control.user_reasonforchat,pRA2ExitInfoRecord->page_reason,77);
               bRAStatus = TRUE;
               btRAStatusToSet = pRA2ExitInfoRecord->status_line-1;
               ODStringPascalToC(od_control.user_last_cost_menu,pRA2ExitInfoRecord->last_cost_menu,8);
               od_control.user_menu_cost=pRA2ExitInfoRecord->menu_cost_per_min;
               od_control.user_rip=pRA2ExitInfoRecord->has_rip;
               od_control.user_rip_ver=pRA2ExitInfoRecord->btRIPVersion;

               od_control.user_ansi=od_control.user_attribute&8;
               od_control.user_avatar=od_control.user_attrib2&2;
            }

            else
            {
               free(pRA2ExitInfoRecord);
               pRA2ExitInfoRecord = NULL;
            }
         }
      }

      else if(dwFileSize>=1493)
      {
         if(ODReadExitInfoPrimitive(pfDropFile,476))
         {
            if((pExtendedExitInfo=malloc(sizeof(tExtendedExitInfo)))!=NULL)
            {
               if(fread(pExtendedExitInfo,1,sizeof(tExtendedExitInfo), pfDropFile)==sizeof(tExtendedExitInfo))
               {                 /* transfer info into od_control struct */
                  ODExitInfoExtendedEndian(pExtendedExitInfo, TRUE);
                  ODStringPascalToC(od_control.user_timeofcreation,pExitInfoRecord->bbs.ra.timeofcreation,5);
                  ODStringPascalToC(od_control.user_logonpassword,pExitInfoRecord->bbs.ra.logonpassword,15);
                  od_control.user_wantchat=pExitInfoRecord->bbs.ra.wantchat;

                  od_control.user_deducted_time=pExtendedExitInfo->deducted_time;
                  for(btCount=0;btCount<50;++btCount)
                  {
                     ODStringPascalToC(od_control.user_menustack[btCount],pExtendedExitInfo->menustack[btCount],8);
                  }
                  od_control.user_menustackpointer=pExtendedExitInfo->menustackpointer;
                  ODStringPascalToC(od_control.user_handle,pExtendedExitInfo->userhandle,35);
                  ODStringPascalToC(od_control.user_comment,pExtendedExitInfo->comment,80);
                  ODStringPascalToC(od_control.user_firstcall,pExtendedExitInfo->firstcall,8);
                  memcpy(od_control.user_combinedrecord,pExtendedExitInfo->combinedrecord,25);
                  ODStringPascalToC(od_control.user_birthday,pExtendedExitInfo->birthday,8);
                  ODStringPascalToC(od_control.user_subdate,pExtendedExitInfo->subdate,8);
                  od_control.user_screenwidth=pExtendedExitInfo->screenwidth;
                  od_control.user_msg_area=pExtendedExitInfo->msgarea;
                  od_control.user_file_area=pExtendedExitInfo->filearea;
                  od_control.user_language=pExtendedExitInfo->language;
                  od_control.user_date_format=pExtendedExitInfo->dateformat;
                  ODStringPascalToC(od_control.user_forward_to,pExtendedExitInfo->forwardto,35);
                  memcpy(&od_control.user_error_free,&pExtendedExitInfo->error_free,3);
                  ODStringPascalToC(od_control.user_emsi_crtdef,pExtendedExitInfo->emsi_crtdef,40);
                  ODStringPascalToC(od_control.user_emsi_protocols,pExtendedExitInfo->emsi_protocols,40);
                  ODStringPascalToC(od_control.user_emsi_capabilities,pExtendedExitInfo->emsi_capabilities,40);
                  ODStringPascalToC(od_control.user_emsi_requests,pExtendedExitInfo->emsi_requests,40);
                  ODStringPascalToC(od_control.user_emsi_software,pExtendedExitInfo->emsi_software,40);
                  memcpy(&od_control.user_hold_attr1,&pExtendedExitInfo->hold_attr1,3);

                  od_control.od_ra_info=TRUE;
                  od_control.od_extended_info=TRUE;
                  od_control.od_info_type=RA1EXITINFO;
               }
               else
               {
                  free(pExtendedExitInfo);
                  pExtendedExitInfo = NULL;
               }
            }
         }
      }

      else if(dwFileSize>476)
      {
         if(dwFileSize > sizeof(tExitInfoRecord))
         {
            dwFileSize = sizeof(tExitInfoRecord);
         }

         if(ODReadExitInfoPrimitive(pfDropFile,(int)dwFileSize))
         {
            od_control.user_wantchat=pExitInfoRecord->bbs.qbbs.qwantchat;
            for(btCount=0;btCount<pExitInfoRecord->bbs.qbbs.gosublevel;++btCount)
            {
               ODStringPascalToC(od_control.user_menustack[btCount],pExitInfoRecord->bbs.qbbs.menustack[btCount],8);
            }
            od_control.user_menustackpointer=pExitInfoRecord->bbs.qbbs.gosublevel;
            ODStringPascalToC(od_control.user_menustack[od_control.user_menustackpointer],pExitInfoRecord->bbs.qbbs.menu,8);

            od_control.od_extended_info=TRUE;
            od_control.od_info_type=QBBS275EXITINFO;
            nInitialElapsed=pExitInfoRecord->elapsed;

            od_control.user_rip = pExitInfoRecord->bbs.qbbs.ripactive;
         }
      }

      else if(dwFileSize>=452)
      {
         if(ODReadExitInfoPrimitive(pfDropFile,(int)dwFileSize))
         {
            ODStringPascalToC(od_control.user_timeofcreation,pExitInfoRecord->bbs.ra.timeofcreation,5);
            ODStringPascalToC(od_control.user_logonpassword,pExitInfoRecord->bbs.ra.logonpassword,15);
            od_control.user_wantchat=pExitInfoRecord->bbs.ra.wantchat;

            od_control.od_extended_info=TRUE;
            od_control.od_info_type=EXITINFO;
         }
      }

      od_control.od_page_pausing=od_control.user_attribute&0x04;

      fclose(pfDropFile);
   }
}


#ifdef ODPLAT_NIX
/* ----------------------------------------------------------------------------
 * ODInitTerminalSpeedToBaud()                       *** PRIVATE FUNCTION ***
 *
 * Converts a POSIX termios speed token to its numeric BPS value. POSIX does
 * not require speed_t constants to contain their corresponding numeric rates.
 *
 * Parameters: nSpeed - A speed_t value returned by cfgetispeed() or
 *                      cfgetospeed().
 *
 *     Return: The numeric BPS value, or zero when nSpeed is not recognized.
 */
static DWORD ODInitTerminalSpeedToBaud(speed_t nSpeed)
{
   if(nSpeed == B50) return(50L);
   if(nSpeed == B75) return(75L);
   if(nSpeed == B110) return(110L);
   if(nSpeed == B134) return(134L);
   if(nSpeed == B150) return(150L);
   if(nSpeed == B200) return(200L);
   if(nSpeed == B300) return(300L);
   if(nSpeed == B600) return(600L);
   if(nSpeed == B1200) return(1200L);
   if(nSpeed == B1800) return(1800L);
   if(nSpeed == B2400) return(2400L);
   if(nSpeed == B4800) return(4800L);
   if(nSpeed == B9600) return(9600L);
   if(nSpeed == B19200) return(19200L);
   if(nSpeed == B38400) return(38400L);
#ifdef B7200
   if(nSpeed == B7200) return(7200L);
#endif
#ifdef B14400
   if(nSpeed == B14400) return(14400L);
#endif
#ifdef B28800
   if(nSpeed == B28800) return(28800L);
#endif
#ifdef B57600
   if(nSpeed == B57600) return(57600L);
#endif
#ifdef B76800
   if(nSpeed == B76800) return(76800L);
#endif
#ifdef B115200
   if(nSpeed == B115200) return(115200L);
#endif
#ifdef B230400
   if(nSpeed == B230400) return(230400L);
#endif
#ifdef B460800
   if(nSpeed == B460800) return(460800L);
#endif
#ifdef B500000
   if(nSpeed == B500000) return(500000L);
#endif
#ifdef B576000
   if(nSpeed == B576000) return(576000L);
#endif
#ifdef B921600
   if(nSpeed == B921600) return(921600L);
#endif
#ifdef B1000000
   if(nSpeed == B1000000) return(1000000L);
#endif
#ifdef B1152000
   if(nSpeed == B1152000) return(1152000L);
#endif
#ifdef B1500000
   if(nSpeed == B1500000) return(1500000L);
#endif
#ifdef B2000000
   if(nSpeed == B2000000) return(2000000L);
#endif
#ifdef B2500000
   if(nSpeed == B2500000) return(2500000L);
#endif
#ifdef B3000000
   if(nSpeed == B3000000) return(3000000L);
#endif
#ifdef B3500000
   if(nSpeed == B3500000) return(3500000L);
#endif
#ifdef B4000000
   if(nSpeed == B4000000) return(4000000L);
#endif
   return(0L);
}


/* ----------------------------------------------------------------------------
 * ODInitSelectTerminalBaud()                         *** PRIVATE FUNCTION ***
 *
 * Selects a numeric BPS value for a Unix standard-I/O session. The terminal's
 * input speed takes precedence, followed by its output speed and the nominal
 * 19,200 BPS fallback.
 */
DWORD ODInitSelectTerminalBaud(speed_t nInputSpeed, speed_t nOutputSpeed)
{
   DWORD dwBaud;

   dwBaud = ODInitTerminalSpeedToBaud(nInputSpeed);
   if(dwBaud == 0)
      dwBaud = ODInitTerminalSpeedToBaud(nOutputSpeed);

   return(dwBaud == 0 ? 19200L : dwBaud);
}


/* ----------------------------------------------------------------------------
 * ODInitApplyUserInfo()                              *** PRIVATE FUNCTION ***
 *
 * Applies the available account-directory names to a Unix forced-local
 * session. Existing values are retained when the account record or either
 * source string is unavailable.
 */
void ODInitApplyUserInfo(const struct passwd *pUserInfo)
{
   if(pUserInfo == NULL)
      return;

   if(pUserInfo->pw_name != NULL)
   {
      ODStringCopy(od_control.user_handle, pUserInfo->pw_name,
         sizeof(od_control.user_handle));
   }

   if(pUserInfo->pw_gecos != NULL)
   {
      ODStringCopy(od_control.user_name, pUserInfo->pw_gecos,
         sizeof(od_control.user_name));
   }
}
#endif /* ODPLAT_NIX */


/* ----------------------------------------------------------------------------
 * ODInitPartTwo()                                     *** PRIVATE FUNCTION ***
 *
 * Called by od_init() to carry on with second stage of OpenDoors
 * initialization. The sole reason that this function exists is because some
 * compilers were unable to compile this file with a huge od_init() function
 * that had this code in the main od_init() body.
 *
 * Parameters: none
 *
 *     Return: void
 */
static void ODInitPartTwo(void)
{
   BYTE btCount;
   tODResult Result;
#ifdef ODPLAT_NIX
   struct termios term;
   struct passwd  *uinfo;
#endif

   /* Initialize any colors that haven't already been set. */
   if(!od_control.od_list_title_col) od_control.od_list_title_col = 0x0f;
   if(!od_control.od_continue_col) od_control.od_continue_col = 0x0f;
   if(!od_control.od_list_name_col) od_control.od_list_name_col = 0x0e;
   if(!od_control.od_list_size_col) od_control.od_list_size_col = 0x0d;
   if(!od_control.od_list_comment_col) od_control.od_list_comment_col = 0x03;
   if(!od_control.od_list_offline_col) od_control.od_list_offline_col = 0x0c;
   if(!od_control.od_local_win_col) od_control.od_local_win_col = 0x19;
   if(!od_control.od_menu_title_col) od_control.od_menu_title_col = 0x74;
   if(!od_control.od_menu_border_col) od_control.od_menu_border_col = 0x70;
   if(!od_control.od_menu_text_col) od_control.od_menu_text_col = 0x70;
   if(!od_control.od_menu_key_col) od_control.od_menu_key_col = 0x7f;
   if(!od_control.od_menu_highkey_col) od_control.od_menu_highkey_col = 0x0f;
   if(!od_control.od_menu_highlight_col)
   {
      od_control.od_menu_highlight_col = 0x07;
   }

   /* Color sequence delimiter characters. */
   od_control.od_color_char = '\0';
   od_control.od_color_delimiter = '`';

   /* Enable sysop paging between the hours of 8:00am and 10:00pm. */
   od_control.od_okaytopage = PAGE_USE_HOURS;
   od_control.od_pagestartmin = 480;
   od_control.od_pageendmin = 1320;

   /* Default maximum user inactivity is 200 seconds. */
   od_control.od_inactivity = 200;

   /* Screen clearing is on by default. */
   od_control.od_always_clear = TRUE;
   
   od_control.od_inactive_warning = 10;
   od_control.od_cur_attrib = -1;

   /* Enable screen clearing on door exit. */
   od_control.od_clear_on_exit = TRUE;

   /* RA/QBBS control codes are now disabled by default. */
   od_control.od_no_ra_codes = TRUE;

   if(od_control.od_chat_color1 == 0) od_control.od_chat_color1 = 0x0c;
   if(od_control.od_chat_color2 == 0) od_control.od_chat_color2 = 0x0f;

   /* Set default messages and prompts. */
   od_control.od_before_shell = "\r\nPlease wait a moment...\r\n";
   od_control.od_after_shell = "\r\n...Thanks for waiting\r\n\r\n";
   od_control.od_help_text = "  Alt: [C]hat [H]angup [L]ockout [J]Dos [K]eyboard-Off [D]rop to BBS            ";
   od_control.od_before_chat = "\r\nThe system operator has placed you in chat mode to talk with you:\r\n\r\n";
   od_control.od_after_chat = "\r\nChat mode ended.\r\n\r\n";
   od_control.od_inactivity_timeout = "\r\nMaximum user inactivity time has elapsed, please call again.\r\n\r\n";
   od_control.od_inactivity_warning = "\r\nWARNING: Inactivity timeout in 10 seconds, press a key now to remain online.\r\n\r\n";
   od_control.od_time_warning = "\r\nWARNING: You only have %d minute(s) remaining for this session.\r\n\r\n";
   od_control.od_time_left = "%4d mins  ";
   od_control.od_sysop_next = "[SN] ";
   od_control.od_no_keyboard = "[Keyboard]";
   od_control.od_want_chat = "[Want-Chat]";
   od_control.od_no_time = "\r\nSorry, you have used up all of your time for this session.\r\n\r\n";
   od_control.od_no_sysop = "\r\nSorry, the system operator is not available at this time.\r\n";
   od_control.od_press_key = "Press [Enter] to continue";
   od_control.od_chat_reason = "               Why would you like to chat? (Blank line to cancel)\r\n";
   od_control.od_paging = "\r\nPaging system operator for chat";
   od_control.od_no_response = " No response.\r\n\r\n";
   od_control.od_status_line[0] = "                                                                     [Node:     ";
   od_control.od_status_line[1] = "%s of %s at %lu BPS";
   od_control.od_status_line[2] = "Security:        Time:                                               [F9]=Help ";
   od_control.od_month[0] = "Jan";
   od_control.od_month[1] = "Feb";
   od_control.od_month[2] = "Mar";
   od_control.od_month[3] = "Apr";
   od_control.od_month[4] = "May";
   od_control.od_month[5] = "Jun";
   od_control.od_month[6] = "Jul";
   od_control.od_month[7] = "Aug";
   od_control.od_month[8] = "Sep";
   od_control.od_month[9] = "Oct";
   od_control.od_month[10] = "Nov";
   od_control.od_month[11] = "Dec";
   od_control.od_day[0] = "Sun";
   od_control.od_day[1] = "Mon";
   od_control.od_day[2] = "Tue";
   od_control.od_day[3] = "Wed";
   od_control.od_day[4] = "Thu";
   od_control.od_day[5] = "Fri";
   od_control.od_day[6] = "Sat";
   od_control.od_offline = "[OFFLINE] ";
   od_control.od_continue = "Continue? [Y/n/=]";
   od_control.od_continue_yes = 'y';
   od_control.od_continue_no = 'n';
   od_control.od_continue_nonstop = '=';
   od_control.od_help_text2 = OD_VER_STATUSLINE;
   od_control.od_sending_rip = "Sending RIP file: ";
   od_control.od_hanging_up = "Ending call...";
   od_control.od_exiting = "Program is exiting...";
   if(strlen(od_control.od_disable_dtr) == 0)
   {
      strcpy(od_control.od_disable_dtr, "~+++~  AT&D0|  ATO|");
   }

   nInitialRemaining = od_control.user_timelimit;

   if(od_control.od_maxtime > 0 && od_control.od_maxtime <= 1440)
   {
      if(od_control.user_timelimit > (INT)od_control.od_maxtime)
      {
         od_control.od_maxtime_deduction
            = od_control.user_timelimit-od_control.od_maxtime;
         od_control.user_timelimit
            = od_control.od_maxtime;
      }
   }

   /* If sysop name is unkown, then use the word "Sysop". */
   if(strlen(od_control.sysop_name) == 0)
   {
      strcpy(od_control.sysop_name, "Sysop");
   }

   /* If in foced local mode and user  name has not yet been set. */
   if(od_control.od_force_local && od_control.user_name[0] == '\0')
   {
      /* If name prompting is not disabled, then turn on flag to prompt for */
      /* user's name.                                                       */
      if(!(od_control.od_disable & DIS_NAME_PROMPT))
      {
         bPromptForUserName = TRUE;
      }

      /* Use sysop's name as default user name. */
      if(bSysopNameSet)
      {
         strcpy(od_control.user_name, szForcedSysopName);
      }
      else
      {
         strcpy(od_control.user_name, od_control.sysop_name);
      }
   }

   dwFileBPS = od_control.baud;

   /* Determine desired BPS rate. */
   if(dwForcedBPS != 1)
   {
      if(od_control.od_disable & DIS_LOCAL_OVERRIDE || od_control.baud != 0)
      {
         od_control.baud = dwForcedBPS;
      }
   }

   if(ODReserveSessionInitialize() != kODRCSuccess)
   {
      ODInitError("Unable to initialize shared reservations.");
      exit(od_control.od_errorlevel[1]);
   }

   /* If we are not operating in local mode, then setup for serial I/O. */
#ifndef ODPLAT_NIX
   if(od_control.baud != 0)
#endif
   {
      tComMethod ComMethod;

      /* Attempt to allocate an OpenDoors port object. */
      if(ODComAlloc(&hSerialPort) != kODRCSuccess)
      {
malloc_error:
         ODInitError("Insufficient memory available to start up program.");
         exit(od_control.od_errorlevel[1]);
      }

      /* Set socket I/O method, if specified by user. */
      if(bBBSDevSession && nBBSDevComMethod != kComMethodUnspecified)
      {
         ODComSetPreferredMethod(hSerialPort,
            (tComMethod)nBBSDevComMethod);
      }
      else if(od_control.od_use_socket)
      {
         ODComSetPreferredMethod(hSerialPort, bTelnetSocket
            ? kComMethodTelnetSocket : kComMethodSocket);
      }

#if defined ODPLAT_WIN32 || defined ODPLAT_NIX
      /* Check whether a handle has been provided by the caller. */
      if(od_control.od_open_handle != 0 || bBBSDevOpenHandleSet)
      {
         if(ODComOpenFromExistingHandle(hSerialPort, od_control.od_open_handle)
            != kODRCSuccess)
         {
            ODInitError("Unable to use provided serial port handle.");
            exit(od_control.od_errorlevel[1]);
         }
      }
      else
#endif /* ODPLAT_WIN32 */
      {
         /* Set flow control type. */
         switch(od_control.od_com_flow_control)
         {
            case COM_NO_FLOW:
               ODComSetFlowControl(hSerialPort, FLOW_NONE);
               break;
            case COM_RTSCTS_FLOW:
               ODComSetFlowControl(hSerialPort, FLOW_RTSCTS);
               break;
            case COM_DEFAULT_FLOW:
               /* Don't say anything to the serial I/O module about flow */
               /* control. */
               break;
            default:
               /* We should never get here. */
               ASSERT(FALSE);
         }

         /* Determine desired port. */
         if(nForcedPort != -1) od_control.port=nForcedPort;

         /* Set port number. */
         ODComSetPort(hSerialPort, (BYTE)od_control.port);

         /* If serial port address has been explicitly set, then set user's */
         /* port address in serial port object.                             */
         if(od_control.od_com_address != 0)
         {
            ODComSetPortAddress(hSerialPort,
               bBBSDevSession && nBBSDevComMethod == kComMethodUART
               ? (int)(WORD)od_control.od_com_address
               : od_control.od_com_address);
         }

         /* If serial port IRQ line number has been explicitly set, then */
         /* set user's setting in serial port object.                    */
         if((bBBSDevSession && nBBSDevComMethod == kComMethodUART)
            || (od_control.od_com_irq >= 1 && od_control.od_com_irq < 15))
         {
            ODComSetIRQ(hSerialPort, od_control.od_com_irq);
         }

         /* Set BPS rate, if this is not disabled. */
         if(!(od_control.od_disable&DIS_BPS_SETTING))
         {
            ODComSetSpeed(hSerialPort, od_control.baud);
         }
         else
         {
            ODComSetSpeed(hSerialPort, SPEED_UNSPECIFIED);
         }

         /* Set serial I/O method, if specified by user. */
         if(od_control.od_no_fossil)
         {
            ODComSetPreferredMethod(hSerialPort, kComMethodUART);
         }

         /* Establish default buffer sizes */
         if(od_control.od_com_rx_buf == 0)
            od_control.od_com_rx_buf = 256;
         if(od_control.od_com_tx_buf == 0)
            od_control.od_com_tx_buf = 3072;

         /* Set buffer sizes for serial port. */
         ODComSetRXBuf(hSerialPort, od_control.od_com_rx_buf);
         ODComSetTXBuf(hSerialPort, od_control.od_com_tx_buf);

         /* Set FIFO enabled/disabled and trigger size */
         if(od_control.od_com_no_fifo)
         {
            /* Disable UART FIFO buffers. */
            ODComSetFIFO(hSerialPort, FIFO_DISABLE);
         }
         else
         {
            /* Enable FIFO, setting size of FIFO trigger. */
            switch(od_control.od_com_fifo_trigger)
            {
               case 1:
                  ODComSetFIFO(hSerialPort, FIFO_ENABLE | FIFO_TRIGGER_1);
                  break;
               case 4:
                  ODComSetFIFO(hSerialPort, FIFO_ENABLE | FIFO_TRIGGER_4);
                  break;
               case 8:
                  ODComSetFIFO(hSerialPort, FIFO_ENABLE | FIFO_TRIGGER_8);
                  break;
               case 14:
                  ODComSetFIFO(hSerialPort, FIFO_ENABLE | FIFO_TRIGGER_14);
                  break;
               default:
                  od_control.od_com_fifo_trigger = 4;
                  ODComSetFIFO(hSerialPort, FIFO_ENABLE | FIFO_TRIGGER_4);
            }
         }

#ifdef ODPLAT_DOS
         /* Set od_kernel() to be idle function. */
         ODComSetIdleFunction(hSerialPort, od_kernel);
#endif /* ODPLAT_DOS */

         /* Open serial port. */
         switch(ODComOpen(hSerialPort))
         {
            case kODRCSuccess:
               /* Serial port open succeeded, so proceed with OpenDoors */
               /* initialization.                                       */
               break;
            case kODRCNoMemory:
               goto malloc_error;
            case kODRCNoPortAddress:
               ODInitError("Serial port address is unknown, cannot continue.");
               exit(od_control.od_errorlevel[1]);
               break;
            case kODRCNoUART:
               ODInitError("No UART at specified port address, cannot continue.\n");
               exit(od_control.od_errorlevel[1]);
               break;
            default:
               ODInitError("Unable to access serial port, cannot continue.\n");
               exit(od_control.od_errorlevel[1]);
               break;
         }
      }

      /* Raise DTR signal. */
      ODComSetDTR(hSerialPort, TRUE);

      /* Get serial I/O method actually being used. */
      ODComGetMethod(hSerialPort, (tComMethod *)&ComMethod);
      switch(ComMethod)
      {
         case kComMethodFOSSIL:
            od_control.od_com_method = COM_FOSSIL;
            break;
         case kComMethodUART:
            od_control.od_com_method = COM_INTERNAL;
            break;
         case kComMethodWin32:
            od_control.od_com_method = COM_WIN32;
            break;
         case kComMethodDoor32:
            od_control.od_com_method = COM_DOOR32;
            break;
			case kComMethodSocket:
				od_control.od_com_method = COM_SOCKET;	/* Why are using doubling up constants here? */
				break;
		 case kComMethodStdIO:
		    od_control.od_com_method = COM_STDIO;
			break;
         default:
            ODInitError("No method of accessing serial port, cannot continue.\n");
            exit(od_control.od_errorlevel[1]);
            break;
      }
   }

#ifndef ODPLAT_WIN32
   /* Traditional non-Windows local mode always has a local display. */
   if(od_control.baud == 0)
      od_control.od_silent_mode = FALSE;
#endif

   /* Setup local screen. */
   Result = ODScrnInitialize();
   if(Result != kODRCSuccess)
   {
      od_control.od_error = ERR_GENERALFAILURE;
      ODInitError("Unable to initialize the OpenDoors local display.");
      exit(od_control.od_errorlevel[1]);
   }
#ifdef ODPLAT_WIN32
   if(ODPlatGetWindowsSubsystem() == kODWindowsSubsystemConsole)
   {
      ODScrnSetBoundary(1, 1, 80, 23);
      ODSessionScreenInitialize(od_control.user_screenwidth,
         od_control.user_screen_length);
   }
   else
   {
      ODScrnSetBoundary(1, 1, 80, 25);
      ODSessionScreenInitialize(80, 25);
   }
#else /* !ODPLAT_WIN32 */
#if defined(OD_TEXTMODE)
   ODScrnSetBoundary(1, 1, 80, 23);
   ODSessionScreenInitialize(80, 23);
#else /* !OD_TEXTMODE */
   ODScrnSetBoundary(1, 1, 80, 25);
   ODSessionScreenInitialize(80, 25);
#endif /* !OD_TEXTMODE */
#endif /* !ODPLAT_WIN32 */

#ifndef ODPLAT_WIN32
   if(bPreset)
   {
      atexit(ODAtExitCallback);
      bPreset = FALSE;
   }
#endif /* !ODPLAT_WIN32 */

   /* Setup remote terminal for ANSI graphics if operating in RIP mode. */
   if(od_control.user_rip)
   {
      od_clr_scr();
   }

   /* Initialize the cooperative kernel before application callbacks can
    * enter other OpenDoors APIs. */
   Result = ODKrnlInitialize();
   if(Result != kODRCSuccess)
   {
      od_control.od_error = ERR_GENERALFAILURE;
      bODInitialized = FALSE;
      ODInitError("Unable to start the OpenDoors kernel.");
      return;
   }

   if(bBBSDevSession)
   {
      if(bBBSDevDeadlineSet)
      {
         time_t nCurrentTime = time(NULL);
         double dSecondsRemaining = difftime(nBBSDevDeadline, nCurrentTime);
         INT nMinutesRemaining;

         od_control.od_disable &= ~DIS_TIMEOUT;
         if(dSecondsRemaining <= 0.0)
         {
            nMinutesRemaining = 0;
            nNextTimeDeductTime = nCurrentTime;
         }
         else
         {
            double dMinutesRemaining = (dSecondsRemaining + 59.0) / 60.0;
            nMinutesRemaining = dMinutesRemaining > 32767.0
               ? 32767 : (INT)dMinutesRemaining;
            nNextTimeDeductTime = nBBSDevDeadline
               - (time_t)(nMinutesRemaining - 1) * 60L;
         }
         od_control.user_timelimit = nMinutesRemaining;
         nInitialRemaining = (INT16)nMinutesRemaining;
      }
      else
         od_control.od_disable |= DIS_TIMEOUT;
   }

#ifdef ODPLAT_NIX
   if(bPromptForUserName)
   {
      od_control.od_com_method=COM_STDIO;
      od_control.baud=19200;
      gethostname(od_control.system_name,sizeof(od_control.system_name));
      od_control.system_name[sizeof(od_control.system_name)-1]=0;
      if(isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &term) == 0)
      {
         od_control.baud = ODInitSelectTerminalBaud(cfgetispeed(&term),
            cfgetospeed(&term));
      }
      uinfo=getpwuid(getuid());
      ODInitApplyUserInfo(uinfo);
   }
#else
   if(bPromptForUserName
#ifdef ODPLAT_WIN32
      && ODPlatGetWindowsSubsystem() == kODWindowsSubsystemConsole
      && !od_control.od_silent_mode
#endif
      )
   {
      void *pWindow = ODScrnCreateWindow(10, 8, 70, 15,
         od_control.od_local_win_col, od_control.od_prog_name,
         od_control.od_local_win_col);

      if(pWindow != NULL)
      {
         ODScrnSetCursorPos(12, 9);
         ODScrnDisplayString("This program has been started in local mode,");
         ODScrnSetCursorPos(12, 10);
         ODScrnDisplayString("independently of a BBS system. When operating in this");
         ODScrnSetCursorPos(12, 11);
         ODScrnDisplayString("mode, you may specify what name you should be known to");
         ODScrnSetCursorPos(12, 12);
         ODScrnDisplayString("the program by.");
         ODScrnSetCursorPos(12, 14);
         ODScrnDisplayString("Your name:");
         ODScrnLocalInput(23, 14, od_control.user_name, 35);
         ODStringCopy(od_control.user_handle, od_control.user_name,
            sizeof(od_control.user_name));
         ODScrnDestroyWindow(pWindow);
         ODScrnSetCursorPos(1, 1);
         ODScrnSetAttribute(0x07);
      }
   }
#endif /* !ODPLAT_NIX */

#if defined(OD_TEXTMODE) || defined(ODPLAT_WIN32)
#ifdef ODPLAT_WIN32
   if(ODPlatGetWindowsSubsystem() == kODWindowsSubsystemConsole)
   {
#endif
   /* Setup sysop status line/function key personality. */
   if(pfSetPersonality == NULL)
   {
no_default:
      if (od_control.od_default_personality == NULL)
      {
#ifdef ODPLAT_WIN32
         pfCurrentPersonality = ODMultiResolvePersonality(NULL);
#else
         pfCurrentPersonality = pdef_opendoors;
#endif
      }
      else
      {
         pfCurrentPersonality = ODMultiResolvePersonality(
            od_control.od_default_personality);
      }
      (*pfCurrentPersonality)(20);
      if(bRAStatus)
      {
         od_set_statusline(btRAStatusToSet);
      }
      else
      {
         od_set_statusline(0);
      }
   }
   else
   {
      if(!((*pfSetPersonality)(szDesiredPersonality)))
      {
         goto no_default;
      }
   }
#ifdef ODPLAT_WIN32
   }
#endif
#endif /* OD_TEXTMODE || ODPLAT_WIN32 */

   /* If connect speed has not been set yet, then set it to the */
   /* serial port speed.                                        */
   if(od_control.od_connect_speed == 0)
   {
      od_control.od_connect_speed = od_control.baud;
   }

   /* Initialize the array of characters to use for drawing boxes, */
   /* window and menu boarders, etc.                               */
   od_control.od_box_chars[0] = 218U;
   od_control.od_box_chars[1] = 196U;
   od_control.od_box_chars[2] = 191U;
   od_control.od_box_chars[3] = 179U;
   od_control.od_box_chars[4] = 192U;
   od_control.od_box_chars[5] = 217U;

   /* Enable pausing and stoping of listing. */
   od_control.od_list_stop = TRUE;
   od_control.od_list_pause = TRUE;

   /* Initialize array of logfile messages. */
   for(btCount = 0; btCount < DIM(apszLogMessages); ++btCount)
   {
      if(od_control.od_logfile_messages[btCount] == NULL)
      {
         od_control.od_logfile_messages[btCount]
            = apszLogMessages[btCount];
      }
   }

   /* Set log file name to default, if none has been specified already. */
   if(strlen(od_control.od_logfile_name) == 0)
   {
      strcpy(od_control.od_logfile_name, "DOOR.LOG");
   }

   /* If OpenDoors log file system is installed, then start it up. */
   if(od_control.od_logfile != NULL)
   {
       (*(OD_COMPONENT_CALLBACK *)od_control.od_logfile)();
   }

#ifdef ODPLAT_WIN32
   ODStringCopy(szWindowsStartupUserName, od_control.user_name,
      sizeof(szWindowsStartupUserName));
   bWindowsStartupCancelled = FALSE;

   if(ODPlatGetWindowsSubsystem() == kODWindowsSubsystemConsole)
   {
      /* od_init() is not enclosed by an API boundary, so publish the first
       * completed console generation explicitly. */
      ODScrnPublish();
   }
   else if(!ODKrnlRefreshUIState())
   {
      ODKrnlShutdown();
      od_control.od_error = ERR_GENERALFAILURE;
      bODInitialized = FALSE;
      ODInitError("Unable to initialize the OpenDoors local window state.");
      return;
   }

   /* Start the Windows command UI after the application dispatcher is ready. */
   if(ODPlatGetWindowsSubsystem() == kODWindowsSubsystemGUI
      && !od_control.od_silent_mode)
   {
      HANDLE h = GetModuleHandle(OD_DLL_NAME);
      if(h == NULL)
         h = GetModuleHandle(NULL);
      /* Hand the completed initialization screen to the UI before its first
       * paint. od_init() itself is not enclosed by an OD_API_EXIT boundary. */
      ODScrnPublish();
      Result = ODFrameStart(h, &hFrameThread);
      if(Result != kODRCSuccess)
      {
         ODKrnlShutdown();
         if(bWindowsStartupCancelled)
            exit(od_control.od_errorlevel[1]);
         od_control.od_error = ERR_GENERALFAILURE;
         bODInitialized = FALSE;
         ODInitError("Unable to start the OpenDoors local window.");
         return;
      }

      if(bPromptForUserName)
      {
         ODStringCopy(od_control.user_name, szWindowsStartupUserName,
            sizeof(od_control.user_name));
         ODStringCopy(od_control.user_handle, od_control.user_name,
            sizeof(od_control.user_handle));
         (void)ODKrnlRefreshUIState();
      }
   }
#endif /* ODPLAT_WIN32 */

}


/* ----------------------------------------------------------------------------
 * ODInitError()
 *
 * Called to display an error message when OpenDoors initialization fails,
 * forcing the program to exit.
 *
 * Parameters: pszErrorText - Pointer to the error message explaining the
 *                            reason for failure.
 *
 *     Return: void
 */
void ODInitError(char *pszErrorText)
{
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   printf("%s: %s\n", od_control.od_prog_name, pszErrorText);
   if(bParsedCmdLine)
   {
      printf("Use the -HELP command line option for help, or -LOCAL for local mode.\n");
   }
#endif
#ifdef ODPLAT_WIN32
   char *pszMessage;
   if(ODPlatGetWindowsSubsystem() == kODWindowsSubsystemConsole)
   {
      fprintf(stderr, "%s: %s\n", od_control.od_prog_name, pszErrorText);
      if(bParsedCmdLine)
         fputs("Use the -HELP command line option for help, or -LOCAL for local mode.\n",
            stderr);
      return;
   }
   if(!bParsedCmdLine ||
      (pszMessage = malloc(strlen(pszErrorText) + 80)) == NULL)
   {
      MessageBox(NULL, pszErrorText, od_control.od_prog_name,
         MB_ICONSTOP | MB_OK | MB_TASKMODAL);
   }
   else
   {
      sprintf(pszMessage,
         "%s\nUse the -HELP command line option for help, or -LOCAL for local mode.",
         pszErrorText);
      MessageBox(NULL, pszMessage, od_control.od_prog_name,
         MB_ICONSTOP | MB_OK | MB_TASKMODAL);
      free(pszMessage);
   }
#endif
#ifdef ODPLAT_NIX
   fwrite(pszErrorText,strlen(pszErrorText),1,stderr);
#endif
}


/* ----------------------------------------------------------------------------
 * ODInitLoginDlgProc()
 *
 * Dialog box proceedure for the local mode login dialog box.
 *
 * Parameters: hwndDlg  - Window handle to the dialog box.
 *
 *             uMsg     - Message ID.
 *
 *             wParam   - First message parameter.
 *
 *             lParam   - Second message parameter.
 *
 *     Return: TRUE if message is processed, FALSE otherwise.
 */
#ifdef ODPLAT_WIN32
INT_PTR CALLBACK ODInitLoginDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
   LPARAM lParam)
{
   switch(uMsg)
   {
      case WM_INITDIALOG:
      {
         char szProgramName[sizeof(od_control.od_prog_name)];
         tODUIState State;

         ODKrnlGetUIState(&State);
         ODStringCopy(szProgramName, State.szProgramName,
            sizeof(szProgramName));

         ODFrameCenterWindowInParent(hwndDlg);

         /* Set the title of the dialog box to the name of this program. */
         SetWindowText(hwndDlg, szProgramName);

         /* The initial text in the user name dialog box should be the */
         /* default user name.                                         */
         SetWindowText(GetDlgItem(hwndDlg, IDC_USER_NAME),
            szWindowsStartupUserName);

         /* Limit the number of characters that may be entered as the */
         /* user's name to the maximum size of the string.            */
         SendMessage(GetDlgItem(hwndDlg, IDC_USER_NAME), EM_LIMITTEXT,
            sizeof(od_control.user_name), 0L);

         return(TRUE);
      }

      case WM_COMMAND:
         /* If a command has been chosen. */
         switch(LOWORD(wParam))
         {
            case IDOK:
            {
               /* If the OK button has been pressed, obtain the entered */
               /* user name.                                            */
               GetWindowText(GetDlgItem(hwndDlg, IDC_USER_NAME),
                  szWindowsStartupUserName,
                  sizeof(szWindowsStartupUserName));

               /* Now close the dialog. */
               EndDialog(hwndDlg, IDOK);
               break;
            }

            case IDCANCEL:
               /* If the Cancel button has benn pressed, close the dialog. */
               bWindowsStartupCancelled = TRUE;
               EndDialog(hwndDlg, IDCANCEL);
               break;
         }
         return(TRUE);

      default:
         /* Otherwise, indicate that this message has not been processed. */
         return(FALSE);
   }
}
#endif /* ODPLAT_WIN32 */
