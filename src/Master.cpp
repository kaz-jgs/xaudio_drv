/*!
 *=================================================================================
 * @file	Master.h
 * @author	kaz-jgs
 * @date	20150109
 * @brief	XAudio2練習用サウンドドライバ マスタートラッククラス.
 *=================================================================================
 */
#include <XAudio2.h>
#include "Master.h"

namespace xaudio_drv{
Master::Master(IXAudio2MasteringVoice* _masteringVoice) :
TrackBase(reinterpret_cast<IXAudio2Voice* const&>(masteringVoice_)),
masteringVoice_(_masteringVoice)
{
}

Master::~Master(){
	if (masteringVoice_){
		masteringVoice_->DestroyVoice();
		masteringVoice_ = NULL;
	}
}

bool Master::exec(){
	// Ticksの更新
	if (updateTicks());
	else
		return false;

	// ボイスが不正ならこれ以上処理しない
	if(masteringVoice_);
	else
		return true;

	// ボリューム情報の変更
	updateVolume();

	return true;
}

}