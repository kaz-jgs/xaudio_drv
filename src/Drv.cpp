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
#include "Bus.h"

#include "util/macros_debug.h"

namespace xaudio_drv{

//! �萔�錾
static unsigned long		THREAD_STACK_SIZE = 0x400;


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
static std::vector<Bus*>	busArray_;				//!< �����I�u�W�F�N�g�z��
static std::vector<Wave*>	waveArray_;				//!< Wave�t�@�C���z��
static std::vector<void*>	sndBufArray_;			//!< �����o�b�t�@�z��


//! �C�����C���֐�
static inline Snd* searchSnd(SndHandle _hdl);
static inline Bus* searchBus(SndHandle _hdl);
static inline Bus* searchBus(const wchar_t* _busName);


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
	std::vector<Snd*>().swap(sndArray_);

	// �o�X�I�u�W�F�N�g�̔j��
	for (Bus*& targ : busArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}
	std::vector<Bus*>().swap(busArray_);

	// Wave�t�@�C���̔j��
	for (Wave*& targ : waveArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}
	std::vector<Wave*>().swap(waveArray_);

	// �����o�b�t�@�̔j��
	for (void*& targ : sndBufArray_){
		if (targ){
			free(targ);
			targ = NULL;
		}
	}
	std::vector<void*>().swap(sndBufArray_);

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
 * Snd�̍쐬
 */
SndHandle createSnd(const char* _fileName, bool _loadOnMemory/* = true*/, bool _isAutoPlay/* = false*/){
	// Wave�t�@�C�����J��
	Wave* wave = new Wave();
	wave->open(_fileName);


	// �]�����A�h���X�̎擾
	const char* addr = static_cast<const char*>(wave->getMappedAddr()) + wave->getWaveOffset();

	// �I�����������[�h���w�肳��Ă���ꍇ
	if (_loadOnMemory){
		// �o�b�t�@�̊m�ۂƓǂݏo��
		void* buf = malloc(wave->getWaveSize());
		wave->readBuffer(buf, wave->getWaveSize());

		// �o�b�t�@���Ǘ��p�z��ɓo�^
		sndBufArray_.push_back(buf);

		// �]�����A�h���X�̏㏑��
		addr = static_cast<const char*>(buf) + wave->getWaveOffset();
	}

	// �����I�u�W�F�N�g�����
	Snd* snd = core_->createSnd(wave, static_cast<const void*>(addr));

	// �Ǘ��p�z��ɓo�^
	sndArray_.push_back(snd);
	waveArray_.push_back(wave);

	// �Đ��J�n
	if (_isAutoPlay)
		snd->play();

	// �n���h����Ԃ�
	return snd->getHandle();
}



/*!
 * �Ώۂ�j������
 */
bool destroySnd(SndHandle _handle){
	// �����I�u�W�F�N�g�̌���
	for (Snd*& targ : sndArray_){
		if (targ);
		else
			continue;

		// ����������폜����true��Ԃ�
		if (targ->getHandle() == _handle){
			delete targ;
			targ = NULL;

			return true;
		}
	}

	return false;
}


/*!
 * �Ώۂ��L���ȉ����I�u�W�F�N�g���ǂ���
 */
bool isAvaliableSnd(SndHandle _handle){
	return searchSnd(_handle) != NULL;
}


/*!
 * �Ώۂ��Đ�����
 */
bool play(SndHandle _handle){
	// �����I�u�W�F�N�g�̌���
	if (Snd* targ = searchSnd(_handle)){
		//����������Đ����Č��ʂ�Ԃ�
		return targ->play();
	}

	return false;
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
	TrackBase* targ = searchSnd(_handle);

	// �����I�u�W�F�N�g��������Ȃ���΃o�X��T��
	if (targ);
	else{
		targ = searchBus(_handle);
	}

	// �����ł�������Ȃ����false��Ԃ�
	if (targ);
	else
		return false;

	// �ݒ肵��true��Ԃ�
	targ->setVolume(_volume, _fadeTime);
	return true;
}


/*!
 * �Ώۂ̉��ʂ��擾����
 */
float getVolume(SndHandle _handle){
	// �����I�u�W�F�N�g�̌���
	TrackBase* targ = searchSnd(_handle);

	// �����I�u�W�F�N�g��������Ȃ���΃o�X��T��
	if (targ);
	else{
		targ = searchBus(_handle);
	}

	// �����ł�������Ȃ����0��Ԃ�
	if (targ);
	else{
		AssertMsgBox(false, "[WARNING] getVolume : not found");
		return 0.f;
	}

	// ���ʂ�Ԃ�
	return targ->getVolume();
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
 * �o�X�̒ǉ�
 */
SndHandle addBus(const wchar_t* _busName){
	// (�f�o�b�O�ł̂� ���O���Ԃ�x��)
	AssertMsgBox(searchBus(_busName) == NULL, "[WARNING] bus name is duplicated !");

	Bus* bus = core_->createBus(_busName);
	busArray_.push_back(bus);

	return bus->getHandle();
}


/*!
 * �o�X�̉��ʐݒ�
 */
bool setBusVolume(const wchar_t* _busName, float _volume, float _fadeTime/* = 0.f*/){
	// �o�X��T��
	if (Bus* targ = searchBus(_busName)){
		// ����������l��ݒ肵��true��Ԃ�
		targ->setVolume(_volume, _fadeTime);
		return true;
	}

	return false;
}


/*!
 * �o�X�̉��ʎ擾
 */
float getBusVolume(const wchar_t* _busName){
	// �o�X��T��
	if (Bus* targ = searchBus(_busName)){
		// ���������猋�ʂ�Ԃ�
		return targ->getVolume();
	}

	return 0.f;
}


/*!
 * �Ώۂ��L���ȃo�X���ǂ���
 */
bool isAvaliableBus(const wchar_t* _busName){
	return searchBus(_busName) != NULL;
}


/*!
 * �o�̓o�X�̐ݒ�
 */
bool setOutputBus(SndHandle _src, const wchar_t* _outputBusName){
	// �Ώۂ��擾
	TrackBase* targ = searchSnd(_src);

	// �Ώۂ��擾�ł��Ă��Ȃ���΃o�X����T��
	if (targ);
	else{
		targ = searchBus(_src);
	}

	// �����܂łőΏۂ��擾�ł��Ă��Ȃ����false��Ԃ�
	if (targ);
	else{
		AssertMsgBox(false, "[ERROR] SndHandle(_src) is not available");
		return false;
	}

	// �o�̓o�X���擾
	Bus* out_bus = searchBus(_outputBusName);
	if (_outputBusName && out_bus == NULL)
		return false; // �o�X�����w�肳�ꂽ�ɂ�������炸NULL�̏ꍇ��false��Ԃ�

	// �o�̓o�X��ݒ肵�Č��ʂ�Ԃ�
	return targ->setOutputBus(out_bus, 1);
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

		// �����I�u�W�F�N�g�̎��s����
		for(Snd*& targ : sndArray_){
			if(targ)
				targ->exec();
		}

		// �o�X�̎��s����
		for(Bus*& targ : busArray_){
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

/*!
 * �o�X�I�u�W�F�N�g�̌����C�����C���֐�
 */
static inline Bus* searchBus(SndHandle _hdl){
	for (Bus*& targ : busArray_){
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

/*!
 * �o�X�I�u�W�F�N�g�̌����C�����C���֐�(�������)
 */
static inline Bus* searchBus(const wchar_t* _busName){
	for (Bus*& targ : busArray_){
		// �I�u�W�F�N�g��NULL�Ȃ珈�����͂���
		if (targ);
		else
			continue;

		// ���O����v����ΑΏۂ�Ԃ�
		if (wcscmp(targ->getBusName(), _busName) == 0){
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