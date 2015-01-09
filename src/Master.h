/*!
 *=================================================================================
 * @file	Master.h
 * @author	kaz-jgs
 * @date	20150109
 * @brief	XAudio2���K�p�T�E���h�h���C�o �}�X�^�[�g���b�N�N���X.
 *
 * �ق��g���b�N�Ƌ��ʏ����ŉ��ʒ����ł���悤�ɂ��邾��.
 *=================================================================================
 */
#pragma once
#include "TrackBase.h"

//! XAudio2�C���^�[�t�F�C�X�̑O���錾
struct IXAudio2MasteringVoice;
struct XAUDIO2_BUFFER;

namespace xaudio_drv{
//! �N���X�錾
class Master : public TrackBase{
public:
	Master(IXAudio2MasteringVoice* _masteringVoice);
	~Master();

//! �����o�ϐ�
protected:
	IXAudio2MasteringVoice*	masteringVoice_;


//! �A�N�Z�X�֐�
public:
	bool					exec();											//!< �t���[�������s�֐�

};

}