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
#pragma once

#include <windows.h>

#include "tttypes.h"
#include "tttypes_charset.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	プラグインから利用できるAPIリスト
 */
typedef struct TTPluginAPIsTag {
	/**
	 *	ヘルプIDを設定
	 */
	void (*SetHelpID)(TComVar *cv, UINT data);

	/**
	 *	文字コードの設定を取得
	 */
	void (*GetCharset)(TComVar *cv, IdLanguage *lang, WORD *send_code, WORD *receive_code);

	/**
	 *	文字コードを設定する
	 */
	void (*SetCharset)(TComVar *cv, IdLanguage lang, WORD send_code, WORD receive_code);
} TTPluginAPIs;

/**
 *	Tera Term Plugin用API
 *	プラグインから利用できるAPIリストへのポインタ取得するためのAPI
 *	@param[in]	reserve		リザーブ, 0を指定する
 *							文字列にして取得できる ptr の種類を指定できるようにするか
 *	@param[out]	ptr			APIリストへのポインタ (TTPluginAPIs)
 *							ワークなども取得できるようにするか
 */
typedef int (TTPluginAPI)(const char *reserve, void **ptr);

/**
 *	TTXBindProc() の第3引数
 */
typedef struct TTPluginDataTag {
	size_t reserve;		// 0
	TComVar *pcv;
	TTPluginAPI *PluginAPI;
} TTPluginData;

#ifdef __cplusplus
}
#endif
