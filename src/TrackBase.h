/*!
 *=================================================================================
 * @file	TrackBase.h
 * @author	kaz-jgs
 * @date	20141217
 * @brief	XAudio2���K�p�T�E���h�h���C�o �g���b�N�I�u�W�F�N�g�p���N���X.
 *=================================================================================
 */
#pragma once

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
	void					setVolume(float _targetVol, float _fadeTime = 0.f);		//! �{�����[���Z�b�g
	float					getVolume() const{ return volInfo_.target; }



//! �����o�ϐ�
protected:
	IXAudio2Voice* const&	refVoiceBase_;											//! Voice�C���^�[�t�F�C�X�̊�ꕔ���ւ̎Q��

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
