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
static int threadFunc(std::future<bool>* _initWait);

//! �R�A�̃V���O���g���C���X�^���X�ւ̎Q��
static CoreInstanceRef	core_ = Core::getInstanceRef();

//! �����ϐ�
static bool					isInitialized_ = false;	//!< �������ς݃t���O
static bool					isReqFinalize_ = false;	//!< �I�����N�G�X�g
static std::thread*			thread_ = NULL;			//!< �X���b�h�I�u�W�F�N�g
static std::vector<Snd*>	sndArray_;				//!< �����I�u�W�F�N�g�z��
static std::vector<Wave*>	waveArray_;				//!< Wave�t�@�C���z��
static std::vector<void*>	sndBufArray_;			//!< �����o�b�t�@�z��


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
		std::promise<bool> init_promise;
		thread_ = new std::thread(threadFunc, &init_promise.get_future());

		// �T�E���h�X���b�h�̊J�n
		init_promise.set_value(true);
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
 * �T���v���p�������ςȂ��Đ��֐�
 */
void sample_play(const char* _fileName){
	// Wave�t�@�C�����J��
	Wave* wave = new Wave();
	wave->open(_fileName);

	// �o�b�t�@�̊m�ۂƔg�`�f�[�^�̓ǂ݂���
	void* buf = malloc(wave->getWaveSize());
	wave->readBuffer(buf, wave->getWaveSize());

	// �����I�u�W�F�N�g�����
	Snd* snd = core_->createSnd(wave, static_cast<const void*>(static_cast<const char*>(wave->getMappedAddr()) + wave->getWaveOffset()));


	// �Đ��J�n
	snd->play();

	// �Ǘ��p�ɓo�^
	sndArray_.push_back(snd);
	waveArray_.push_back(wave);
	sndBufArray_.push_back(buf);
}



/*!
 * �X���b�h�֐�
 */
int threadFunc(std::future<bool>* _initWait){
	// �����������҂��ƃG���[�`�F�b�N
	if(_initWait->get());
	else
		return -1;

	while(isReqFinalize_ == false){
		if(sndArray_.size() > 0);
		else
			continue;

		for(Snd*& targ : sndArray_){
			if(targ)
				targ->exec();
		}
	}

	isReqFinalize_ = false;
	return 0;
}
}
