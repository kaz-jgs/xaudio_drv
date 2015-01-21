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
struct XAUDIO2_SEND_DESCRIPTOR;

namespace xaudio_drv{
//! �N���X�̑O���錾
class Bus;

class TrackBase{
//! �R���X�g���N�^�ƃf�X�g���N�^
public:
	TrackBase(IXAudio2Voice* const& _voice);
	virtual ~TrackBase();

//! ���ʃC���^�[�t�F�C�X
public:
	virtual bool				exec() = 0;											//!< ���s�֐�
	SndHandle					getHandle() const{ return handle_; }				//!< �n���h���̎擾

	void						setVolume(float _targetVol, float _fadeTime = 0.f);	//!< �{�����[���Z�b�g
	float						getVolume() const{ return volInfo_.target; }		//!< �Z�b�g���ꂽ�{�����[���̎擾

	virtual bool				setOutputBus(const Bus* _bus, unsigned int _num);	//!< �o�̓o�X�̐ݒ�

//! ���ʃ����o�֐�
protected:
	bool						updateTicks();										//!< ��������Ticks�̍X�V
	bool						updateVolume();										//!< ���ʍX�V

//! ���ʃ����o�ϐ�
protected:
	IXAudio2Voice* const&		refVoiceBase_;										//!< Voice�C���^�[�t�F�C�X�̊�ꕔ���ւ̎Q��
	SndHandle					handle_;											//!< �n���h��

//! �{�����[�����
protected:
	struct{
		float					vol;												//!< ���݃{�����[��
		float					target;												//!< �ڕW�{�����[��
		float					delta;												//!< �P�ʎ��ԓ�����̑�����
		unsigned long			lastTicks;											//!< �Ō�ɏ�������TickCount
	} volInfo_;


//! �N���X���萔
private:
	static const long			DELTA_TICKS_THRESHOLD_ = 500;						//!< �f���^�^�C����臒l
	static const unsigned int	SEND_LIST_MAX_ = 4;									//!< �o��

//! �����o�ϐ�
private:
	long						lastTicks_;											//!< �O��update��������ticks
	XAUDIO2_SEND_DESCRIPTOR*	sendList_;											//!< �Z���h�̃��X�g
	unsigned int				sendNum_;											//!< �Z���h�̐�

//! �ÓI�����o
private:
	static unsigned long		handleSeed__;										//! �n���h�������̎�
};

}
