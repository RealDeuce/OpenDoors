/* OpenDoors Online Software Programming Toolkit
 * (C) Copyright 1991 - 1999 by Brian Pirie.
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
 *        File: ODKrnl.h
 *
 * Description: Contains internal definitions related to odkrnl.c.
 *
 *   Revisions: Date          Ver   Who  Change
 *              ---------------------------------------------------------------
 *              Jan 01, 1995  6.00  BP   Split off from odcore.c and oddoor.h
 *              Dec 12, 1995  6.00  BP   Added entry, exit and kernel macros.
 *              Jan 12, 1996  6.00  BP   Added bOnlyShiftArrow.
 *              Feb 19, 1996  6.00  BP   Changed version number to 6.00.
 *              Mar 03, 1996  6.10  BP   Begin version 6.10.
 *              Mar 13, 1996  6.10  BP   bOnlyShiftArrow -> nArrowUseCount.
 */

#ifndef _INC_ODKRNL
#define _INC_ODKRNL

#include "ODPlat.h"

#define OD_MIN_USER_TIME_MINUTES 0
#define OD_MAX_USER_TIME_MINUTES 1440

/* Global kernel-related variables. */
extern tODTimer RunKernelTimer;
extern time_t nNextTimeDeductTime;
extern char chLastControlKey;
extern INT nArrowUseCount;
extern BOOL bForceStatusUpdate;
extern BOOL bSysopColor;

/* Chat mode global variables. */
extern BOOL bIsShell;
extern BOOL bChatted;

/* Kernel function prototypes. */
tODResult ODKrnlInitialize(void);
tODResult ODKrnlRestart(void);
void ODKrnlShutdown(void);
void ODKrnlHandleLocalKey(WORD wKeyCode);
void ODKrnlEndChatMode(void);
void ODKrnlForceOpenDoorsShutdown(BYTE btReasonForShutdown);
void ODStatStartArrowUse(void);
void ODStatEndArrowUse(void);
void ODKrnlDispatchPending(BOOL bAllowApplicationCallbacks);
void ODKrnlRequestChatToggle(void);
void ODKrnlRequestTimeUpdate(void);
void ODKrnlRequestKeyboardToggle(void);
void ODKrnlRequestSysopNextToggle(void);
void ODKrnlRequestInactivityToggle(void);
void ODKrnlRequestTimeAdjustment(INT nMinutes);
void ODKrnlRequestTimeValue(INT nMinutes);
void ODKrnlRequestLockout(void);

/* Macro used to generate the appropriate code (if any) to call */
/* the OpenDoors kernel from within OpenDoors code.             */
#ifdef OD_MULTITHREADED
#define CALL_KERNEL_IF_NEEDED()
#else /* !OD_MULTITHREADED */
#define CALL_KERNEL_IF_NEEDED()     od_kernel()
#endif /* !OD_MULTITHREADED */

#include "ODSync.h"

#define OD_API_ENTRY()              ODSyncAPIEntry();
#define OD_API_EXIT()               ODSyncAPIExit();

#endif /* _INC_ODKRNL */
