/*===========================================================================
 *  FileName : error.c
 *  About    : handling errors
 *
 *  Copyright (C) 2005      by Kazuki Ohta (mover@hct.zaq.ne.jp)
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of authors nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
===========================================================================*/

/*=======================================
  System Include
=======================================*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*=======================================
  Local Include
=======================================*/
#include "sigscheme.h"
#include "sigschemeinternal.h"

/*=======================================
  File Local Struct Declarations
=======================================*/

/*=======================================
  File Local Macro Declarations
=======================================*/
#define SCM_ERR_HEADER "Error: "
#define SCM_BACKTRACE_HEADER "**** BACKTRACE ****\n"

/*=======================================
  Variable Declarations
=======================================*/
ScmObj scm_std_error_port  = NULL;
ScmObj scm_current_error_port  = NULL;

/*=======================================
  File Local Function Declarations
=======================================*/

/*=======================================
  Function Implementations
=======================================*/
int SigScm_Die(const char *msg, const char *filename, int line) {
    if (SigScm_DebugCategories() & SCM_DBG_ERRMSG) {
        SigScm_ShowErrorHeader();
        SigScm_ErrorPrintf("SigScheme Died : %s (file : %s, line : %d)\n",
                           msg, filename, line);
    }

    if (SigScm_DebugCategories() & SCM_DBG_BACKTRACE)
        SigScm_ShowBacktrace();

    exit(EXIT_FAILURE);
    /* NOTREACHED */
    return 1;
}

void SigScm_Error(const char *msg, ...)
{
    va_list va;

    if (SigScm_DebugCategories() & SCM_DBG_ERRMSG) {
        SigScm_ShowErrorHeader();

        va_start(va, msg);
        SigScm_VErrorPrintf(msg, va);
        va_end(va);

        SigScm_ErrorNewline();
    }

    if (SigScm_DebugCategories() & SCM_DBG_BACKTRACE)
        SigScm_ShowBacktrace();

    /* TODO: throw an exception instead of exiting */
    exit(EXIT_FAILURE);
}

void SigScm_ErrorObj(const char *msg, ScmObj obj)
{
    if (SigScm_DebugCategories() & SCM_DBG_ERRMSG) {
        SigScm_ShowErrorHeader();
        SigScm_ErrorPrintf(msg);
        SigScm_WriteToPort(scm_current_error_port, obj);
        SigScm_ErrorNewline();
    }
   
    if (SigScm_DebugCategories() & SCM_DBG_BACKTRACE)
        SigScm_ShowBacktrace();
 
    /* TODO: throw an exception instead of exiting */
    exit(EXIT_FAILURE);
}

void SigScm_ShowBacktrace(void)
{
#if SCM_DEBUG
    struct trace_frame *f;

    SigScm_ErrorPrintf(SCM_BACKTRACE_HEADER);

    /* show each frame's obj */
    for (f = scm_trace_root; f; f = f->prev) {
        SigScm_WriteToPort(scm_current_error_port, f->obj);
        SigScm_ErrorNewline();
    }
#endif
}

void SigScm_ShowErrorHeader(void)
{
    SigScm_ErrorPrintf(SCM_ERR_HEADER);
}

void SigScm_ErrorPrintf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    SigScm_VErrorPrintf(fmt, args);
    va_end(args);
}

void SigScm_VErrorPrintf(const char *fmt, va_list args)
{
    FILE *err;

    if (scm_current_error_port) {
        err = SCM_PORTINFO_FILE(scm_current_error_port);
        vfprintf(err, fmt, args);
#if SCM_VOLATILE_OUTPUT
        fflush(err);
#endif
    }
}

void SigScm_ErrorNewline(void)
{
    FILE *err;

    if (scm_current_error_port) {
        err = SCM_PORTINFO_FILE(scm_current_error_port);
        fputc('\n', err);
#if SCM_VOLATILE_OUTPUT
        fflush(err);
#endif
    }
}
