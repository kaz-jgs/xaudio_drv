/*!
 *=================================================================================
 * @file	Master.h
 * @author	kaz-jgs
 * @date	20150109
 * @brief	XAudio2練習用サウンドドライバ マスタートラッククラス.
 *
 * ほかトラックと共通処理で音量調整できるようにするだけ.
 *=================================================================================
 */
#pragma once
#include "TrackBase.h"

//! XAudio2インターフェイスの前方宣言
struct IXAudio2MasteringVoice;
struct XAUDIO2_BUFFER;

namespace xaudio_drv{
//! クラス宣言
class Master : public TrackBase{
public:
	Master(IXAudio2MasteringVoice* _masteringVoice);
	~Master();

//! メンバ変数
protected:
	IXAudio2MasteringVoice*	masteringVoice_;


//! アクセス関数
public:
	bool					exec();											//!< フレーム毎実行関数

};

}