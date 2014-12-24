/*!
 *=================================================================================
 * @file	Snd.h
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2���K�p�T�E���h�h���C�o �����I�u�W�F�N�g�N���X.
 *=================================================================================
 */
#pragma once
#include "TrackBase.h"

//! XAudio2�C���^�[�t�F�C�X�̑O���錾
struct IXAudio2SourceVoice;
struct XAUDIO2_BUFFER;

namespace xaudio_drv{
//! �R�[���o�b�N�N���X�̑O���錾
class SndCallback;

//! �N���X�錾
class Snd : public TrackBase{
//! �R�[���o�b�N�N���X�ɂ̓����o�A�N�Z�X��������
friend class SndCallback;

//! �R���X�g���N�^�ƃf�X�g���N�^
public:
	Snd(IXAudio2SourceVoice* _srcVoice, SndCallback* _callback);
	~Snd();

//! �N���X���萔
protected:
	static const long		DELTA_TICKS_THRESHOLD_ = 1000;					//!< �f���^�^�C����臒l

//! �����o�ϐ�
protected:
	IXAudio2SourceVoice*	srcVoice_;														
	SndCallback*			callback_;

	long					lastExec_;										//!< �O��exec�֐����s����ticks


	bool					isStopping_;									//!< ��~����
	bool					isPlaying_;


//! �A�N�Z�X�֐�
public:
	bool					exec();											//!< �t���[�������s�֐�

	bool					play(float _fadeTime = 0.f);					//!< �Đ����N�G�X�g
	bool					stop(float _fadeTime = 0.f);					//!< ��~���N�G�X�g
	bool					pause();										//!< �|�[�Y

	bool					isPlaying() const { return isPlaying_; }		//!< �Đ������ǂ���

};

}
