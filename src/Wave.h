/*!
 *=================================================================================
 * @file	Wave.h
 * @author	kawatanik
 * @date	20141218
 * @brief	XAudio2練習用サウンドドライバ Waveファイルクラス.
 *
 * このファイルのinclude前にWindows.hがincludeされている必要があります.
 * 4GB以上のファイルを食わすと飛びます.
 *=================================================================================
 */
#pragma once

namespace xaudio_drv{
class Wave{
//! コンストラクタとデストラクタ
public:
	Wave();
	virtual ~Wave(){ close(); }

//! アクセス関数
public:
	bool			open(const char* _fileName);										//!< ファイルを開く(ANSI版)
	bool			open(const wchar_t* _fileName);										//!< ファイルを開く(UNICODE版)
	void			close();															//!< ファイルを閉じる

	bool			seek(unsigned long _offset);										//!< ファイルのシーク
	bool			seekWave();															//!< Waveデータ本体先頭へのシーク

	unsigned long	getFileSize() const { return size_; }								//!< ファイルサイズの取得
	unsigned long	getWaveSize() const { return waveSize_; }							//!< Waveデータ本体のサイズの取得

	unsigned long	readBuffer(void* _outBuffer, unsigned long _size = 0xffffffff);		//!< バッファの読み込み
	const void*		getMappedAddr() const{ return mappedAddr_; }						//!< ファイルマッピングアドレスの取得
	unsigned long	getWaveOffset() const{ return waveOffset_; }						//!< Waveデータ本体先頭オフセットの取得

	//!< ループ情報の取得
	unsigned long	getLoopCount() const{ return loopCount_; }
	unsigned long	getLoopBegin() const{ return loopBegin_; }
	unsigned long	getLoopLength() const{ return loopLength_; }

	//!< Waveフォーマットの取得
	const WAVEFORMATEX*	getWaveFormatEx() const{ return &fmt_; }

//! メンバ関数
protected:
	bool			mapping();															//!< マッピング
	bool			parse();															//!< パース
	inline bool		prepare();															//!< ファイルオープン後のマッピングとパースの処理まとめ
	inline void*	searchChunk(const char* _name) const;								//!< チャンクの検索
	inline void		clearMember();														//!< メンバのクリア 


//! メンバ変数
protected:
	HANDLE			fileHandle_;														//!< ファイルハンドル

	HANDLE			mapHandle_;															//!< ファイルマッピングハンドル
	void*			mappedAddr_;														//!< ファイルマッピングアドレス

	unsigned long	size_;																//!< ファイルサイズ
	unsigned long	offset_;															//!< 読み込みバッファのオフセット

	unsigned long	waveOffset_;														//!< Waveデータ本体へのオフセット
	unsigned long	waveSize_;															//!< Waveデータ本体のサイズ

	unsigned long	loopCount_;															//!< ループカウント
	unsigned long	loopBegin_;															//!< ループ開始サンプル数
	unsigned long	loopLength_;														//!< ループの長さ

	WAVEFORMATEX	fmt_;																//!< Waveフォーマット構造体
};

}
