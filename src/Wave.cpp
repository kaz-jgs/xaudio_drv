/*!
 *=================================================================================
 * @file	Wave.cpp
 * @author	kaz-jgs
 * @date	20141218
 * @brief	XAudio2練習用サウンドドライバ Waveファイルクラス.
 *=================================================================================
 */
#include <Windows.h>
#include <XAudio2.h>

#include "Wave.h"
#include "util/macros_debug.h"

#pragma warning(disable : 4200) // Dataチャンクの可変長データ配列用の警告無視

/*!
 * チャンク関連の宣言
 */
// ---------------------------------------- ここから ----------------------------------------
#define RIFF_CHUNK_STRING_RIFF	"RIFF"
#define RIFF_CHUNK_STRING_WAVE	"WAVE"
#define RIFF_CHUNK_STRING_FMT	"fmt "
#define RIFF_CHUNK_STRING_SMPL	"smpl"
#define RIFF_CHUNK_STRING_DATA	"DATA"
#define RIFF_CHUNK_ID_SIZE		(4)

//! RIFFヘッダ
struct ChunkRiff{
	char			headerId[RIFF_CHUNK_ID_SIZE];	//!< "RIFF"が入ってる
	unsigned long	size;							//!< データサイズ
	char			riffType[RIFF_CHUNK_ID_SIZE];	//!< チャンクの種類(WAVEを想定)
};

//! fmtチャンク
struct ChunkFmt{
	char			headerId[RIFF_CHUNK_ID_SIZE];	//!< "fmt "が入っている
	unsigned long	size;							//!< データサイズ
	unsigned short	formatId;						//!< フォーマットID
	unsigned short	channelNum;						//!< チャネル数
	unsigned long	sampleRate;						//!< サンプリングレート
	unsigned long	bytePerSec;						//!< Byte/sec
	unsigned short	blockSize;						//!< ブロックアラインメント
	unsigned short	quantizeBit;					//!< 量子化ビット数
};

//! smplチャンク
struct ChunkSmpl{
	char			headerId[RIFF_CHUNK_ID_SIZE];	//!< "smpl"が入っている
	unsigned long	size;							//!< データサイズ
	unsigned long	manufacture;					//!< データ受け入れMIDI機器のメーカー固有番号
	unsigned long	product;						//!< データ受け入れMIDI機器の製品番号
	unsigned long	samplePeriod;					//!< ナノ秒単位の再生時間
	unsigned long	unityNote;						//!< 基準ピッチ
	unsigned long	pitchFraction;					//!< unityNoteからのピッチ変動量(0x80000000で半音上)
	unsigned long	smpteFormat;					//!< SMPTEの形式
	unsigned long	smpteOffset;					//!< SMPTEのオフセット時間(0xhhmmssff)
	unsigned long	loopInfoNum;					//!< ループ情報構造体の数
	unsigned long	loopInfoSize;					//!< ループ情報構造体フィールドが占めるサイズ

	//!< ループ情報構造体
	struct{
		unsigned long	cuePoint;					//!< ほかのチャンクのCUEポイントと関連付けさせるID
		unsigned long	loopType;					//!< ループのタイプ
		unsigned long	startPoint;					//!< ループ始点サンプル番号
		unsigned long	endPoint;					//!< ループ終点サンプル番号
		unsigned long	fraction;
		unsigned long	playCount;
	}loopInfo[];
};

// DATAチャンク
struct ChunkData{
	char	headerId[RIFF_CHUNK_ID_SIZE];			// "DATA"が入っている
	DWORD	size;									// データサイズ
	char	data[];
};
// ---------------------------------------- ここまで ----------------------------------------

namespace xaudio_drv{
/*!
 * @brief	コンストラクタ
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
 * @brief ファイルオープン
 *
 * ファイルをオープンしてマッピング関数を呼び出します.
 */
//! ファイルオープンの第二引数以降(WとAで共通のものを使うため)
#define WAVE_OPEN_ARGS GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
//!< ANSI版
bool Wave::open(const char* _fileName){
	// 多重オープン防止
	if(fileHandle_ == INVALID_HANDLE_VALUE);
	else{
		AssertMsgBox(false, "a file has already opend in this class.");
		return false;
	}

	// ファイルオープン
	fileHandle_ = CreateFileA(_fileName, WAVE_OPEN_ARGS);
	AssertMsgBox(fileHandle_ != INVALID_HANDLE_VALUE, "couldn't open file");

	// ファイル準備の結果を返す
	return prepare();
}
//!< UNICODE版
bool Wave::open(const wchar_t* _fileName){
	// 多重オープン防止
	if (fileHandle_ == INVALID_HANDLE_VALUE);
	else{
		AssertMsgBox(false, "a file has already opend in this class.");
		return false;
	}

	// ファイルオープン
	fileHandle_ = CreateFileW(_fileName, WAVE_OPEN_ARGS);
	AssertMsgBox(fileHandle_ != INVALID_HANDLE_VALUE, "couldn't open file");

	// ファイル準備の結果を返す
	return prepare();
}
#undef WAVE_OPEN_ARGS // define解除


/*!
 * @brief	ファイルクローズ
 *
 * マッピングの解放まで行います.
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
 * @brief	ファイルのシーク
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
 * @brief	Waverデータ本体先頭へのシーク
 */
bool Wave::seekWave(){
	return seek(waveOffset_);
}

/*!
 * @brief	ファイルのメモリマッピング
 */
bool Wave::mapping(){
	// ファイルハンドルのチェック
	if(fileHandle_ == INVALID_HANDLE_VALUE)
		return false;

	// マッピング済みでないかどうか
 	if(mapHandle_ == INVALID_HANDLE_VALUE);
	else
		return false;

	// マッピング
	mapHandle_ = CreateFileMapping(fileHandle_, NULL, PAGE_READONLY, 0, 0, NULL);
	AssertMsgBox(mapHandle_ != INVALID_HANDLE_VALUE, "couldn't map file");

	// マップトアドレスの取得
	mappedAddr_ = MapViewOfFile(mapHandle_, FILE_MAP_READ, 0, 0, 0);

	// マップトアドレスが取得できているかどうかを返す
	return mappedAddr_ != NULL;
}


/*!
 * @brief ファイルのパース
 */
bool Wave::parse(){
	// フォーマットの初期化
	memset(&fmt_, 0, sizeof(fmt_));
	fmt_.cbSize = sizeof(fmt_);

	bool ret = true;

	// RIFFチャンクのチェック
	{
		const ChunkRiff* const riff = static_cast<const ChunkRiff*>(mappedAddr_);
		if (_strnicmp(riff->headerId, RIFF_CHUNK_STRING_RIFF, RIFF_CHUNK_ID_SIZE) == 0);
		else
			return false;
		if (_strnicmp(riff->riffType, RIFF_CHUNK_STRING_WAVE, RIFF_CHUNK_ID_SIZE) == 0);
		else
			return false;
	}

	// FMTチャンク
	{
		// チャンクの検索
		const ChunkFmt* const chunk = static_cast<ChunkFmt*>(searchChunk(RIFF_CHUNK_STRING_FMT));
		if (chunk){
			// Waveフォーマットデータへの反映
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

	// DATAチャンク
	{
		// DATAチャンクの検索
		const ChunkData* const chunk = static_cast<ChunkData*>(searchChunk(RIFF_CHUNK_STRING_DATA));
		if (chunk);
		else
			return false;

		// データ先頭までのオフセットを計算して保持
		offset_ = waveOffset_ = static_cast<unsigned long>(chunk->data - static_cast<char*>(mappedAddr_));

		// データサイズを保持
		waveSize_ = chunk->size;
	}

	// smplチャンクの検索
	// @note 現状ではループ情報の先頭のみ対応します
	{
		// @note smplチャンクは任意チャンクなので見つからなくてもエラー扱いはしない
		const ChunkSmpl* const chunk = static_cast<ChunkSmpl*>(searchChunk(RIFF_CHUNK_STRING_SMPL));
		if (chunk){
			// ループ情報が1つ以上あれば先頭のループ情報をXAudio2のループ情報に変換して保持
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
 * @brief ファイルの準備
 *
 * マッピングとパースのまとめ.
 * ANSI版とUNICODE版のファイルオープン処理の後半共通部分.
 */
bool Wave::prepare(){
	// ファイルサイズ取得
	if(fileHandle_ == INVALID_HANDLE_VALUE);
	else{
		size_ = GetFileSize(fileHandle_, NULL);
	}

	// マッピング
	if (mapping());
	else
		return false;

	// パース
	if (parse());
	else{
		close();
		return false;
	}

	return true;
}


/*!
 * @brief		バッファの読み込み
 * @param[out]	_outBuffer	出力バッファへのポインタ
 * @param[in]	_size		最大読み出しサイズ(デフォルトではファイル全体)
 * @retval		実際の読み出しサイズ
 */
unsigned long Wave::readBuffer(void* _outBuffer, unsigned long _size /* = 0xffffffff */){
	// オフセットが進み切っていたら0を返す
	if(size_ > offset_);
	else
		return 0;

	// 読み出しサイズの保持
	unsigned long ret = (_size < size_ - offset_)? _size : size_ - offset_;

	// マップトアドレスからmemcpy
	memcpy_s(_outBuffer, _size, static_cast<char*>(mappedAddr_) + offset_, ret);

	// オフセットを進める
	offset_ += ret;

	return ret;
}


/*!
 * @brief		チャンクの検索
 * @param[in]	_name	チャンク名
 * @retval		チャンクの先頭アドレス
 */
void* Wave::searchChunk(const char* _name) const{
	// アドレス格納先ポインタの宣言
	void* chunk = NULL;

	// 対象チャンクのヘッダ文字列を探す
	for (unsigned long cnt = 0; cnt < size_; cnt++){
		// 文字列があっていれば該当アドレスを_chunkに入れてbreak
		if (!_strnicmp(static_cast<const char*>(mappedAddr_) + cnt, _name, RIFF_CHUNK_ID_SIZE)){
			chunk = static_cast<void*>(static_cast<char*>(mappedAddr_) + cnt);
			break;
		}
	}

	return chunk;
}


/*!
 * @brief	メンバのクリア
 */
void Wave::clearMember(){
	size_ = 0;
	waveSize_ = 0;

	offset_ = 0;
	waveOffset_ = 0;
	
	loopCount_ = 0;
	loopBegin_ = 1; // サンプル番号は1開始
	loopLength_ = 0;

	memset(&fmt_, 0, sizeof(fmt_));
}

}
