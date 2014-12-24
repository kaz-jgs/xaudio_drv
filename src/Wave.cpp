/*!
 *=================================================================================
 * @file	Wave.cpp
 * @author	kaz-jgs
 * @date	20141218
 * @brief	XAudio2���K�p�T�E���h�h���C�o Wave�t�@�C���N���X.
 *=================================================================================
 */
#include <Windows.h>
#include <XAudio2.h>

#include "Wave.h"
#include "util/macros_debug.h"

#pragma warning(disable : 4200) // Data�`�����N�̉ϒ��f�[�^�z��p�̌x������

/*!
 * �`�����N�֘A�̐錾
 */
// ---------------------------------------- �������� ----------------------------------------
#define RIFF_CHUNK_STRING_RIFF	"RIFF"
#define RIFF_CHUNK_STRING_WAVE	"WAVE"
#define RIFF_CHUNK_STRING_FMT	"fmt "
#define RIFF_CHUNK_STRING_SMPL	"smpl"
#define RIFF_CHUNK_STRING_DATA	"DATA"
#define RIFF_CHUNK_ID_SIZE		(4)

//! RIFF�w�b�_
struct ChunkRiff{
	char			headerId[RIFF_CHUNK_ID_SIZE];	//!< "RIFF"�������Ă�
	unsigned long	size;							//!< �f�[�^�T�C�Y
	char			riffType[RIFF_CHUNK_ID_SIZE];	//!< �`�����N�̎��(WAVE��z��)
};

//! fmt�`�����N
struct ChunkFmt{
	char			headerId[RIFF_CHUNK_ID_SIZE];	//!< "fmt "�������Ă���
	unsigned long	size;							//!< �f�[�^�T�C�Y
	unsigned short	formatId;						//!< �t�H�[�}�b�gID
	unsigned short	channelNum;						//!< �`���l����
	unsigned long	sampleRate;						//!< �T���v�����O���[�g
	unsigned long	bytePerSec;						//!< Byte/sec
	unsigned short	blockSize;						//!< �u���b�N�A���C�������g
	unsigned short	quantizeBit;					//!< �ʎq���r�b�g��
};

//! smpl�`�����N
struct ChunkSmpl{
	char			headerId[RIFF_CHUNK_ID_SIZE];	//!< "smpl"�������Ă���
	unsigned long	size;							//!< �f�[�^�T�C�Y
	unsigned long	manufacture;					//!< �f�[�^�󂯓���MIDI�@��̃��[�J�[�ŗL�ԍ�
	unsigned long	product;						//!< �f�[�^�󂯓���MIDI�@��̐��i�ԍ�
	unsigned long	samplePeriod;					//!< �i�m�b�P�ʂ̍Đ�����
	unsigned long	unityNote;						//!< ��s�b�`
	unsigned long	pitchFraction;					//!< unityNote����̃s�b�`�ϓ���(0x80000000�Ŕ�����)
	unsigned long	smpteFormat;					//!< SMPTE�̌`��
	unsigned long	smpteOffset;					//!< SMPTE�̃I�t�Z�b�g����(0xhhmmssff)
	unsigned long	loopInfoNum;					//!< ���[�v���\���̂̐�
	unsigned long	loopInfoSize;					//!< ���[�v���\���̃t�B�[���h����߂�T�C�Y

	//!< ���[�v���\����
	struct{
		unsigned long	cuePoint;					//!< �ق��̃`�����N��CUE�|�C���g�Ɗ֘A�t��������ID
		unsigned long	loopType;					//!< ���[�v�̃^�C�v
		unsigned long	startPoint;					//!< ���[�v�n�_�T���v���ԍ�
		unsigned long	endPoint;					//!< ���[�v�I�_�T���v���ԍ�
		unsigned long	fraction;
		unsigned long	playCount;
	}loopInfo[];
};

// DATA�`�����N
struct ChunkData{
	char	headerId[RIFF_CHUNK_ID_SIZE];			// "DATA"�������Ă���
	DWORD	size;									// �f�[�^�T�C�Y
	char	data[];
};
// ---------------------------------------- �����܂� ----------------------------------------

namespace xaudio_drv{
/*!
 * @brief	�R���X�g���N�^
 */
Wave::Wave():
fileHandle_(INVALID_HANDLE_VALUE),
mapHandle_(INVALID_HANDLE_VALUE),
mappedAddr_(NULL),
size_(0),
waveSize_(0),
offset_(0),
waveOffset_(0),
fmt_({0})
{}

/*!
 * @brief �t�@�C���I�[�v��
 *
 * �t�@�C�����I�[�v�����ă}�b�s���O�֐����Ăяo���܂�.
 */
//! �t�@�C���I�[�v���̑������ȍ~(W��A�ŋ��ʂ̂��̂��g������)
#define WAVE_OPEN_ARGS GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
//!< ANSI��
bool Wave::open(const char* _fileName){
	// ���d�I�[�v���h�~
	if(fileHandle_ == INVALID_HANDLE_VALUE);
	else{
		AssertMsgBox(false, "a file has already opend in this class.");
		return false;
	}

	// �t�@�C���I�[�v��
	fileHandle_ = CreateFileA(_fileName, WAVE_OPEN_ARGS);
	AssertMsgBox(fileHandle_ != INVALID_HANDLE_VALUE, "couldn't open file");

	// �t�@�C�������̌��ʂ�Ԃ�
	return prepare();
}
//!< UNICODE��
bool Wave::open(const wchar_t* _fileName){
	// ���d�I�[�v���h�~
	if (fileHandle_ == INVALID_HANDLE_VALUE);
	else{
		AssertMsgBox(false, "a file has already opend in this class.");
		return false;
	}

	// �t�@�C���I�[�v��
	fileHandle_ = CreateFileW(_fileName, WAVE_OPEN_ARGS);
	AssertMsgBox(fileHandle_ != INVALID_HANDLE_VALUE, "couldn't open file");

	// �t�@�C�������̌��ʂ�Ԃ�
	return prepare();
}
#undef WAVE_OPEN_ARGS // define����


/*!
 * @brief	�t�@�C���N���[�Y
 *
 * �}�b�s���O�̉���܂ōs���܂�.
 */
void Wave::close(){
	if(mappedAddr_){
		UnmapViewOfFile(mappedAddr_);
		mappedAddr_ = NULL;
	}

	if(mapHandle_ == INVALID_HANDLE_VALUE);
	else{
		CloseHandle(mapHandle_);
		mapHandle_ = INVALID_HANDLE_VALUE;
	}

	if(fileHandle_ == INVALID_HANDLE_VALUE);
	else{
		CloseHandle(fileHandle_);
		fileHandle_ = INVALID_HANDLE_VALUE;
	}

	clearMember();
}


/*!
 * @brief	�t�@�C���̃V�[�N
 */
bool Wave::seek(unsigned long _offset){
	if(mappedAddr_);
	else
		return false;

	if(size_ < _offset)
		return false;

	offset_ = _offset;
	return true;
}


/*!
 * @brief	Waver�f�[�^�{�̐擪�ւ̃V�[�N
 */
bool Wave::seekWave(){
	return seek(waveOffset_);
}

/*!
 * @brief	�t�@�C���̃������}�b�s���O
 */
bool Wave::mapping(){
	// �t�@�C���n���h���̃`�F�b�N
	if(fileHandle_ == INVALID_HANDLE_VALUE)
		return false;

	// �}�b�s���O�ς݂łȂ����ǂ���
 	if(mapHandle_ == INVALID_HANDLE_VALUE);
	else
		return false;

	// �}�b�s���O
	mapHandle_ = CreateFileMapping(fileHandle_, NULL, PAGE_READONLY, 0, 0, NULL);
	AssertMsgBox(mapHandle_ != INVALID_HANDLE_VALUE, "couldn't map file");

	// �}�b�v�g�A�h���X�̎擾
	mappedAddr_ = MapViewOfFile(mapHandle_, FILE_MAP_READ, 0, 0, 0);

	// �}�b�v�g�A�h���X���擾�ł��Ă��邩�ǂ�����Ԃ�
	return mappedAddr_ != NULL;
}


/*!
 * @brief �t�@�C���̃p�[�X
 */
bool Wave::parse(){
	// �t�H�[�}�b�g�̏�����
	memset(&fmt_, 0, sizeof(fmt_));
	fmt_.cbSize = sizeof(fmt_);

	bool ret = true;

	// RIFF�`�����N�̃`�F�b�N
	{
		const ChunkRiff* const riff = static_cast<const ChunkRiff*>(mappedAddr_);
		if (_strnicmp(riff->headerId, RIFF_CHUNK_STRING_RIFF, RIFF_CHUNK_ID_SIZE) == 0);
		else
			return false;
		if (_strnicmp(riff->riffType, RIFF_CHUNK_STRING_WAVE, RIFF_CHUNK_ID_SIZE) == 0);
		else
			return false;
	}

	// FMT�`�����N
	{
		// �`�����N�̌���
		const ChunkFmt* const chunk = static_cast<ChunkFmt*>(searchChunk(RIFF_CHUNK_STRING_FMT));
		if (chunk){
			// Wave�t�H�[�}�b�g�f�[�^�ւ̔��f
			fmt_.nAvgBytesPerSec	= chunk->bytePerSec;
			fmt_.nBlockAlign		= chunk->blockSize;
			fmt_.nChannels			= chunk->channelNum;
			fmt_.nSamplesPerSec		= chunk->sampleRate;
			fmt_.wFormatTag			= chunk->formatId;
			fmt_.wBitsPerSample		= chunk->quantizeBit;
		}
		else
			return false;
	}

	// DATA�`�����N
	{
		// DATA�`�����N�̌���
		const ChunkData* const chunk = static_cast<ChunkData*>(searchChunk(RIFF_CHUNK_STRING_DATA));
		if (chunk);
		else
			return false;

		// �f�[�^�擪�܂ł̃I�t�Z�b�g���v�Z���ĕێ�
		offset_ = waveOffset_ = static_cast<unsigned long>(chunk->data - static_cast<char*>(mappedAddr_));

		// �f�[�^�T�C�Y��ێ�
		waveSize_ = chunk->size;
	}

	// smpl�`�����N�̌���
	// @note ����ł̓��[�v���̐擪�̂ݑΉ����܂�
	{
		// @note smpl�`�����N�͔C�Ӄ`�����N�Ȃ̂Ō�����Ȃ��Ă��G���[�����͂��Ȃ�
		const ChunkSmpl* const chunk = static_cast<ChunkSmpl*>(searchChunk(RIFF_CHUNK_STRING_SMPL));
		if (chunk){
			// ���[�v���1�ȏ゠��ΐ擪�̃��[�v����XAudio2�̃��[�v���ɕϊ����ĕێ�
			if(chunk->loopInfoNum > 0){
				loopBegin_ = chunk->loopInfo[0].startPoint;
				loopLength_ = chunk->loopInfo[0].endPoint - chunk->loopInfo[0].startPoint;
				loopCount_ = (chunk->loopInfo[0].playCount > 0)? chunk->loopInfo[0].playCount : XAUDIO2_LOOP_INFINITE;
			}
		}
	}

	return true;
}


/*!
 * @brief �t�@�C���̏���
 *
 * �}�b�s���O�ƃp�[�X�̂܂Ƃ�.
 * ANSI�ł�UNICODE�ł̃t�@�C���I�[�v�������̌㔼���ʕ���.
 */
bool Wave::prepare(){
	// �t�@�C���T�C�Y�擾
	if(fileHandle_ == INVALID_HANDLE_VALUE);
	else{
		size_ = GetFileSize(fileHandle_, NULL);
	}

	// �}�b�s���O
	if (mapping());
	else
		return false;

	// �p�[�X
	if (parse());
	else{
		close();
		return false;
	}

	return true;
}


/*!
 * @brief		�o�b�t�@�̓ǂݍ���
 * @param[out]	_outBuffer	�o�̓o�b�t�@�ւ̃|�C���^
 * @param[in]	_size		�ő�ǂݏo���T�C�Y(�f�t�H���g�ł̓t�@�C���S��)
 * @retval		���ۂ̓ǂݏo���T�C�Y
 */
unsigned long Wave::readBuffer(void* _outBuffer, unsigned long _size /* = 0xffffffff */){
	// �I�t�Z�b�g���i�ݐ؂��Ă�����0��Ԃ�
	if(size_ > offset_);
	else
		return 0;

	// �ǂݏo���T�C�Y�̕ێ�
	unsigned long ret = (_size < size_ - offset_)? _size : size_ - offset_;

	// �}�b�v�g�A�h���X����memcpy
	memcpy_s(_outBuffer, _size, static_cast<char*>(mappedAddr_) + offset_, ret);

	// �I�t�Z�b�g��i�߂�
	offset_ += ret;

	return ret;
}


/*!
 * @brief		�`�����N�̌���
 * @param[in]	_name	�`�����N��
 * @retval		�`�����N�̐擪�A�h���X
 */
void* Wave::searchChunk(const char* _name) const{
	// �A�h���X�i�[��|�C���^�̐錾
	void* chunk = NULL;

	// �Ώۃ`�����N�̃w�b�_�������T��
	for (unsigned long cnt = 0; cnt < size_; cnt++){
		// �����񂪂����Ă���ΊY���A�h���X��_chunk�ɓ����break
		if (!_strnicmp(static_cast<const char*>(mappedAddr_) + cnt, _name, RIFF_CHUNK_ID_SIZE)){
			chunk = static_cast<void*>(static_cast<char*>(mappedAddr_) + cnt);
			break;
		}
	}

	return chunk;
}


/*!
 * @brief	�����o�̃N���A
 */
void Wave::clearMember(){
	size_ = 0;
	waveSize_ = 0;

	offset_ = 0;
	waveOffset_ = 0;
	
	loopCount_ = 0;
	loopBegin_ = 1; // �T���v���ԍ���1�J�n
	loopLength_ = 0;

	memset(&fmt_, 0, sizeof(fmt_));
}

}
