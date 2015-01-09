/*!
 *=================================================================================
 * @file	TrackBase.h
 * @author	kaz-jgs
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
 * @brief	�n���h������
 *
 * �C���X�^���X�A�h���X�̉���32bit��32bit�̎���o�C�g�P�ʂŕ��בւ���8�o�C�g.
 * ���0xffffffff�X�^�[�g�ŃC���X�^���X�������Ƀf�N�������g.
 * ���בւ��K���ɓ��ɈӖ��͂Ȃ�
 * �u(�킪�I�[�o�[�t���[���Ȃ����)���Ȃ��v
 * �u�O���ŗe�Ղɐ��l�����H���Đ����ł��Ȃ��v
 * �𖞂�����悤�ɂ�.
 */
unsigned long TrackBase::handleSeed__ = 0xffffffff;
#define HANDLE_CREATE(_seed)														\
	static_cast<SndHandle>(															\
		((static_cast<unsigned __int64>(_seed)		& 0x0000000000ff0000) >> 16) |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x00000000ff000000) >> 16) |	\
		((static_cast<unsigned __int64>(_seed)		& 0x00000000ff000000) >> 8)  |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x000000000000ff00) << 16) |	\
		((static_cast<unsigned __int64>(_seed)		& 0x00000000000000ff) << 40) |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x00000000000000ff) << 40) |	\
		((static_cast<unsigned __int64>(_seed)		& 0x000000000000ff00) << 40) |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x0000000000ff0000) << 40)	\
	)

/*!
 * @brief		�R���X�g���N�^.
 * @param[in]	_voice	�q�N���X��XAudio�{�C�X�C���^�[�t�F�C�X�ւ̎Q��
 */
TrackBase::TrackBase(IXAudio2Voice* const& _voice) :
refVoiceBase_(_voice), 
handle_(HANDLE_CREATE(handleSeed__--)), 
lastTicks_(GetTickCount())
{
	// �n���h���̎�̃I�[�o�[�t���[(���ʂɎg���Ă��炵�Ȃ��Ǝv��)�΍�
	if (handleSeed__ > 0);
	else{
		AssertMsgBox(false, "[TrackBase] warning : handle counter has overflowed.");
		handleSeed__ = 0xffffffff;
	}
}


/*!
 * @brief		Ticks�̍X�V.
 * @retval		Ticks�X�V�ɐ����������ǂ���.
 *
 * lastTicks_���X�V���܂�.
 * �O��X�V������DELTA_TICKS_THRESHOLD_�ȏ��Ticks���o�߂��Ă���΍X�V
 * �����łȂ���΍X�V���Ȃ������ɂȂ��Ă��܂�.
 * �q�N���X��exec�֐��擪�ŌĂяo����false���Ԃ����珈����ł��؂邱�Ƃ�
 * �T�E���h�X���b�h�̉ߕ��זh�~�Ɏg���܂�.
 */
bool TrackBase::updateTicks(){
	// �O�񂩂�̃f���^���擾
	unsigned long delta_ticks = GetTickCount() - lastTicks_;

	// �f���^�^�C��������ĂȂ� or �I�[�o�[�t���[���N���Ă���΍X�V������false��Ԃ�
	if (delta_ticks < DELTA_TICKS_THRESHOLD_ || static_cast<signed>(delta_ticks) < 0){
		return false;
	}

	// ticks�X�V����true��Ԃ�
	lastTicks_ += delta_ticks;
	return true;
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
