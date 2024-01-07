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
 *	�v���O�C�����痘�p�ł���API���X�g
 */
typedef struct TTPluginAPIsTag {
	/**
	 *	�w���vID��ݒ�
	 */
	void (*SetHelpID)(TComVar *cv, UINT data);

	/**
	 *	�����R�[�h�̐ݒ���擾
	 */
	void (*GetCharset)(TComVar *cv, IdLanguage *lang, WORD *send_code, WORD *receive_code);

	/**
	 *	�����R�[�h��ݒ肷��
	 */
	void (*SetCharset)(TComVar *cv, IdLanguage lang, WORD send_code, WORD receive_code);
} TTPluginAPIs;

/**
 *	Tera Term Plugin�pAPI
 *	�v���O�C�����痘�p�ł���API���X�g�ւ̃|�C���^�擾���邽�߂�API
 *	@param[in]	reserve		���U�[�u, 0���w�肷��
 *							������ɂ��Ď擾�ł��� ptr �̎�ނ��w��ł���悤�ɂ��邩
 *	@param[out]	ptr			API���X�g�ւ̃|�C���^ (TTPluginAPIs)
 *							���[�N�Ȃǂ��擾�ł���悤�ɂ��邩
 */
typedef int (TTPluginAPI)(const char *reserve, void **ptr);

/**
 *	TTXBindProc() �̑�3����
 */
typedef struct TTPluginDataTag {
	size_t reserve;		// 0
	TComVar *pcv;
	TTPluginAPI *PluginAPI;
} TTPluginData;

#ifdef __cplusplus
}
#endif
