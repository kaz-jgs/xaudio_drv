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

	// �f���^�^�C��������ĂȂ� or �I�[�o�[�t���[���N���Ă���Ώ������Ȃ�
	if (delta_ticks < DELTA_TICKS_THRESHOLD_ || static_cast<signed>(delta_ticks) < 0)
		return false;

	// ticks�X�V
	lastExec_ += delta_ticks;

	// �{�C�X���s���Ȃ炱��ȏ㏈�����Ȃ�
	if(submixVoice_);
	else
		return true;

	// �{�����[�����̕ύX
	updateVolume();

	return true;
}

}