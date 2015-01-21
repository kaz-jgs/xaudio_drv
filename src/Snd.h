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
	Snd(IXAudio2SourceVoice* _srcVoice, SndCallback* _callback, const XAUDIO2_BUFFER* _buffer);
	~Snd();

//! �����o�ϐ�
protected:
	IXAudio2SourceVoice*	srcVoice_;										//!< XAudio2�{�C�X�C���X�^���X
	SndCallback*			callback_;										//!< �R�[���o�b�N�C���X�^���X
	const XAUDIO2_BUFFER*	buffer_;										//!< XAudio2�o�b�t�@

	bool					isStopping_;									//!< ��~����
	bool					isPlaying_;										//!< �Đ�����


//! �A�N�Z�X�֐�
public:
	bool					exec();											//!< �t���[�������s�֐�

	bool					play(float _fadeTime = 0.f);					//!< �Đ����N�G�X�g
	bool					stop(float _fadeTime = 0.f);					//!< ��~���N�G�X�g
	bool					pause();										//!< �|�[�Y

	bool					isPlaying() const { return isPlaying_; }		//!< �Đ������ǂ���

};

}
