/*!
 *=================================================================================
 * @file	Snd.cpp
 * @author	kawatanik
 * @date	20141216
 * @brief	XAudio2練習用サウンドドライバ 音源オブジェクトクラス.
 *=================================================================================
 */
#include <XAudio2.h>

#include "Snd.h"
#include "SndCallback.h"

namespace xaudio_drv{



/*!
 * コンストラクタ
 */
Snd::Snd(IXAudio2SourceVoice* _srcVoice, SndCallback* _callback) : 
TrackBase(dynamic_cast<IXAudio2Voice* const>(srcVoice_)),
srcVoice_(_srcVoice),
callback_(_callback)
{
}


/*!
 * デストラクタ
 */
Snd::~Snd(){
	if (srcVoice_){
		srcVoice_->Stop();
		srcVoice_->DestroyVoice();
		srcVoice_ = NULL;
	}
	if (callback_){
		delete callback_;
		callback_ = NULL;
	}
}

/*!
 * 実行関数
 * @retval	デルタタイムの関係で処理を打ち切った場合はfalse
 */
bool Snd::exec(){
	unsigned long delta_ticks = GetTickCount() - lastExec_;

	// デルタタイムが足りてない or オーバーフローが起きていれば処理しない
	if(delta_ticks < DELTA_TICKS_THRESHOLD_ || static_cast<signed>(delta_ticks) < 0)
		return false;

	// ticks更新
	lastExec_ += delta_ticks;

	// waveが不正ならこれ以上処理しない
	if(srcVoice_);
	else
		return true;
	
	// 再生中フラグの更新
	if(callback_->isStreamEnd()){
		isPlaying_ = false;
		stop(0.f);
		callback_->resetStreamEnd();
	}
	
	// waveが再生中でなければこれ以上処理しない
	if(isPlaying_);
	else
		return true;

	// ボリューム情報の変更
	updateVolume();

	return true;
}


/*!
 * 再生
 */
bool Snd::play(float _fadeTime /* = 0.f */){
	// waveが不正ならfalse
	if (!srcVoice_)
		return false;

	srcVoice_->Start(0);
	isPlaying_ = true;

	// フェードタイムが指定されていればボリュームの設定
	if (_fadeTime > 0.f){
		float targ = volInfo_.target;
		setVolume(0.f, 0.f);
		setVolume(targ, _fadeTime);
	}

	return true;
}

/*!
 * 停止
 */
bool Snd::stop(float _fadeTime /* = 0.f */){
	// waveが不正ならfalse
	if (!srcVoice_) return false;

	// フェードアウトタイムが0なら単に止める
	if (_fadeTime == 0.f){
		srcVoice_->Stop();
		isPlaying_ = false;
	}

	// フェードタイムが指定されていれば停止リクエストフラグを立てて目標ボリュームを最低に設定
	else{
		isStopping_ = true;
		setVolume(0.f, 0.f);
	}
	return true;
}

}
