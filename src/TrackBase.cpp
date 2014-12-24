/*!
 *=================================================================================
 * @file	TrackBase.h
 * @author	kawatanik
 * @date	20141217
 * @brief	XAudio2���K�p�T�E���h�h���C�o �g���b�N�I�u�W�F�N�g�p���N���X.
 *=================================================================================
 */
#include "TrackBase.h"
#include <XAudio2.h>
#include <time.h>

#include "util/macros_debug.h"

namespace xaudio_drv{
//! �萔�錾
static const int TICKS_PER_SECOND = 1000;	// �b�������ticks��(Windows��msec���x)


/*!
 * @brief		�R���X�g���N�^.
 * @param[in]	_voice	�q�N���X��XAudio�{�C�X�C���^�[�t�F�C�X�ւ̎Q��
 */
TrackBase::TrackBase(IXAudio2Voice* const& _voice) : refVoiceBase_(_voice){
}

/*!
 * @brief		���ʍX�V.
 * @retval		���ʂ��ύX���ꂽ���ǂ���.
 *
 * ���`�X�V�݂̂ł�.
 * �q�N���X��exec�֐����Ŗ���Ă΂��z��ł�.
 */
bool TrackBase::updateVolume(){
	// �{�C�X������ς݂̏ꍇ��false��Ԃ�
	if(refVoiceBase_);
	else{
		AssertMsgBox(false, "voice instance is not exist.");
		return false;
	}

	// ���ʍX�V���Ȃ����false��Ԃ�
	if(volInfo_.vol == volInfo_.target || volInfo_.delta == 0.f){
		return false;
	}

	// �f���^ticks�̎擾��ticks�̍X�V
	unsigned long delta_ticks = GetTickCount() - volInfo_.lastTicks;
	volInfo_.lastTicks += delta_ticks;

	// �f���^�^�C�����I�[�o�[�t���[���Ă��ꍇ(Windows�N�������50���o�߂��Ƃɔ���)�̏���
	// @note	�����ł̌��o�����́uunsigned��signed�ɃL���X�g�����ۂɕ����ɂȂ�ق�delta�����������ꍇ�v�Ƃ��܂�.
	//			tick�����ݒl�Ɏw�肵�Đ��������ŏ�����ł��؂�̂Ńt�F�[�h���Ԃ��w��l���኱�L�т܂�
	if(static_cast<signed>(delta_ticks) < 0){
		volInfo_.lastTicks = GetTickCount();
		return true;
	}

	// �{�����[�����̍X�V
	volInfo_.vol += (volInfo_.delta * delta_ticks);

	// ���~�b�g�`�F�b�N
	if (	(volInfo_.vol > volInfo_.target && volInfo_.delta > 0) ||	// �f���^�������œK�p�{�����[�����^�[�Q�b�g���������ꍇ
			(volInfo_.vol < volInfo_.target && volInfo_.delta < 0) )	// �f���^�������œK�p�{�����[�����^�[�Q�b�g����������ꍇ
	{
		volInfo_.vol = volInfo_.target;
		volInfo_.delta = 0.f;
	}

	// �{�����[���ύX�𔽉f
	refVoiceBase_->SetVolume(volInfo_.vol);

	// true��Ԃ�
	return true;
}


/*!
 * @brief		�{�����[���̐ݒ�
 * @param[in]	_targetVol	�ݒ肷��{�����[���̖ڕW�l(1.0 = 0dB)
 * @param[in]	_fadeTime	�t�F�[�h����[sec]
 */
void TrackBase::setVolume(float _targetVol, float _fadeTime /* = 0.f */){
	// �t�F�[�h�^�C����0�̏ꍇ�͒��ڐݒ�
	if (_fadeTime <= 0.f){
		volInfo_.vol = volInfo_.target = _targetVol;
		volInfo_.delta = 0.f;
		refVoiceBase_->SetVolume(_targetVol);
	}
	// �t�F�[�h�^�C���Ɉ˂���delta�̌v�Z
	else{
		volInfo_.target = _targetVol;
		refVoiceBase_->GetVolume(&volInfo_.vol);
		volInfo_.delta = (volInfo_.target - volInfo_.vol) * (1.f / (_fadeTime * TICKS_PER_SECOND));
		volInfo_.lastTicks = GetTickCount();
	}
}
}