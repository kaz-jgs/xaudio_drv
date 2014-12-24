/*!
 *=================================================================================
 * @file	SndCallback.h
 * @author	kawatanik
 * @date	20141218
 * @brief	XAudio2���K�p�T�E���h�h���C�o �����I�u�W�F�N�g�p�R�[���o�b�N�N���X.
 *=================================================================================
 */
#include <XAudio2.h>
#include "SndCallback.h"


namespace xaudio_drv{

/*!
 * �R���X�g���N�^
 */
SndCallback::SndCallback():
evtVoiceProcessingPassStart_	(CreateEvent(NULL, TRUE, FALSE, NULL)),
evtVoiceProcessingPassEnd_		(CreateEvent(NULL, TRUE, FALSE, NULL)),
evtStreamEnd_					(CreateEvent(NULL, TRUE, FALSE, NULL)),
evtBufferStart_					(CreateEvent(NULL, TRUE, FALSE, NULL)),
evtBufferEnd_					(CreateEvent(NULL, TRUE, FALSE, NULL)),
evtLoopEnd_						(CreateEvent(NULL, TRUE, FALSE, NULL)),
evtVoiceError_					(CreateEvent(NULL, TRUE, FALSE, NULL))
{}


/*!
 * �f�X�g���N�^
 */
SndCallback::~SndCallback(){
	CloseHandle(evtVoiceProcessingPassStart_);
	CloseHandle(evtVoiceProcessingPassEnd_);
	CloseHandle(evtStreamEnd_);
	CloseHandle(evtBufferStart_);
	CloseHandle(evtBufferEnd_);
	CloseHandle(evtLoopEnd_);
	CloseHandle(evtVoiceError_);
}



/*!
 * �ewait�֐��̎���
 */
bool SndCallback::waitVoiceProcessingPassStart(unsigned int _waitTime/* = INFINITE*/){
	return WaitForSingleObject(evtVoiceProcessingPassStart_, _waitTime) == WAIT_OBJECT_0;
}
bool SndCallback::waitVoiceProcessingPassEnd(unsigned int _waitTime/* = INFINITE*/){
	return WaitForSingleObject(evtVoiceProcessingPassEnd_, _waitTime) == WAIT_OBJECT_0;
}
bool SndCallback::waitStreamEnd(unsigned int _waitTime/* = INFINITE*/){
	return WaitForSingleObject(evtStreamEnd_, _waitTime) == WAIT_OBJECT_0;
}
bool SndCallback::waitBufferStart(unsigned int _waitTime/* = INFINITE*/){
	return WaitForSingleObject(evtBufferStart_, _waitTime) == WAIT_OBJECT_0;
}
bool SndCallback::waitBufferEnd(unsigned int _waitTime/* = INFINITE*/){
	return WaitForSingleObject(evtBufferEnd_, _waitTime) == WAIT_OBJECT_0;
}
bool SndCallback::waitLoopEnd(unsigned int _waitTime/* = INFINITE*/){
	return WaitForSingleObject(evtLoopEnd_, _waitTime) == WAIT_OBJECT_0;
}
bool SndCallback::waitVoiceError(unsigned int _waitTime/* = INFINITE*/){
	return WaitForSingleObject(evtVoiceError_, _waitTime) == WAIT_OBJECT_0;
}


/*!
 * �e�`�F�b�N�֐��̎���
 */
bool SndCallback::isVoiceProcessingPassStart()	{ return waitVoiceProcessingPassStart(0); }
bool SndCallback::isVoiceProcessingPassEnd()	{ return waitVoiceProcessingPassEnd(0); }
bool SndCallback::isStreamEnd()					{ return waitStreamEnd(0); }
bool SndCallback::isBufferStart()				{ return waitBufferStart(0); }
bool SndCallback::isBufferEnd()					{ return waitBufferEnd(0); }
bool SndCallback::isLoopEnd()					{ return waitLoopEnd(0); }
bool SndCallback::isVoiceError()				{ return waitVoiceError(0); }


/*!
 * �e���Z�b�g�֐��̎���
 */
bool SndCallback::resetVoiceProcessingPassStart(){
	return ResetEvent(evtVoiceProcessingPassStart_) != FALSE;
}
bool SndCallback::resetVoiceProcessingPassEnd(){
	return ResetEvent(evtVoiceProcessingPassEnd_) != FALSE;
}
bool SndCallback::resetStreamEnd(){
	return ResetEvent(evtStreamEnd_) != FALSE;
}
bool SndCallback::resetBufferStart(){
	return ResetEvent(evtBufferStart_) != FALSE;
}
bool SndCallback::resetBufferEnd(){
	return ResetEvent(evtBufferEnd_) != FALSE;
}
bool SndCallback::resetLoopEnd(){
	return ResetEvent(evtLoopEnd_) != FALSE;
}
bool SndCallback::resetVoiceError(){
	return ResetEvent(evtVoiceError_) != FALSE;
}


/*!
 * �R�[���o�b�N�֐��̃I�[�o�[���C�h
 */
void SndCallback::OnVoiceProcessingPassStart(UINT32){
	SetEvent(evtVoiceProcessingPassStart_);
}
void SndCallback::OnVoiceProcessingPassEnd(){
	SetEvent(evtVoiceProcessingPassEnd_);
}
void SndCallback::OnStreamEnd(){
	SetEvent(evtStreamEnd_);
}
void SndCallback::OnBufferStart(void*){
	SetEvent(evtBufferStart_);
}
void SndCallback::OnBufferEnd(void*){
	SetEvent(evtBufferEnd_);
}
void SndCallback::OnLoopEnd(void*){
	SetEvent(evtLoopEnd_);
}
void SndCallback::OnVoiceError(void*, HRESULT){
	SetEvent(evtVoiceError_);
}

}
