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
Bus::Bus(IXAudio2SubmixVoice* _submixVoice, const wchar_t* _busName) :
TrackBase(reinterpret_cast<IXAudio2Voice* const&>(submixVoice_)),
submixVoice_(_submixVoice)
{
	wcsncpy_s(busName_, _busName, wcslen(_busName));
}


Bus::~Bus(){
	if (submixVoice_){
		submixVoice_->DestroyVoice();
		submixVoice_ = NULL;
	}
}

bool Bus::exec(){
	// Ticksの更新
	if (updateTicks());
	else
		return false;

	// ボイスが不正ならこれ以上処理しない
	if(submixVoice_);
	else
		return true;

	// ボリューム情報の変更
	updateVolume();

	return true;
}

}