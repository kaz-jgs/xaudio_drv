/*!
 *=================================================================================
 * @file	Snd.cpp
 * @author	kawatanik
 * @date	20141216
 * @brief	XAudio2���K�p�T�E���h�h���C�o �����I�u�W�F�N�g�N���X.
 *=================================================================================
 */
#include <XAudio2.h>

#include "Snd.h"
#include "SndCallback.h"

namespace xaudio_drv{



/*!
 * �R���X�g���N�^
 */
Snd::Snd(IXAudio2SourceVoice* _srcVoice, SndCallback* _callback) : 
TrackBase(dynamic_cast<IXAudio2Voice* const>(srcVoice_)),
srcVoice_(_srcVoice),
callback_(_callback)
{
}


/*!
 * �f�X�g���N�^
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
 * ���s�֐�
 * @retval	�f���^�^�C���̊֌W�ŏ�����ł��؂����ꍇ��false
 */
bool Snd::exec(){
	unsigned long delta_ticks = GetTickCount() - lastExec_;

	// �f���^�^�C��������ĂȂ� or �I�[�o�[�t���[���N���Ă���Ώ������Ȃ�
	if(delta_ticks < DELTA_TICKS_THRESHOLD_ || static_cast<signed>(delta_ticks) < 0)
		return false;

	// ticks�X�V
	lastExec_ += delta_ticks;

	// wave���s���Ȃ炱��ȏ㏈�����Ȃ�
	if(srcVoice_);
	else
		return true;
	
	// �Đ����t���O�̍X�V
	if(callback_->isStreamEnd()){
		isPlaying_ = false;
		stop(0.f);
		callback_->resetStreamEnd();
	}
	
	// wave���Đ����łȂ���΂���ȏ㏈�����Ȃ�
	if(isPlaying_);
	else
		return true;

	// �{�����[�����̕ύX
	updateVolume();

	return true;
}


/*!
 * �Đ�
 */
bool Snd::play(float _fadeTime /* = 0.f */){
	// wave���s���Ȃ�false
	if (!srcVoice_)
		return false;

	srcVoice_->Start(0);
	isPlaying_ = true;

	// �t�F�[�h�^�C�����w�肳��Ă���΃{�����[���̐ݒ�
	if (_fadeTime > 0.f){
		float targ = volInfo_.target;
		setVolume(0.f, 0.f);
		setVolume(targ, _fadeTime);
	}

	return true;
}

/*!
 * ��~
 */
bool Snd::stop(float _fadeTime /* = 0.f */){
	// wave���s���Ȃ�false
	if (!srcVoice_) return false;

	// �t�F�[�h�A�E�g�^�C����0�Ȃ�P�Ɏ~�߂�
	if (_fadeTime == 0.f){
		srcVoice_->Stop();
		isPlaying_ = false;
	}

	// �t�F�[�h�^�C�����w�肳��Ă���Β�~���N�G�X�g�t���O�𗧂ĂĖڕW�{�����[�����Œ�ɐݒ�
	else{
		isStopping_ = true;
		setVolume(0.f, 0.f);
	}
	return true;
}

}
