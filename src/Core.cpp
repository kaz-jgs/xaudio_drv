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
#include "Master.h"
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

	// �}�X�^�[�g���b�N�̐���
	{
		// XAudio2�}�X�^�����O�{�C�X�C���X�^���X�̐���
		IXAudio2MasteringVoice* mastering_voice = NULL;
		xaudioEngine_->CreateMasteringVoice(&mastering_voice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL);
		if (mastering_voice);
		else{
			AssertMsgBox(false, "Core : creating mastering voice is failed.");
			return false;
		}

		// �}�X�^�[�g���b�N�I�u�W�F�N�g�C���X�^���X�̐���
		masterTrack_ = new Master(mastering_voice);
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

	// �}�X�^�[�g���b�N�̊J��
	if(masterTrack_){
		delete masterTrack_;
		masterTrack_ = NULL;
	}

	// XAudio�G���W���̊J��
	xaudioEngine_->Release();
	xaudioEngine_ = NULL;

	return true;
}


/*!
 * ���s����
 */
bool Core::exec(){
	return masterTrack_->exec();
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
	XAUDIO2_BUFFER* buffer = new XAUDIO2_BUFFER();
	{
		buffer->pAudioData = static_cast<const BYTE*>(_bufAddr);
		buffer->Flags = XAUDIO2_END_OF_STREAM;
		buffer->AudioBytes = _wave->getWaveSize();
		buffer->LoopBegin = _wave->getLoopBegin();
		buffer->LoopLength = _wave->getLoopLength();
		buffer->LoopCount = _wave->getLoopCount();
	}

	// �����I�u�W�F�N�g���쐬���ăC���X�^���X��Ԃ�
	return new Snd(voice, callback, buffer);
}


/*!
 * �o�̓o�X�̐���
 */
Bus* Core::createBus(const wchar_t* _busName /* = NULL */){
	// �T�u�~�N�X�{�C�X�̃C���X�^���X����
	IXAudio2SubmixVoice* voice = NULL;
	xaudioEngine_->CreateSubmixVoice(&voice, 2, 48000);

	// �o�̓o�X�C���X�^���X�𐶐����ĕԂ�
	return new Bus(voice, _busName);
}


/*!
 * �}�X�^�[�{�����[���̎擾
 */
float Core::getMasterVolume() const{
	// �������`�F�b�N
	if(xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return 0.f;
	}

	// �}�X�^�[����{�����[�����擾����
	return masterTrack_->getVolume();
}

/*!
 * �}�X�^�[�{�����[���̃Z�b�g
 */
void Core::setMasterVolume(float _volume, float _fadeTime/* = 0.f*/){
	// �������`�F�b�N
	if (xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return;
	}

	masterTrack_->setVolume(_volume, _fadeTime);
}
}
