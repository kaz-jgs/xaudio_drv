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
	Snd(IXAudio2SourceVoice* _srcVoice, SndCallback* _callback, const XAUDIO2_BUFFER* _buffer);
	~Snd();

//! メンバ変数
protected:
	IXAudio2SourceVoice*	srcVoice_;										//!< XAudio2ボイスインスタンス
	SndCallback*			callback_;										//!< コールバックインスタンス
	const XAUDIO2_BUFFER*	buffer_;										//!< XAudio2バッファ

	bool					isStopping_;									//!< 停止中か
	bool					isPlaying_;										//!< 再生中か


//! アクセス関数
public:
	bool					exec();											//!< フレーム毎実行関数

	bool					play(float _fadeTime = 0.f);					//!< 再生リクエスト
	bool					stop(float _fadeTime = 0.f);					//!< 停止リクエスト
	bool					pause();										//!< ポーズ

	bool					isPlaying() const { return isPlaying_; }		//!< 再生中かどうか

};

}
