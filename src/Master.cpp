/*!
 *=================================================================================
 * @file	Master.h
 * @author	kaz-jgs
 * @date	20150109
 * @brief	XAudio2���K�p�T�E���h�h���C�o �}�X�^�[�g���b�N�N���X.
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
	// Ticks�̍X�V
	if (updateTicks());
	else
		return false;

	// �{�C�X���s���Ȃ炱��ȏ㏈�����Ȃ�
	if(masteringVoice_);
	else
		return true;

	// �{�����[�����̕ύX
	updateVolume();

	return true;
}

}