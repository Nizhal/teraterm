/*
 * (C) 2024- TeraTerm Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
//#include <winsock2.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <crtdbg.h>

#include "teraterm.h"
#include "tttypes.h"
#include "ttlib.h"
#include "ttplugin.h"
#include "codeconv.h"
#include "asprintf.h"
#include "ttplug.h"
#include "vtwin.h"
#include "vtterm.h"

#include "ttplugin_api.h"
#include "ttplugin_api_i.h"

// extern "C"
extern "C" {
static void SetHelpId(TComVar *cv, UINT data);
static void GetCharset(TComVar *cv, IdLanguage *lang, WORD *send_code, WORD *receive_code);
static void SetCharset(TComVar *cv, IdLanguage lang, WORD send_code, WORD receive_code);
}

static void SetHelpId(TComVar *cv, UINT data)
{
	VtwinSetHelpId(data);
}

static void GetCharset(TComVar *cv, IdLanguage *lang, WORD *send_code, WORD *receive_code)
{
	TTTSet *pts = cv->ts;
	*lang = (IdLanguage)pts->Language;
	*send_code = pts->KanjiCodeSend;
	*receive_code = pts->KanjiCode;
}

static void SetCharset(TComVar *cv, IdLanguage lang, WORD send_code, WORD receive_code)
{
	TTTSet *pts = cv->ts;
	pts->Language = (WORD)lang;
	pts->KanjiCodeSend = send_code;
	pts->KanjiCode = receive_code;
	ResetCharSet();
	cv->CRSend = pts->CRSend;	//?
}

static const TTPluginAPIs PluginApiV1Table= {
	SetHelpId,
	GetCharset,
	SetCharset,
};

//__declspec(dllexport) BOOL PluginGetAPI(const char *name, size_t size, void **ptr)
int PluginAPI(const char *reserve, void **ptr)
{
	*ptr = (void *) & PluginApiV1Table;
	return 1;
}
