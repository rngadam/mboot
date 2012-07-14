/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#include "types.h"

void ShellAllClear(void);

U32 ShellPrintf(const char* format,...);
void ShellSendString(S8 *pt);
void ShellGetString(S8 *string);
S32 ShellGetIntNum(void);

void ShellPutByte(S8 c);
S8 ShellGetByte(void);
S8 ShellGetKey(void);

void String_Build(char string[16384], const char *fmt,...);

#endif /* __SHELL_H__ */
