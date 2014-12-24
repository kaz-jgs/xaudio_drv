/*!
 *=================================================================================
 * @file	SndCallback.h
 * @author	kaz-jgs
 * @date	20141218
 * @brief	XAudio2���K�p�T�E���h�h���C�o �����I�u�W�F�N�g�N���X�p�R�[���o�b�N.
 *
 * ���̃t�@�C����include�O��XAudio2.h��include����Ă���K�v������܂�.
 *=================================================================================
 */
#pragma once

namespace xaudio_drv{
/*!
 * �R�[���o�b�N�N���X�̃v���g�^�C�v�錾.
 */
class SndCallback : public IXAudio2VoiceCallback{
public:
	SndCallback();
	~SndCallback();
	bool waitVoiceProcessingPassStart(unsigned int _waitTime = INFINITE);
	bool waitVoiceProcessingPassEnd(unsigned int _waitTime = INFINITE);
	bool waitStreamEnd(unsigned int _waitTime = INFINITE);
	bool waitBufferStart(unsigned int _waitTime = INFINITE);
	bool waitBufferEnd(unsigned int _waitTime = INFINITE);
	bool waitLoopEnd(unsigned int _waitTime = INFINITE);
	bool waitVoiceError(unsigned int _waitTime = INFINITE);
	bool isVoiceProcessingPassStart();
	bool isVoiceProcessingPassEnd();
	bool isStreamEnd();
	bool isBufferStart();
	bool isBufferEnd();
	bool isLoopEnd();
	bool isVoiceError();
	bool resetVoiceProcessingPassStart();
	bool resetVoiceProcessingPassEnd();
	bool resetStreamEnd();
	bool resetBufferStart();
	bool resetBufferEnd();
	bool resetLoopEnd();
	bool resetVoiceError();
protected:
	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32);
	STDMETHOD_(void, OnVoiceProcessingPassEnd)();
	STDMETHOD_(void, OnStreamEnd)();
	STDMETHOD_(void, OnBufferStart)(void*);
	STDMETHOD_(void, OnBufferEnd)(void*);
	STDMETHOD_(void, OnLoopEnd)(void*);
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT);
protected:
	HANDLE	evtVoiceProcessingPassStart_;
	HANDLE	evtVoiceProcessingPassEnd_;
	HANDLE	evtStreamEnd_;
	HANDLE	evtBufferStart_;
	HANDLE	evtBufferEnd_;
	HANDLE	evtLoopEnd_;
	HANDLE	evtVoiceError_;
};

}
