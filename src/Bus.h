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
	Bus(IXAudio2SubmixVoice* _submixVoice, const wchar_t* _busName);
	~Bus();

//! クラス内定数
protected:
	static const int		BUS_NAME_LEN_ = 16;

//! メンバ変数
protected:
	IXAudio2SubmixVoice*	submixVoice_;
	wchar_t					busName_[BUS_NAME_LEN_];


//! アクセス関数
public:
	bool					exec();											//!< フレーム毎実行関数
	const wchar_t*			getBusName() const{ return busName_; }			//!< バス名取得
};

}