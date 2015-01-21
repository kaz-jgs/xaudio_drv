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
	// 初期化と終了
	bool initialize();
	bool finalize();

	// インターフェイスそろうまでのサンプル用
	SndHandle sample_play(const char* _fileName);

	// 音源オブジェクト周り
	SndHandle createSnd(const char* _fileName, bool _loadOnMemory = true, bool _isAutoPlay = false);
	bool destroySnd(SndHandle _handle);

	bool play(SndHandle _handle);
	bool stop(SndHandle _handle, float _fadeTime = 0.f);
	bool isPlaying(SndHandle _handle);
	bool isPlayingAny();

	bool isAvailableSnd(SndHandle _handle);

	// バス周り
	SndHandle addBus(const wchar_t* _busName);
	bool setBusVolume(const wchar_t* _busName, float _volume, float _fadeTime = 0.f);
	float getBusVolume(const wchar_t* _busName);

	bool isAvailableBus(const wchar_t* _busName);

	// 共通
	bool setOutputBus(SndHandle _src, const wchar_t* _outputBusName);

	bool setVolume(SndHandle _handle, float _volume, float _fadeTime = 0.f);
	float getVolume(SndHandle _handle);
}
#endif

//! Cインターフェイス
#ifdef __cplusplus
extern "C"{
#endif
bool XAUDIO_DRV_Initialize();
bool XAUDIO_DRV_Finalize();
bool XAUDIO_DRV_Stop(SndHandle _handle, float _fadeTime = 0.f);
bool XAUDIO_DRV_SetVolume(SndHandle _handle, float _volume, float _fadeTime = 0.f);
float XAUDIO_DRV_GetVolume(SndHandle _handle);
bool XAUDIO_DRV_IsPlaying(SndHandle _handle);
bool XAUDIO_DRV_IsPlayingAny();
// @todo 残り
#ifdef __cplusplus
}
#endif
