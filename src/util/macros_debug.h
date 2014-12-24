/*!
 *=================================================================================
 * @file	macros_debug.h
 * @author	kawatanik
 * @date	20141210
 * @brief	デバッグマクロの定義.
 *=================================================================================
 */
#ifndef __MACROS_DEBUG_H_
#define __MACROS_DEBUG_H_

#include <windows.h>
#include <stdio.h>
#include <tchar.h>


/*!
 * メッセージボックス表示型のAssert
 */
#define ASSERT_MSGBOX_STRING_LEN (256)
#ifdef _DEBUG
#define AssertMsgBox(_exp, _msg) _inlineAssertMsgBox(_exp != 0, _msg, _T(__FILE__), __LINE__)
static inline void _inlineAssertMsgBox(bool _exp, const TCHAR* _msg, TCHAR* _file, int _line){
	if (_exp);
	else{
		TCHAR str[ASSERT_MSGBOX_STRING_LEN] = { '\0' };
		_stprintf_s(str, ASSERT_MSGBOX_STRING_LEN - 1, _T("assertion is occured on\n%s\n(line : %d)\n\nmessage:\n%s"), _file, _line, _msg);
		MessageBox(NULL, str, _T("ASSERT"), MB_OK | MB_TASKMODAL);
	}
}
#ifdef UNICODE
static inline void _inlineAssertMsgBox(bool _exp, const char* _msg, TCHAR* _file, int _line){
	wchar_t buf[ASSERT_MSGBOX_STRING_LEN] = {L'\0'};
	size_t dummy = 0;
	mbstowcs_s(&dummy, buf, ASSERT_MSGBOX_STRING_LEN, _msg, ASSERT_MSGBOX_STRING_LEN);
	_inlineAssertMsgBox(_exp, buf, _file, _line);
}
#else
static inline void _inlineAssertMsgBox(bool _exp, const wchar_t* _msg, TCHAR* _file, int _line){
	char buf[ASSERT_MSGBOX_STRING_LEN] = { L'\0' };
	size_t dummy = 0;
	wcstombs_s(&dummy, buf, ASSERT_MSGBOX_STRING_LEN, _msg, ASSERT_MSGBOX_STRING_LEN);
	_inlineAssertMsgBox(_exp, buf, _file, _line);
}
#endif // #ifdef UNICODE
#else
#define AssertMsgBox(_exp, _msg)
#endif // #ifdef _DEBUG



#endif // #ifndef __MACROS_DEBUG_H_
