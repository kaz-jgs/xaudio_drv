/*!
 *=================================================================================
 * @file	Core.cpp
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2���K�p�T�E���h�h���C�o�̃R�A����.
 *=================================================================================
 */
#include <XAudio2.h>

#include "Core.h"
#include "Wave.h"
#include "Snd.h"
#include "Bus.h"
#include "SndCallback.h"

#include "util/macros_debug.h"

namespace xaudio_drv{

//! �V���O���g���C���X�^���X
static Core* s_instance = NULL;


/*!
 * �V���O���g���C���X�^���X�̐���
 */
void Core::createInstance(){
	if(s_instance)
		return;

	s_instance = new Core();
}

/*!
 * �V���O���g���C���X�^���X�̎擾
 */
CoreInstanceRef Core::getInstanceRef(){
	return s_instance;
}

/*!
 * �V���O���g���C���X�^���X�̍폜
 */
void Core::deleteInstance(){
	if(s_instance);
	else
		return;

	delete s_instance;
	s_instance = NULL;
}

/*!
 * �R���X�g���N�^
 */
Core::Core() :	xaudioEngine_(NULL),
				masterTrack_(NULL)
{
}



/*!
 * �f�X�g���N�^
 */
Core::~Core(){
	if(xaudioEngine_)
		finalize();
}



/*!
 * ����������
 */
bool Core::initialize(){
	// �ߏ������`�F�b�N
	if(xaudioEngine_)
		return false;

	// �G���W���̏�����
	{
		// �t���O(�f�o�b�O���̓f�o�b�O�p�t���O�𗧂Ă邽��
		unsigned int flags = 0;
#ifdef _DEBUG
		flags |= XAUDIO2_DEBUG_ENGINE;
#endif

		// �G���W���̏�����
		if (XAudio2Create(&xaudioEngine_, flags) == S_OK);
		else{
			return false;
		}
	}

	// �}�X�^�����O�{�C�X(�}�X�^�[�g���b�N)�̐���
	xaudioEngine_->CreateMasteringVoice(&masterTrack_, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL);
	if(masterTrack_);
	else{
		AssertMsgBox(false, "Core : creating mastering voice is failed.");
		return false;
	}

	return true;
}



/*!
 * �I������
 */
bool Core::finalize(){
	// �߉���`�F�b�N
	if(xaudioEngine_);
	else
		return false;

	if(masterTrack_){
		masterTrack_->DestroyVoice();
		masterTrack_ = NULL;
	}

	xaudioEngine_->Release();
	xaudioEngine_ = NULL;

	return true;
}


/*!
 * �����I�u�W�F�N�g�̐���
 */
Snd* Core::createSnd(const Wave* _wave, const void* _bufAddr){
	// �R�[���o�b�N�I�u�W�F�N�g�̐���
	SndCallback* callback = new SndCallback();

	// �����{�C�X�̃C���X�^���X����
	IXAudio2SourceVoice* voice = NULL;
	xaudioEngine_->CreateSourceVoice(&voice, _wave->getWaveFormatEx(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, callback, NULL);


	// XAudio2�o�b�t�@�\���̂����
	XAUDIO2_BUFFER buffer = { 0 };
	{
		buffer.pAudioData = static_cast<const BYTE*>(_bufAddr);
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.AudioBytes = _wave->getWaveSize();
		buffer.LoopBegin = _wave->getLoopBegin();
		buffer.LoopLength = _wave->getLoopLength();
		buffer.LoopCount = _wave->getLoopCount();
	}

	// �o�b�t�@��]������
	voice->SubmitSourceBuffer(&buffer);

	// �����I�u�W�F�N�g���쐬���ăC���X�^���X��Ԃ�
	return new Snd(voice, callback);
}


/*!
 * �o�̓o�X�̐���
 */
Bus* Core::createBus(){
	// �T�u�~�N�X�{�C�X�̃C���X�^���X����
	IXAudio2SubmixVoice* voice = NULL;
	xaudioEngine_->CreateSubmixVoice(&voice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE);

	// �o�̓o�X�C���X�^���X�𐶐����ĕԂ�
	return new Bus(voice);
}


/*!
 * �}�X�^�[�{�����[���̎擾
 */
float Core::getMasterVolume() const{
	// �߂�l�p�ϐ�
	float ret = 0.f;

	// �������`�F�b�N
	if(xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return ret;
	}

	// �}�X�^�[����{�����[�����擾����
	masterTrack_->GetVolume(&ret);

	return ret;
}

/*!
 * �}�X�^�[�{�����[���̃Z�b�g
 */
void Core::setMasterVolume(float _volume){
	// �������`�F�b�N
	if (xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return;
	}

	masterTrack_->SetVolume(_volume);
}
}
