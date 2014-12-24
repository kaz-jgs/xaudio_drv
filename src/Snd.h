/*!
 *=================================================================================
 * @file	Snd.h
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2練習用サウンドドライバ 音源オブジェクトクラス.
 *=================================================================================
 */
#pragma once
#include "TrackBase.h"

//! XAudio2インターフェイスの前方宣言
struct IXAudio2SourceVoice;
struct XAUDIO2_BUFFER;

namespace xaudio_drv{
//! コールバッククラスの前方宣言
class SndCallback;

//! クラス宣言
class Snd : public TrackBase{
//! コールバッククラスにはメンバアクセスを許可する
friend class SndCallback;

//! コンストラクタとデストラクタ
public:
	Snd(IXAudio2SourceVoice* _srcVoice, SndCallback* _callback);
	~Snd();

//! クラス内定数
protected:
	static const long		DELTA_TICKS_THRESHOLD_ = 1000;					//!< デルタタイムの閾値

//! メンバ変数
protected:
	IXAudio2SourceVoice*	srcVoice_;														
	SndCallback*			callback_;

	long					lastExec_;										//!< 前回exec関数実行時のticks


	bool					isStopping_;									//!< 停止中か
	bool					isPlaying_;


//! アクセス関数
public:
	bool					exec();											//!< フレーム毎実行関数

	bool					play(float _fadeTime = 0.f);					//!< 再生リクエスト
	bool					stop(float _fadeTime = 0.f);					//!< 停止リクエスト
	bool					pause();										//!< ポーズ

	bool					isPlaying() const { return isPlaying_; }		//!< 再生中かどうか

};

}
