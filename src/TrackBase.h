/*!
 *=================================================================================
 * @file	TrackBase.h
 * @author	kaz-jgs
 * @date	20141217
 * @brief	XAudio2練習用サウンドドライバ トラックオブジェクト用基底クラス.
 *=================================================================================
 */
#pragma once

#include "Drv.h"

//! XAudio2インターフェイスの前方宣言
struct IXAudio2Voice;

namespace xaudio_drv{

class TrackBase{
//! コンストラクタとデストラクタ
public:
	TrackBase(IXAudio2Voice* const& _voice);
	virtual ~TrackBase(){};

//! 共通インターフェイス
public:
	virtual bool			exec() = 0;												//! 実行関数
	bool					updateVolume();											//! 音量更新
	SndHandle				getHandle() const{ return handle_; }					//! ハンドルの取得

	void					setVolume(float _targetVol, float _fadeTime = 0.f);		//! ボリュームセット
	float					getVolume() const{ return volInfo_.target; }			//! セットされたボリュームの取得

//! 共通クラス内定数
protected:
	static const long		DELTA_TICKS_THRESHOLD_ = 1000;							//!< デルタタイムの閾値



//! メンバ変数
protected:
	IXAudio2Voice* const&	refVoiceBase_;											//! Voiceインターフェイスの基底部分への参照
	SndHandle				handle_;												//! ハンドル


//! 静的メンバ
protected:
	static unsigned long	handleSeed__;											//! ハンドル生成の種

//! ボリューム情報
protected:
	struct{
		float				vol;													//!< 現在ボリューム
		float				target;													//!< 目標ボリューム
		float				delta;													//!< 単位時間当たりの増減量
		unsigned long		lastTicks;												//!< 最後に処理したTickCount
	} volInfo_;

};

}
