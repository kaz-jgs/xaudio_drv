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
struct XAUDIO2_SEND_DESCRIPTOR;

namespace xaudio_drv{
//! クラスの前方宣言
class Bus;

class TrackBase{
//! コンストラクタとデストラクタ
public:
	TrackBase(IXAudio2Voice* const& _voice);
	virtual ~TrackBase();

//! 共通インターフェイス
public:
	virtual bool				exec() = 0;											//!< 実行関数
	SndHandle					getHandle() const{ return handle_; }				//!< ハンドルの取得

	void						setVolume(float _targetVol, float _fadeTime = 0.f);	//!< ボリュームセット
	float						getVolume() const{ return volInfo_.target; }		//!< セットされたボリュームの取得

	virtual bool				setOutputBus(const Bus* _bus, unsigned int _num);	//!< 出力バスの設定

//! 共通メンバ関数
protected:
	bool						updateTicks();										//!< 処理時間Ticksの更新
	bool						updateVolume();										//!< 音量更新

//! 共通メンバ変数
protected:
	IXAudio2Voice* const&		refVoiceBase_;										//!< Voiceインターフェイスの基底部分への参照
	SndHandle					handle_;											//!< ハンドル

//! ボリューム情報
protected:
	struct{
		float					vol;												//!< 現在ボリューム
		float					target;												//!< 目標ボリューム
		float					delta;												//!< 単位時間当たりの増減量
		unsigned long			lastTicks;											//!< 最後に処理したTickCount
	} volInfo_;


//! クラス内定数
private:
	static const long			DELTA_TICKS_THRESHOLD_ = 500;						//!< デルタタイムの閾値
	static const unsigned int	SEND_LIST_MAX_ = 4;									//!< 出力

//! メンバ変数
private:
	long						lastTicks_;											//!< 前回update成功時のticks
	XAUDIO2_SEND_DESCRIPTOR*	sendList_;											//!< センドのリスト
	unsigned int				sendNum_;											//!< センドの数

//! 静的メンバ
private:
	static unsigned long		handleSeed__;										//! ハンドル生成の種
};

}
