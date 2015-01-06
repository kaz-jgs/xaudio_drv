/*!
 *=================================================================================
 * @file	Bus.h
 * @author	kaz-jgs
 * @date	20150106
 * @brief	XAudio2練習用サウンドドライバ 出力バスクラス.
 *=================================================================================
 */
#include <XAudio2.h>
#include "Bus.h"

namespace xaudio_drv{
Bus::Bus(IXAudio2SubmixVoice* _submixVoice) :
TrackBase(reinterpret_cast<IXAudio2Voice* const&>(submixVoice_)),
submixVoice_(_submixVoice)
{
}


Bus::~Bus(){
	if (submixVoice_){
		submixVoice_->DestroyVoice();
		submixVoice_ = NULL;
	}
}

bool Bus::exec(){
	unsigned long delta_ticks = GetTickCount() - lastExec_;

	// デルタタイムが足りてない or オーバーフローが起きていれば処理しない
	if (delta_ticks < DELTA_TICKS_THRESHOLD_ || static_cast<signed>(delta_ticks) < 0)
		return false;

	// ticks更新
	lastExec_ += delta_ticks;

	// ボイスが不正ならこれ以上処理しない
	if(submixVoice_);
	else
		return true;

	// ボリューム情報の変更
	updateVolume();

	return true;
}

}