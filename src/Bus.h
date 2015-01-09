/*!
 *=================================================================================
 * @file	Bus.h
 * @author	kaz-jgs
 * @date	20150105
 * @brief	XAudio2練習用サウンドドライバ 出力バスクラス.
 *=================================================================================
 */
#pragma once
#include "TrackBase.h"

//! XAudio2インターフェイスの前方宣言
struct IXAudio2SubmixVoice;
struct XAUDIO2_BUFFER;

namespace xaudio_drv{
//! クラス宣言
class Bus : public TrackBase{
public:
	Bus(IXAudio2SubmixVoice* _submixVoice);
	~Bus();

//! メンバ変数
protected:
	IXAudio2SubmixVoice*	submixVoice_;


//! アクセス関数
public:
	bool					exec();											//!< フレーム毎実行関数

};

}