/*!
 *=================================================================================
 * @file	Core.h
 * @author	kawatanik
 * @date	20141216
 * @brief	XAudio2���K�p�T�E���h�h���C�o�̃R�A����.
 *=================================================================================
 */
#pragma once

//! XAudio2�C���^�[�t�F�C�X�̑O���錾
struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace xaudio_drv{
//! �V���O���g���C���X�^���X�^�̐錾
typedef class Core* const& CoreInstanceRef;

//! �I�u�W�F�N�g�̑O���錾
class Master;
class Bus;
class Snd;
class Wave;


//! �T�E���h�h���C�o�R�A�N���X(�V���O���g��)
class Core final{
//! �R���X�g���N�^�ƃf�X�g���N�^
private:
	Core();
	~Core();

//! �V���O���g���C���^�[�t�F�C�X
public:
	static void				createInstance();
	static void				deleteInstance();
	static CoreInstanceRef	getInstanceRef();

//! �A�N�Z�X�֐�
public:
	bool			initialize();
	bool			finalize();
	void			setMasterVolume(float _volume);
	float			getMasterVolume() const;
	Snd*			createSnd(const Wave* _wave, const void* _bufAddr);



//! �����o�ϐ�
protected:
	IXAudio2*				xaudioEngine_;	//!< XAudio2�̃G���W���C���X�^���X�ւ̃|�C���^
	IXAudio2MasteringVoice*	masterTrack_;	//!< �}�X�^�����O�{�C�X(�}�X�^�[�g���b�N)�ւ̃|�C���^
};
}
