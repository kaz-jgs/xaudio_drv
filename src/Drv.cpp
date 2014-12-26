/*!
 *=================================================================================
 * @file	Drv.cpp
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2練習用サウンドドライバの本体.
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

//! 定数宣言
static unsigned long	THREAD_STACK_SIZE = 0x400;

//! スレッド関数のプロトタイプ宣言
static int threadFunc(std::future<bool>* _initWait);

//! コアのシングルトンインスタンスへの参照
static CoreInstanceRef	core_ = Core::getInstanceRef();

//! 内部変数
static bool					isInitialized_ = false;	//!< 初期化済みフラグ
static bool					isReqFinalize_ = false;	//!< 終了リクエスト
static std::thread*			thread_ = NULL;			//!< スレッドオブジェクト
static std::promise<bool>	initPromiss_;			//!< 初期化同期用プロミスオブジェクト
static std::vector<Snd*>	sndArray_;				//!< 音源オブジェクト配列
static std::vector<Wave*>	waveArray_;				//!< Waveファイル配列
static std::vector<void*>	sndBufArray_;			//!< 音源バッファ配列


/*!
 * 初期化
 */
bool initialize(){
	// 過初期化防止
	if(isInitialized_)
		return false;

	// コアのシングルトンインスタンスを生成
	Core::createInstance();
	AssertMsgBox(core_, "xaudio_drv::initialize() : couldn't create core instance");

	// コアの初期化
	if(core_->initialize());
	else{
		AssertMsgBox(false, "xaudio_drv::initialize() : couldn't initialize core.");
		return false;
	}

	// サウンドスレッドの生成
	{
		// スレッドオブジェクトと同期オブジェクトの生成
		thread_ = new std::thread(threadFunc, &initPromiss_.get_future());

		// @todo スレッドの詳細設定

		// サウンドスレッドの開始許可
		initPromiss_.set_value(true);
	}

	// 初期化済みフラグを立ててtrueを返す
	isInitialized_ = true;
	return true;
}


/*!
 * 終了
 */
bool finalize(){
	// 過解放防止
	if(isInitialized_);
	else
		return false;

	// スレッドの終了
	if(thread_){
		// サウンドスレッドの終了リクエスト
		isReqFinalize_ = true;

		// サウンドスレッドの終了待ち
		thread_->join();

		// スレッドオブジェクトの破棄
		delete thread_;
		thread_ = NULL;
	}

	// 音源オブジェクトの破棄
	for (Snd*& targ : sndArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}

	// Waveファイルの破棄
	for (Wave*& targ : waveArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}

	// 音源バッファの破棄
	for (void*& targ : sndBufArray_){
		if (targ){
			free(targ);
			targ = NULL;
		}
	}

	// コアの終了
	if(Core::getInstanceRef()->finalize());
	else{
		AssertMsgBox(false, "xaudio_drv::finalize() : couldn't finalize core.");
		return false;
	}

	// コアのインスタンス破棄
	Core::deleteInstance();

	// 初期化済みフラグを落としてtrueを返す
	isInitialized_ = false;
	return true;
}


/*!
 * 対象が再生中かどうかの取得
 */
bool isPlaying(SndHandle _handle){
	// 音源オブジェクト配列の走査
	for (Snd*& targ : sndArray_){
		// オブジェクトがNULLなら処理をはじく
		if (targ);
		else
			continue;

		// オブジェクトのハンドルが一致すれば再生中かどうかを返す
		if (targ->getHandle() == _handle){
			return targ->isPlaying();
		}
	}

	return false;
}


/*!
 * 何かが再生中かどうかの取得
 */
bool isPlayingAny(){
	// 音源オブジェクト配列の走査
	for (Snd*& targ : sndArray_){
		// オブジェクトがNULLなら処理をはじく
		if (targ);
		else
			continue;

		// 再生中のものがあればtrueを返す
		if (targ->isPlaying()){
			return true;
		}
	}

	return false;
}



/*!
 * サンプル用投げっぱなし再生関数
 */
SndHandle sample_play(const char* _fileName){
	// Waveファイルを開く
	Wave* wave = new Wave();
	wave->open(_fileName);

	// バッファの確保と波形データの読みだし
	void* buf = malloc(wave->getWaveSize());
	wave->readBuffer(buf, wave->getWaveSize());

	// 音源オブジェクトを作る
	Snd* snd = core_->createSnd(wave, static_cast<const void*>(static_cast<const char*>(wave->getMappedAddr()) + wave->getWaveOffset()));


	// 再生開始
	snd->play();

	// 管理用に登録
	sndArray_.push_back(snd);
	waveArray_.push_back(wave);
	sndBufArray_.push_back(buf);

	// ハンドルを返す
	return snd->getHandle();
}



/*!
 * スレッド関数
 */
int threadFunc(std::future<bool>* _initWait){
	// 初期化完了待ちとエラーチェック
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


//----- 以下Cインターフェイスの実装
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