/*!
 *=================================================================================
 * @file	Bus.h
 * @author	kaz-jgs
 * @date	20150105
 * @brief	XAudio2���K�p�T�E���h�h���C�o �o�̓o�X�N���X.
 *=================================================================================
 */
#pragma once
#include "TrackBase.h"

//! XAudio2�C���^�[�t�F�C�X�̑O���錾
struct IXAudio2SubmixVoice;
struct XAUDIO2_BUFFER;

namespace xaudio_drv{
//! �N���X�錾
class Bus : public TrackBase{
public:
	Bus(IXAudio2SubmixVoice* _submixVoice, const wchar_t* _busName);
	~Bus();

//! �N���X���萔
protected:
	static const int		BUS_NAME_LEN_ = 16;

//! �����o�ϐ�
protected:
	IXAudio2SubmixVoice*	submixVoice_;
	wchar_t					busName_[BUS_NAME_LEN_];


//! �A�N�Z�X�֐�
public:
	bool					exec();											//!< �t���[�������s�֐�
	const wchar_t*			getBusName() const{ return busName_; }			//!< �o�X���擾
};

}