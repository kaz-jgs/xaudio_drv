/*!
 *=================================================================================
 * @file	TrackBase.h
 * @author	kaz-jgs
 * @date	20141217
 * @brief	XAudio2���K�p�T�E���h�h���C�o �g���b�N�I�u�W�F�N�g�p���N���X.
 *=================================================================================
 */
#pragma once

#include "Drv.h"

//! XAudio2�C���^�[�t�F�C�X�̑O���錾
struct IXAudio2Voice;

namespace xaudio_drv{

class TrackBase{
//! �R���X�g���N�^�ƃf�X�g���N�^
public:
	TrackBase(IXAudio2Voice* const& _voice);
	virtual ~TrackBase(){};

//! ���ʃC���^�[�t�F�C�X
public:
	virtual bool			exec() = 0;												//! ���s�֐�
	bool					updateVolume();											//! ���ʍX�V
	SndHandle				getHandle() const{ return handle_; }					//! �n���h���̎擾

	void					setVolume(float _targetVol, float _fadeTime = 0.f);		//! �{�����[���Z�b�g
	float					getVolume() const{ return volInfo_.target; }			//! �Z�b�g���ꂽ�{�����[���̎擾

//! ���ʃN���X���萔
protected:
	static const long		DELTA_TICKS_THRESHOLD_ = 1000;							//!< �f���^�^�C����臒l



//! �����o�ϐ�
protected:
	IXAudio2Voice* const&	refVoiceBase_;											//! Voice�C���^�[�t�F�C�X�̊�ꕔ���ւ̎Q��
	SndHandle				handle_;												//! �n���h��


//! �ÓI�����o
protected:
	static unsigned long	handleSeed__;											//! �n���h�������̎�

//! �{�����[�����
protected:
	struct{
		float				vol;													//!< ���݃{�����[��
		float				target;													//!< �ڕW�{�����[��
		float				delta;													//!< �P�ʎ��ԓ�����̑�����
		unsigned long		lastTicks;												//!< �Ō�ɏ�������TickCount
	} volInfo_;

};

}
