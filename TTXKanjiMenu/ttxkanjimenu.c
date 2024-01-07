/*
 * TTX KanjiMenu Plugin
 *    Copyright (C) 2007 Sunao HARA (naoh@nagoya-u.jp)
 *    (C) 2007- TeraTerm Project
 */

//// ORIGINAL SOURCE CODE: ttxtest.c

/* Tera Term extension mechanism
   Robert O'Callahan (roc+tt@cs.cmu.edu)

   Tera Term by Takashi Teranishi (teranishi@rikaxp.riken.go.jp)
*/

#include "teraterm.h"
#include "tttypes.h"
#include "ttplugin.h"
#include "tt_res.h"
#include "i18n.h"
#include "inifile_com.h"
#include "ttplugin_api.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IniSection "TTXKanjiMenu"
#define ORDER 5000

#define ID_MI_KANJIRECV 54009
#define ID_MI_KANJISEND 54109
#define ID_MI_USEONESETTING 54200

#define UpdateRecvMenu(val)	\
	CheckMenuRadioItem(pvar->hmEncode, \
	                   ID_MI_KANJIRECV + IdSJIS, \
	                   ID_MI_KANJIRECV + IdUTF8, \
	                   ID_MI_KANJIRECV + (val), \
	                   MF_BYCOMMAND)
#define UpdateSendMenu(val)	\
	CheckMenuRadioItem(pvar->hmEncode, \
	                   ID_MI_KANJISEND + IdSJIS, \
	                   ID_MI_KANJISEND + IdUTF8, \
	                   ID_MI_KANJISEND + (val), \
	                   MF_BYCOMMAND)

// ���j���[���ږ��̏��
typedef struct {
	int menuID;
	const char *menuStr;
} KmTextInfo;

// ��M�����R�[�h (���{��)
static const KmTextInfo MenuNameRecvJ[] = {
	{ ID_MI_KANJIRECV + IdSJIS,  "Recv: &Shift_JIS" },
	{ ID_MI_KANJIRECV + IdEUC,   "Recv: &EUC-JP" },
	{ ID_MI_KANJIRECV + IdJIS,   "Recv: &JIS" },
	{ ID_MI_KANJIRECV + IdUTF8,  "Recv: &UTF-8" },
};

// ��M�����R�[�h (�؍���)
static const KmTextInfo MenuNameRecvK[] = {
	{ ID_MI_KANJIRECV + IdSJIS,  "Recv: &KS5601" },
	{ ID_MI_KANJIRECV + IdUTF8,  "Recv: &UTF-8" },
};

// ���M�����R�[�h (���{��)
static const KmTextInfo MenuNameSendJ[] = {
	{ ID_MI_KANJISEND + IdSJIS,  "Send: S&hift_JIS" },
	{ ID_MI_KANJISEND + IdEUC,   "Send: EU&C-JP" },
	{ ID_MI_KANJISEND + IdJIS,   "Send: J&IS" },
	{ ID_MI_KANJISEND + IdUTF8,  "Send: U&TF-8" }
};

// ���M�����R�[�h (�؍���)
static const KmTextInfo MenuNameSendK[] = {
	{ ID_MI_KANJISEND + IdSJIS,  "Send: K&S5601" },
	{ ID_MI_KANJISEND + IdUTF8,  "Send: U&TF-8" }
};

// ����M�����R�[�h (���{��)
static const KmTextInfo MenuNameOneJ[] = {
	{ ID_MI_KANJIRECV + IdSJIS,  "Recv/Send: &Shift_JIS" },
	{ ID_MI_KANJIRECV + IdEUC,   "Recv/Send: &EUC-JP" },
	{ ID_MI_KANJIRECV + IdJIS,   "Recv/Send: &JIS" },
	{ ID_MI_KANJIRECV + IdUTF8,  "Recv/Send: &UTF-8" },
};

// ����M�����R�[�h (�؍���)
static const KmTextInfo MenuNameOneK[] = {
	{ ID_MI_KANJIRECV + IdSJIS,  "Recv/Send: &KS5601" },
	{ ID_MI_KANJIRECV + IdUTF8,  "Recv/Send: &UTF-8" },
};

// ���j���[���̂̍��ۉ��p��� (���{��)
static DlgTextInfo MenuTitleInfoJ[] = {
	{ -1, "MENU_KANJI" }
};

// ���j���[���̂̍��ۉ��p��� (�؍���)
static DlgTextInfo MenuTitleInfoK[] = {
	{ -1, "MENU_KANJI_K" }
};

// ���j���[���̊e���ڂ̍��ۉ��p��� (����M������/���{��)
static const DlgTextInfo MenuInfoSeparateJ[] = {
	// ��M
	{ ID_MI_KANJIRECV + IdSJIS,  "MENU_RECV_SJIS" },
	{ ID_MI_KANJIRECV + IdEUC,   "MENU_RECV_EUCJP" },
	{ ID_MI_KANJIRECV + IdJIS,   "MENU_RECV_JIS" },
	{ ID_MI_KANJIRECV + IdUTF8,  "MENU_RECV_UTF8" },
	// ���M
	{ ID_MI_KANJISEND + IdSJIS,  "MENU_SEND_SJIS" },
	{ ID_MI_KANJISEND + IdEUC,   "MENU_SEND_EUCJP" },
	{ ID_MI_KANJISEND + IdJIS,   "MENU_SEND_JIS" },
	{ ID_MI_KANJISEND + IdUTF8,  "MENU_SEND_UTF8" },
	// UseOneSetting
	{ ID_MI_USEONESETTING, "MENU_USE_ONE_SETTING" }
};

// ���j���[���̊e���ڂ̍��ۉ��p��� (����M������/�؍���)
static const DlgTextInfo MenuInfoSeparateK[] = {
	// ��M
	{ ID_MI_KANJIRECV + IdSJIS,  "MENU_RECV_KS5601" },
	{ ID_MI_KANJIRECV + IdUTF8,  "MENU_RECV_UTF8" },
	// ���M
	{ ID_MI_KANJISEND + IdSJIS,  "MENU_SEND_KS5601" },
	{ ID_MI_KANJISEND + IdUTF8,  "MENU_SEND_UTF8" },
	// UseOneSetting
	{ ID_MI_USEONESETTING, "MENU_USE_ONE_SETTING" }
};

// ���j���[���̊e���ڂ̍��ۉ��p��� (����M���ʎ�/���{��)
static const DlgTextInfo MenuInfoOneJ[] = {
	// ����M
	{ ID_MI_KANJIRECV + IdSJIS,  "MENU_SJIS" },
	{ ID_MI_KANJIRECV + IdEUC,   "MENU_EUCJP" },
	{ ID_MI_KANJIRECV + IdJIS,   "MENU_JIS" },
	{ ID_MI_KANJIRECV + IdUTF8,  "MENU_UTF8" },
	// UseOneSetting
	{ ID_MI_USEONESETTING, "MENU_USE_ONE_SETTING" }
};

// ���j���[���̊e���ڂ̍��ۉ��p��� (����M���ʎ�/�؍���)
static const DlgTextInfo MenuInfoOneK[] = {
	// ����M
	{ ID_MI_KANJIRECV + IdSJIS,  "MENU_KS5601" },
	{ ID_MI_KANJIRECV + IdUTF8,  "MENU_UTF8" },
	// UseOneSetting
	{ ID_MI_USEONESETTING, "MENU_USE_ONE_SETTING" }
};

static HANDLE hInst; /* Instance handle of TTX*.DLL */

typedef struct {
	PTTSet ts;
	PComVar cv;
	HMENU hmEncode;
	PSetupTerminal origSetupTermDlg;
	PReadIniFile origReadIniFile;
	PWriteIniFile origWriteIniFile;
	BOOL UseOneSetting;
	TTPluginAPI *plugin_API;
	TTPluginAPIs *plugin_APIs;
} TInstVar;

static TInstVar *pvar;

/* WIN32 allows multiple instances of a DLL */
static TInstVar InstVar;

/*
 * ������
 */
static void PASCAL TTXInit(PTTSet ts, PComVar cv) {
	pvar->ts = ts;
	pvar->cv = cv;
	pvar->origReadIniFile = NULL;
	pvar->origWriteIniFile = NULL;
	pvar->UseOneSetting = TRUE;
}

/*
 * �ݒ�̓ǂݍ���
 */
static void PASCAL TTXKanjiMenuReadIniFile(const wchar_t *fn, PTTSet ts) {
	char buff[20];

	/* Call original ReadIniFile */
	pvar->origReadIniFile(fn, ts);

	GetPrivateProfileStringAFileW(IniSection, "UseOneSetting", "on", buff, sizeof(buff), fn);
	if (_stricmp(buff, "off") == 0) {
		pvar->UseOneSetting = FALSE;
	}
	else {
		pvar->UseOneSetting = TRUE;
		// UseOneSetting �� on �̏ꍇ�́A����M�ݒ肪�����ɂȂ�悤�ɒ�������
		if (pvar->ts->Language == IdJapanese) {
			pvar->ts->KanjiCodeSend = pvar->ts->KanjiCode;
		}
		else if (pvar->ts->Language == IdKorean) {
			pvar->ts->KanjiCodeSend = pvar->ts->KanjiCode;
		}
	}
	return;
}

/*
 * �ݒ�̕ۑ�
 */
static void PASCAL TTXKanjiMenuWriteIniFile(const wchar_t *fn, PTTSet ts) {
	/* Call original WriteIniFile */
	pvar->origWriteIniFile(fn, ts);

	WritePrivateProfileStringAFileW(IniSection, "UseOneSetting", pvar->UseOneSetting?"on":"off", fn);

	return;
}

/*
 * �ݒ�̓ǂݏ������t�b�N����
 */
static void PASCAL TTXGetSetupHooks(TTXSetupHooks *hooks) {
	pvar->origReadIniFile = *hooks->ReadIniFile;
	*hooks->ReadIniFile = TTXKanjiMenuReadIniFile;
	pvar->origWriteIniFile = *hooks->WriteIniFile;
	*hooks->WriteIniFile = TTXKanjiMenuWriteIniFile;
}

/*
 * �����ID�̎q�������j���[�̈ʒu��Ԃ�
 */
static int GetMenuPosByChildId(HMENU menu, UINT id) {
	UINT i, j, items, subitems, cur_id;
	HMENU m;

	items = GetMenuItemCount(menu);

	for (i=0; i<items; i++) {
		if (m = GetSubMenu(menu, i)) {
			subitems = GetMenuItemCount(m);
			for (j=0; j<subitems; j++) {
				cur_id = GetMenuItemID(m, j);
				if (cur_id == id) {
					return i;
				}
			}
		}
	}
	return -1;
}

/*
 * ���M�����R�[�h�ݒ�p�̃��j���[���ڂ�ǉ�����
 *
 * UseOneSetting �� off �̎��Ɏg��
 */
static void InsertSendKcodeMenu(HMENU menu) {
//	UINT flag = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	int i;

	if (pvar->ts->Language == IdJapanese) {
		for (i = 0; i < _countof(MenuNameSendJ); i++) {
			InsertMenu(pvar->hmEncode, ID_MI_USEONESETTING, MF_BYCOMMAND | MF_STRING,
					MenuNameSendJ[i].menuID, MenuNameSendJ[i].menuStr);
		}
	}
	else { // IdKorean
		for (i = 0; i < _countof(MenuNameSendK); i++) {
			InsertMenu(pvar->hmEncode, ID_MI_USEONESETTING, MF_BYCOMMAND | MF_STRING,
					MenuNameSendK[i].menuID, MenuNameSendK[i].menuStr);
		}
	}

	InsertMenu(menu, ID_MI_USEONESETTING, MF_BYCOMMAND | MF_SEPARATOR, 0, NULL);
}

/*
 * ���M�����R�[�h�ݒ�p�̃��j���[���ڂ��폜����
 *
 * UseOneSetting �� on �ɂ��ꂽ���ɌĂ΂��
 */
static void DeleteSendKcodeMenu(HMENU menu) {
	int i;

	if (pvar->ts->Language == IdJapanese) {
		for (i=0; i < _countof(MenuNameSendJ); i++) {
			DeleteMenu(menu, MenuNameSendJ[i].menuID, MF_BYCOMMAND);
		}
		// ��M���j���[�̒���ɗL��Z�p���[�^���폜����
		DeleteMenu(menu, _countof(MenuNameRecvJ), MF_BYPOSITION);
	}
	else { // IdKorean
		for (i=0; i < _countof(MenuNameSendK); i++) {
			DeleteMenu(menu, MenuNameSendK[i].menuID, MF_BYCOMMAND);
		}
		// ��M���j���[�̒���ɗL��Z�p���[�^���폜����
		DeleteMenu(menu, _countof(MenuNameRecvK), MF_BYPOSITION);
	}
}

/*
 * ���j���[���ڂ̍X�V
 *
 * �ȉ��̓�ɂ��ă��j���[���ڂ��X�V����B
 * 1. UseOneSetting �̐ݒ�Ɋ�Â��āA��M�p���j���[���ڂ���M��p/����M���p�̐؂�ւ����s��
 * 2. ���j���[���ڂ̍��ۉ����s��
 *
 * �ʏ�� 1 �Őݒ肵�����ږ��� 2 �ŏ㏑���X�V����邪�Alng �t�@�C�����ݒ肳��Ă��Ȃ��A
 * �܂��� lng �t�@�C���Ƀ��j���[���ږ����܂܂�Ă��Ȃ��ꍇ�ւ̑Ή��Ƃ��� 1 ���s���Ă���B
 */
static void UpdateMenuCaption(HMENU menu, BOOL UseOneSetting) {
#define doUpdateMenu(nameInfo, i18nInfo) { \
	UINT i, id; \
	for (i=0; i < _countof(nameInfo); i++) { \
		id = (nameInfo)[i].menuID; \
		ModifyMenu(menu, id, MF_BYCOMMAND, id, (nameInfo)[i].menuStr); \
	} \
	SetI18nMenuStrsW(menu, IniSection, i18nInfo, _countof(i18nInfo), pvar->ts->UILanguageFileW); \
}

	if (pvar->ts->Language == IdJapanese) {
		if (UseOneSetting) {
			doUpdateMenu(MenuNameOneJ, MenuInfoOneJ);
		}
		else {
			doUpdateMenu(MenuNameRecvJ, MenuInfoSeparateJ);
		}
	}
	else { // IdKorean
		if (UseOneSetting) {
			doUpdateMenu(MenuNameOneK, MenuInfoOneK);
		}
		else {
			doUpdateMenu(MenuNameRecvK, MenuInfoSeparateK);
		}
	}
}

/*
 * This function is called when Tera Term creates a new menu.
 */
static void PASCAL TTXModifyMenu(HMENU menu) {
	// ���ꂪ���{��܂��͊؍���̎��̂݃��j���[�ɒǉ�����
	if (pvar->ts->Language != IdJapanese && pvar->ts->Language != IdKorean) {
		return;
	}

	{
		MENUITEMINFO mi;
		int pos, i;

		pvar->hmEncode = CreateMenu();

		// Windows 95 �Ń��j���[���\������Ȃ��̂Ńo�[�W�����`�F�b�N������ (2009.2.18 maya)
		if (IsWindows2000OrLater()) {
			memset(&mi, 0, sizeof(MENUITEMINFO));
			mi.cbSize = sizeof(MENUITEMINFO);
		}
		else {
			memset(&mi, 0, sizeof(MENUITEMINFO)-sizeof(HBITMAP));
			mi.cbSize = sizeof(MENUITEMINFO)-sizeof(HBITMAP);
		}
		mi.fMask  = MIIM_TYPE | MIIM_SUBMENU;
		mi.fType  = MFT_STRING;
		mi.hSubMenu = pvar->hmEncode;

		if (pvar->ts->Language == IdJapanese) {
			mi.dwTypeData = "&KanjiCode";
		}
		else { // IdKorean
			mi.dwTypeData = "Coding(&K)";
		}
		InsertMenuItem(menu, ID_HELPMENU, FALSE, &mi);

		if (pvar->ts->Language == IdJapanese) {
			for (i = 0; i < _countof(MenuNameRecvJ); i++) {
				AppendMenu(pvar->hmEncode, MF_STRING, MenuNameRecvJ[i].menuID, MenuNameRecvJ[i].menuStr);
			}
		}
		else { // IdKorean
			for (i = 0; i < _countof(MenuNameRecvK); i++) {
				AppendMenu(pvar->hmEncode, MF_STRING, MenuNameRecvK[i].menuID, MenuNameRecvK[i].menuStr);
			}
		}

		AppendMenu(pvar->hmEncode, MF_SEPARATOR, 0, NULL);
		AppendMenu(pvar->hmEncode, MF_STRING, ID_MI_USEONESETTING ,  "Use &one setting");

		if (!pvar->UseOneSetting) {
			InsertSendKcodeMenu(pvar->hmEncode);
		}

		pos = GetMenuPosByChildId(menu, ID_MI_KANJIRECV + IdSJIS);

		if (pos > 0) {
			if (pvar->ts->Language == IdJapanese) {
				MenuTitleInfoJ->nIDDlgItem = pos;
				SetI18nMenuStrsW(menu, IniSection, MenuTitleInfoJ, _countof(MenuTitleInfoJ), pvar->ts->UILanguageFileW);
			}
			else {
				MenuTitleInfoK->nIDDlgItem = pos;
				SetI18nMenuStrsW(menu, IniSection, MenuTitleInfoK, _countof(MenuTitleInfoK), pvar->ts->UILanguageFileW);
			}
		}

		UpdateMenuCaption(pvar->hmEncode, pvar->UseOneSetting);
	}
}


/*
 * ���W�I���j���[/�`�F�b�N���j���[�̏�Ԃ�ݒ�ɍ��킹�čX�V����B
 */
static void PASCAL TTXModifyPopupMenu(HMENU menu) {
	// ���j���[���Ăяo���ꂽ��A�ŐV�̐ݒ�ɍX�V����B(2007.5.25 yutaka)
	(void)menu;
	UpdateRecvMenu(pvar->ts->KanjiCode);
	if (!pvar->UseOneSetting) {
		UpdateSendMenu(pvar->ts->KanjiCodeSend);
	}
	CheckMenuItem(pvar->hmEncode, ID_MI_USEONESETTING, MF_BYCOMMAND | (pvar->UseOneSetting)?MF_CHECKED:0);
}


/*
 * This function is called when Tera Term receives a command message.
 */
static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd) {
	WORD kanji_code_receive;
	WORD kanji_code_send;
	IdLanguage language;

	// �ݒ���擾
	pvar->plugin_APIs->GetCharset(pvar->cv, &language, &kanji_code_send, &kanji_code_receive);

	if ((cmd > ID_MI_KANJIRECV) && (cmd <= ID_MI_KANJIRECV+IdUTF8)) {
		kanji_code_receive = cmd - ID_MI_KANJIRECV;
		if (pvar->UseOneSetting) {
			kanji_code_send = kanji_code_receive;
		}
		pvar->plugin_APIs->SetCharset(pvar->cv, language, kanji_code_send, kanji_code_receive);
		return 1;
	}
	else if ((cmd > ID_MI_KANJISEND) && (cmd <= ID_MI_KANJISEND+IdUTF8)) {
		kanji_code_send = cmd - ID_MI_KANJISEND;
		if (pvar->UseOneSetting) {
			kanji_code_receive = kanji_code_send;
		}
		pvar->plugin_APIs->SetCharset(pvar->cv, language, kanji_code_send, kanji_code_receive);
		return 1;
	}
	else if (cmd == ID_MI_USEONESETTING) {
		if (pvar->UseOneSetting) {
			pvar->UseOneSetting = FALSE;
			InsertSendKcodeMenu(pvar->hmEncode);
		}
		else {
			pvar->UseOneSetting = TRUE;

			kanji_code_send = kanji_code_receive;
			pvar->plugin_APIs->SetCharset(pvar->cv, language, kanji_code_send, kanji_code_receive);

			DeleteSendKcodeMenu(pvar->hmEncode);
		}
		UpdateMenuCaption(pvar->hmEncode, pvar->UseOneSetting);
		return 1;
	}

	return 0;
}


/*
 * This record contains all the information that the extension forwards to the
 * main Tera Term code. It mostly consists of pointers to the above functions.
 * Any of the function pointers can be replaced with NULL, in which case
 * Tera Term will just ignore that function and assume default behaviour, which
 * means "do nothing".
 */
static TTXExports Exports = {
/* This must contain the size of the structure. See below for its usage. */
	sizeof(TTXExports),

/* This is the load order number of this DLL. */
	ORDER,

/* Now we just list the functions that we've implemented. */
	TTXInit,
	NULL, // TTXGetUIHooks,	//
	TTXGetSetupHooks,
	NULL, // TTXOpenTCP,
	NULL, // TTXCloseTCP,
	NULL, // TTXSetWinSize,
	TTXModifyMenu,
	TTXModifyPopupMenu,
	TTXProcessCommand,
	NULL, // TTXEnd,
	NULL, // TTXSetCommandLine
	NULL, // TTXOpenFile
	NULL, // TTXCloseFile
};

BOOL __declspec(dllexport) PASCAL TTXBind(WORD Version, TTXExports *exports, void *plugin_data)
{
	int size = sizeof(Exports) - sizeof(exports->size);
	/* do version checking if necessary */
	/* if (Version!=TTVERSION) return FALSE; */
	(void)Version;

	if (size > exports->size) {
		size = exports->size;
	}
	memcpy((char *)exports + sizeof(exports->size),
	       (char *)&Exports + sizeof(exports->size),
	       size);

	pvar->plugin_API = ((TTPluginData*)plugin_data)->PluginAPI;
	pvar->plugin_API(0, (void**) & pvar->plugin_APIs);

	return TRUE;
}

BOOL WINAPI DllMain(HANDLE hInstance,
                    ULONG ul_reason_for_call,
                    LPVOID lpReserved)
{
	(void)lpReserved;
	switch( ul_reason_for_call ) {
		case DLL_THREAD_ATTACH:
			/* do thread initialization */
			break;
		case DLL_THREAD_DETACH:
			/* do thread cleanup */
			break;
		case DLL_PROCESS_ATTACH:
			/* do process initialization */
			hInst = hInstance;
			pvar = &InstVar;
			break;
		case DLL_PROCESS_DETACH:
			/* do process cleanup */
			break;
	}
	return TRUE;
}

/* vim: set ts=4 sw=4 ff=dos : */
