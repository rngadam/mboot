/*
 *------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */

#ifndef __TRACE_H__
#define __TRACE_H__

#include "types.h"
#include "option.h"
#include "shell.h"

#define PRINT_F(...)				{ShellPrintf(__VA_ARGS__);}

#define SHELL_INF(...)			{ShellPrintf(__VA_ARGS__); ShellPrintf("\n\r");}
#define SHELL_EMPTY()				{ShellPrintf("\n\r");}
#define SHELL_BNNER()				{ShellPrintf("==========================================\n\r");}

#define TRACE_MSG(...)			{ShellPutByte(0x1B); ShellPrintf("[0;0;36minfo:"); ShellPutByte(0x1B); ShellPrintf("[m "); ShellPrintf(__VA_ARGS__); ShellPrintf("\n\r");}
#define TRACE_MSG_NONR(...)	{ShellPutByte(0x1B); ShellPrintf("[0;0;36minfo:"); ShellPutByte(0x1B); ShellPrintf("[m "); ShellPrintf(__VA_ARGS__);}
#define TRACE_FIN(...)			{ShellPutByte(0x1B); ShellPrintf("[0;0;32mdone: "); ShellPrintf(__VA_ARGS__); ShellPutByte(0x1B); ShellPrintf("[m"); ShellPrintf("\n\r");}
#define TRACE_WRN(...)			{ShellPutByte(0x1B); ShellPrintf("[0;0;33mwarn:"); ShellPutByte(0x1B); ShellPrintf("[m "); ShellPrintf(__VA_ARGS__); ShellPrintf("\n\r");}
#define TRACE_ERR(...)			{ShellPutByte(0x1B); ShellPrintf("[0;0;31mERR!: "); ShellPrintf(__VA_ARGS__); ShellPutByte(0x1B); ShellPrintf("[m"); ShellPrintf("\n\r");}

#if (DEBUG_ENABLE == 1)

	#define DEBUG_MSG(...)		{ShellPutByte(0x1B); ShellPrintf("[47;30m%s (%d):", __MODULE__, __LINE__); ShellPutByte(0x1B); ShellPrintf("[m "); ShellPrintf(__VA_ARGS__); ShellPrintf("\n\r");}
	#define DEBUG_WP(...)		{ShellPrintf(__VA_ARGS__);}
	
	#define ASSERT(condition, ...)  { \
	    if (!(condition)) { \
	        ShellPrintf("ASSERT: "); \
	        ShellPrintf(__VA_ARGS__); \
	        while (1); \
	    } \
	}
	#define SANITY_ERROR            "Sanity check failed at %s:%d\n\r"
	
	/// Performs the same duty as the ASSERT() macro, except a default error
	/// message is output if the condition is false.
	/// \param condition  Condition to verify.
	#define SANITY_CHECK(condition) ASSERT(condition, SANITY_ERROR, __FILE__, __LINE__)
		
#else

	#define DEBUG_MSG(...)		{ }
	#define DEBUG_WP(...)		{ }
	#define ASSERT(...)
	#define SANITY_CHECK(...)

#endif

#endif /* __TRACE_H__ */
