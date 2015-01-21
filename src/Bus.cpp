/*!
 *=================================================================================
 * @file	Bus.h
 * @author	kaz-jgs
 * @date	20150106
 * @brief	XAudio2���K�p�T�E���h�h���C�o �o�̓o�X�N���X.
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
	// Ticks�̍X�V
	if (updateTicks());
	else
		return false;

	// �{�C�X���s���Ȃ炱��ȏ㏈�����Ȃ�
	if(submixVoice_);
	else
		return true;

	// �{�����[�����̕ύX
	updateVolume();

	return true;
}

}