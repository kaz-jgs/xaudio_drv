/*!
 *=================================================================================
 * @file	Drv.cpp
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2���K�p�T�E���h�h���C�o�̖{��.
 *=================================================================================
 */

#include <vector>
#include <thread>
#include <future>

#include <XAudio2.h>

#include "Drv.h"
#include "Core.h"
#include "Snd.h"
#include "Wave.h"

#include "util/macros_debug.h"

namespace xaudio_drv{

//! �萔�錾
static unsigned long	THREAD_STACK_SIZE = 0x400;

//! �X���b�h�֐��̃v���g�^�C�v�錾
static int threadFunc(std::future<bool>& _initWait);

//! �R�A�̃V���O���g���C���X�^���X�ւ̎Q��
static CoreInstanceRef	core_ = Core::getInstanceRef();

//! �����ϐ�
static bool					isInitialized_ = false;	//!< �������ς݃t���O
static bool					isReqFinalize_ = false;	//!< �I�����N�G�X�g
static std::thread*			thread_ = NULL;			//!< �X���b�h�I�u�W�F�N�g
static std::promise<bool>	initPromiss_;			//!< �����������p�v���~�X�I�u�W�F�N�g
static std::vector<Snd*>	sndArray_;				//!< �����I�u�W�F�N�g�z��
static std::vector<Wave*>	waveArray_;				//!< Wave�t�@�C���z��
static std::vector<void*>	sndBufArray_;			//!< �����o�b�t�@�z��

static inline Snd* searchSnd(SndHandle _hdl);

/*!
 * ������
 */
bool initialize(){
	// �ߏ������h�~
	if(isInitialized_)
		return false;

	// �R�A�̃V���O���g���C���X�^���X�𐶐�
	Core::createInstance();
	AssertMsgBox(core_, "xaudio_drv::initialize() : couldn't create core instance");

	// �R�A�̏�����
	if(core_->initialize());
	else{
		AssertMsgBox(false, "xaudio_drv::initialize() : couldn't initialize core.");
		return false;
	}

	// �T�E���h�X���b�h�̐���
	{
		// �X���b�h�I�u�W�F�N�g�Ɠ����I�u�W�F�N�g�̐���
		thread_ = new std::thread(threadFunc, initPromiss_.get_future());

		// @todo �X���b�h�̏ڍאݒ�

		// �T�E���h�X���b�h�̊J�n����
		initPromiss_.set_value(true);
	}

	// �������ς݃t���O�𗧂Ă�true��Ԃ�
	isInitialized_ = true;
	return true;
}


/*!
 * �I��
 */
bool finalize(){
	// �߉���h�~
	if(isInitialized_);
	else
		return false;

	// �X���b�h�̏I��
	if(thread_){
		// �T�E���h�X���b�h�̏I�����N�G�X�g
		isReqFinalize_ = true;

		// �T�E���h�X���b�h�̏I���҂�
		thread_->join();

		// �X���b�h�I�u�W�F�N�g�̔j��
		delete thread_;
		thread_ = NULL;
	}

	// �����I�u�W�F�N�g�̔j��
	for (Snd*& targ : sndArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}

	// Wave�t�@�C���̔j��
	for (Wave*& targ : waveArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}

	// �����o�b�t�@�̔j��
	for (void*& targ : sndBufArray_){
		if (targ){
			free(targ);
			targ = NULL;
		}
	}

	// �R�A�̏I��
	if(Core::getInstanceRef()->finalize());
	else{
		AssertMsgBox(false, "xaudio_drv::finalize() : couldn't finalize core.");
		return false;
	}

	// �R�A�̃C���X�^���X�j��
	Core::deleteInstance();

	// �������ς݃t���O�𗎂Ƃ���true��Ԃ�
	isInitialized_ = false;
	return true;
}


/*!
 * �Ώۂ��~����
 */
bool stop(SndHandle _handle, float _fadeTime/* = 0.f*/){
	// �����I�u�W�F�N�g�̌���
	if (Snd* targ = searchSnd(_handle)){
		//���������� ��~���Č��ʂ�Ԃ�
		return targ->stop(_fadeTime);
	}

	return false;
}


/*!
 * �Ώۂ̉��ʂ�ύX����
 */
bool setVolume(SndHandle _handle, float _volume, float _fadeTime/* = 0.f*/){
	// �����I�u�W�F�N�g�̌���
	if (Snd* targ = searchSnd(_handle)){
		// ����������l��ݒ肵��true��Ԃ�
		targ->setVolume(_volume, _fadeTime);
		return true;
	}

	return false;
}


/*!
 * �Ώۂ̉��ʂ��擾����
 */
float getVolume(SndHandle _handle){
	// �����I�u�W�F�N�g�̌���
	if (Snd* targ = searchSnd(_handle)){
		// ���������猋�ʂ�Ԃ�
		return targ->getVolume();
	}

	// ������Ȃ������ꍇ��0��Ԃ�
	return 0.f;
}


/*!
 * �Ώۂ��Đ������ǂ����̎擾
 */
bool isPlaying(SndHandle _handle){
	// �����I�u�W�F�N�g�̌���
	if (Snd* targ = searchSnd(_handle)){
		// ������΍Đ������ǂ�����Ԃ�
		return targ->isPlaying();
	}

	return false;
}


/*!
 * �������Đ������ǂ����̎擾
 */
bool isPlayingAny(){
	// �����I�u�W�F�N�g�z��̑���
	for (Snd*& targ : sndArray_){
		// �I�u�W�F�N�g��NULL�Ȃ珈�����͂���
		if (targ);
		else
			continue;

		// �Đ����̂��̂������true��Ԃ�
		if (targ->isPlaying()){
			return true;
		}
	}

	return false;
}



/*!
 * �T���v���p�������ςȂ��Đ��֐�
 */
SndHandle sample_play(const char* _fileName){
	// Wave�t�@�C�����J��
	Wave* wave = new Wave();
	wave->open(_fileName);

	// �o�b�t�@�̊m�ۂƔg�`�f�[�^�̓ǂ݂���
	void* buf = malloc(wave->getWaveSize());
	wave->readBuffer(buf, wave->getWaveSize());

	// �����I�u�W�F�N�g�����
	Snd* snd = core_->createSnd(wave, static_cast<const void*>(static_cast<const char*>(buf)+wave->getWaveOffset()));


	// �Đ��J�n
	snd->play();

	// �Ǘ��p�ɓo�^
	sndArray_.push_back(snd);
	waveArray_.push_back(wave);
	sndBufArray_.push_back(buf);

	// �n���h����Ԃ�
	return snd->getHandle();
}



/*!
 * �X���b�h�֐�
 */
int threadFunc(std::future<bool>& _initWait){
	// �����������҂��ƃG���[�`�F�b�N
	if(_initWait.get());
	else
		return -1;

	while(isReqFinalize_ == false){
		// �R�A�̎��s����
		core_->exec();

		// �����Ώۂ��Ȃ���΃��[�v�擪��
		if(sndArray_.size() > 0);
		else
			continue;

		// �����I�u�W�F�N�g�̎��s����
		for(Snd*& targ : sndArray_){
			if(targ)
				targ->exec();
		}
	}

	isReqFinalize_ = false;
	return 0;
}

/*!
 * �����I�u�W�F�N�g�̌����C�����C���֐�
 */
static inline Snd* searchSnd(SndHandle _hdl){
	for (Snd*& targ : sndArray_){
		// �I�u�W�F�N�g��NULL�Ȃ珈�����͂���
		if (targ);
		else
			continue;

		// �n���h������v����ΑΏۂ�Ԃ�
		if (targ->getHandle() == _hdl){
			return targ;
		}
	}

	return NULL;
}

}


//----- �ȉ�C�C���^�[�t�F�C�X�̎���
bool XAUDIO_DRV_Initialize(){
	return xaudio_drv::initialize();
}
bool XAUDIO_DRV_Finalize(){
	return xaudio_drv::finalize();
}
bool XAUDIO_DRV_IsPlaying(SndHandle _handle){
	return xaudio_drv::isPlaying(_handle);
}
bool XAUDIO_DRV_IsPlayingAny(){
	return xaudio_drv::isPlayingAny();
}