/*!
 *=================================================================================
 * @file	Wave.h
 * @author	kawatanik
 * @date	20141218
 * @brief	XAudio2���K�p�T�E���h�h���C�o Wave�t�@�C���N���X.
 *
 * ���̃t�@�C����include�O��Windows.h��include����Ă���K�v������܂�.
 * 4GB�ȏ�̃t�@�C����H�킷�Ɣ�т܂�.
 *=================================================================================
 */
#pragma once

namespace xaudio_drv{
class Wave{
//! �R���X�g���N�^�ƃf�X�g���N�^
public:
	Wave();
	virtual ~Wave(){ close(); }

//! �A�N�Z�X�֐�
public:
	bool			open(const char* _fileName);										//!< �t�@�C�����J��(ANSI��)
	bool			open(const wchar_t* _fileName);										//!< �t�@�C�����J��(UNICODE��)
	void			close();															//!< �t�@�C�������

	bool			seek(unsigned long _offset);										//!< �t�@�C���̃V�[�N
	bool			seekWave();															//!< Wave�f�[�^�{�̐擪�ւ̃V�[�N

	unsigned long	getFileSize() const { return size_; }								//!< �t�@�C���T�C�Y�̎擾
	unsigned long	getWaveSize() const { return waveSize_; }							//!< Wave�f�[�^�{�̂̃T�C�Y�̎擾

	unsigned long	readBuffer(void* _outBuffer, unsigned long _size = 0xffffffff);		//!< �o�b�t�@�̓ǂݍ���
	const void*		getMappedAddr() const{ return mappedAddr_; }						//!< �t�@�C���}�b�s���O�A�h���X�̎擾
	unsigned long	getWaveOffset() const{ return waveOffset_; }						//!< Wave�f�[�^�{�̐擪�I�t�Z�b�g�̎擾

	//!< ���[�v���̎擾
	unsigned long	getLoopCount() const{ return loopCount_; }
	unsigned long	getLoopBegin() const{ return loopBegin_; }
	unsigned long	getLoopLength() const{ return loopLength_; }

	//!< Wave�t�H�[�}�b�g�̎擾
	const WAVEFORMATEX*	getWaveFormatEx() const{ return &fmt_; }

//! �����o�֐�
protected:
	bool			mapping();															//!< �}�b�s���O
	bool			parse();															//!< �p�[�X
	inline bool		prepare();															//!< �t�@�C���I�[�v����̃}�b�s���O�ƃp�[�X�̏����܂Ƃ�
	inline void*	searchChunk(const char* _name) const;								//!< �`�����N�̌���
	inline void		clearMember();														//!< �����o�̃N���A 


//! �����o�ϐ�
protected:
	HANDLE			fileHandle_;														//!< �t�@�C���n���h��

	HANDLE			mapHandle_;															//!< �t�@�C���}�b�s���O�n���h��
	void*			mappedAddr_;														//!< �t�@�C���}�b�s���O�A�h���X

	unsigned long	size_;																//!< �t�@�C���T�C�Y
	unsigned long	offset_;															//!< �ǂݍ��݃o�b�t�@�̃I�t�Z�b�g

	unsigned long	waveOffset_;														//!< Wave�f�[�^�{�̂ւ̃I�t�Z�b�g
	unsigned long	waveSize_;															//!< Wave�f�[�^�{�̂̃T�C�Y

	unsigned long	loopCount_;															//!< ���[�v�J�E���g
	unsigned long	loopBegin_;															//!< ���[�v�J�n�T���v����
	unsigned long	loopLength_;														//!< ���[�v�̒���

	WAVEFORMATEX	fmt_;																//!< Wave�t�H�[�}�b�g�\����
};

}
