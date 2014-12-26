/*!
 *=================================================================================
 * @file	Drv.h
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2練習用サウンドドライバの本体.
 *=================================================================================
 */
#pragma once

//! ハンドル型の定義
typedef unsigned __int64 SndHandle;

//! C++インターフェイス
#ifdef __cplusplus
namespace xaudio_drv{
	bool initialize();
	bool finalize();

	SndHandle sample_play(const char* _fileName);

	bool isPlaying(SndHandle _handle);
	bool isPlayingAny();
}
#endif

//! Cインターフェイス
#ifdef __cplusplus
extern "C"{
#endif
bool XAUDIO_DRV_Initialize();
bool XAUDIO_DRV_Finalize();
bool XAUDIO_DRV_IsPlaying(SndHandle _handle);
bool XAUDIO_DRV_IsPlayingAny();
#ifdef __cplusplus
}
#endif
