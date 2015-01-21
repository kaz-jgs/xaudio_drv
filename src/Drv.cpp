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
#include "Bus.h"

#include "util/macros_debug.h"

namespace xaudio_drv{

//! 定数宣言
static unsigned long		THREAD_STACK_SIZE = 0x400;


//! スレッド関数のプロトタイプ宣言
static int threadFunc(std::future<bool>& _initWait);

//! コアのシングルトンインスタンスへの参照
static CoreInstanceRef	core_ = Core::getInstanceRef();

//! 内部変数
static bool					isInitialized_ = false;	//!< 初期化済みフラグ
static bool					isReqFinalize_ = false;	//!< 終了リクエスト
static std::thread*			thread_ = NULL;			//!< スレッドオブジェクト
static std::promise<bool>	initPromiss_;			//!< 初期化同期用プロミスオブジェクト
static std::vector<Snd*>	sndArray_;				//!< 音源オブジェクト配列
static std::vector<Bus*>	busArray_;				//!< 音源オブジェクト配列
static std::vector<Wave*>	waveArray_;				//!< Waveファイル配列
static std::vector<void*>	sndBufArray_;			//!< 音源バッファ配列


//! インライン関数
static inline Snd* searchSnd(SndHandle _hdl);
static inline Bus* searchBus(SndHandle _hdl);
static inline Bus* searchBus(const wchar_t* _busName);


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
		thread_ = new std::thread(threadFunc, initPromiss_.get_future());

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
	std::vector<Snd*>().swap(sndArray_);

	// バスオブジェクトの破棄
	for (Bus*& targ : busArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}
	std::vector<Bus*>().swap(busArray_);

	// Waveファイルの破棄
	for (Wave*& targ : waveArray_){
		if (targ){
			delete targ;
			targ = NULL;
		}
	}
	std::vector<Wave*>().swap(waveArray_);

	// 音源バッファの破棄
	for (void*& targ : sndBufArray_){
		if (targ){
			free(targ);
			targ = NULL;
		}
	}
	std::vector<void*>().swap(sndBufArray_);

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
 * Sndの作成
 */
SndHandle createSnd(const char* _fileName, bool _loadOnMemory/* = true*/, bool _isAutoPlay/* = false*/){
	// Waveファイルを開く
	Wave* wave = new Wave();
	wave->open(_fileName);


	// 転送元アドレスの取得
	const char* addr = static_cast<const char*>(wave->getMappedAddr()) + wave->getWaveOffset();

	// オンメモリロードが指定されている場合
	if (_loadOnMemory){
		// バッファの確保と読み出し
		void* buf = malloc(wave->getWaveSize());
		wave->readBuffer(buf, wave->getWaveSize());

		// バッファを管理用配列に登録
		sndBufArray_.push_back(buf);

		// 転送元アドレスの上書き
		addr = static_cast<const char*>(buf) + wave->getWaveOffset();
	}

	// 音源オブジェクトを作る
	Snd* snd = core_->createSnd(wave, static_cast<const void*>(addr));

	// 管理用配列に登録
	sndArray_.push_back(snd);
	waveArray_.push_back(wave);

	// 再生開始
	if (_isAutoPlay)
		snd->play();

	// ハンドルを返す
	return snd->getHandle();
}



/*!
 * 対象を破棄する
 */
bool destroySnd(SndHandle _handle){
	// 音源オブジェクトの検索
	for (Snd*& targ : sndArray_){
		if (targ);
		else
			continue;

		// 見つかったら削除してtrueを返す
		if (targ->getHandle() == _handle){
			delete targ;
			targ = NULL;

			return true;
		}
	}

	return false;
}


/*!
 * 対象が有効な音源オブジェクトかどうか
 */
bool isAvaliableSnd(SndHandle _handle){
	return searchSnd(_handle) != NULL;
}


/*!
 * 対象を再生する
 */
bool play(SndHandle _handle){
	// 音源オブジェクトの検索
	if (Snd* targ = searchSnd(_handle)){
		//見つかったら再生して結果を返す
		return targ->play();
	}

	return false;
}


/*!
 * 対象を停止する
 */
bool stop(SndHandle _handle, float _fadeTime/* = 0.f*/){
	// 音源オブジェクトの検索
	if (Snd* targ = searchSnd(_handle)){
		//見つかったら 停止して結果を返す
		return targ->stop(_fadeTime);
	}

	return false;
}


/*!
 * 対象の音量を変更する
 */
bool setVolume(SndHandle _handle, float _volume, float _fadeTime/* = 0.f*/){
	// 音源オブジェクトの検索
	TrackBase* targ = searchSnd(_handle);

	// 音源オブジェクトが見つからなければバスを探す
	if (targ);
	else{
		targ = searchBus(_handle);
	}

	// ここでも見つからなければfalseを返す
	if (targ);
	else
		return false;

	// 設定してtrueを返す
	targ->setVolume(_volume, _fadeTime);
	return true;
}


/*!
 * 対象の音量を取得する
 */
float getVolume(SndHandle _handle){
	// 音源オブジェクトの検索
	TrackBase* targ = searchSnd(_handle);

	// 音源オブジェクトが見つからなければバスを探す
	if (targ);
	else{
		targ = searchBus(_handle);
	}

	// ここでも見つからなければ0を返す
	if (targ);
	else{
		AssertMsgBox(false, "[WARNING] getVolume : not found");
		return 0.f;
	}

	// 結果を返す
	return targ->getVolume();
}


/*!
 * 対象が再生中かどうかの取得
 */
bool isPlaying(SndHandle _handle){
	// 音源オブジェクトの検索
	if (Snd* targ = searchSnd(_handle)){
		// 見つかれば再生中かどうかを返す
		return targ->isPlaying();
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
 * バスの追加
 */
SndHandle addBus(const wchar_t* _busName){
	// (デバッグ版のみ 名前かぶり警告)
	AssertMsgBox(searchBus(_busName) == NULL, "[WARNING] bus name is duplicated !");

	Bus* bus = core_->createBus(_busName);
	busArray_.push_back(bus);

	return bus->getHandle();
}


/*!
 * バスの音量設定
 */
bool setBusVolume(const wchar_t* _busName, float _volume, float _fadeTime/* = 0.f*/){
	// バスを探す
	if (Bus* targ = searchBus(_busName)){
		// 見つかったら値を設定してtrueを返す
		targ->setVolume(_volume, _fadeTime);
		return true;
	}

	return false;
}


/*!
 * バスの音量取得
 */
float getBusVolume(const wchar_t* _busName){
	// バスを探す
	if (Bus* targ = searchBus(_busName)){
		// 見つかったら結果を返す
		return targ->getVolume();
	}

	return 0.f;
}


/*!
 * 対象が有効なバスかどうか
 */
bool isAvaliableBus(const wchar_t* _busName){
	return searchBus(_busName) != NULL;
}


/*!
 * 出力バスの設定
 */
bool setOutputBus(SndHandle _src, const wchar_t* _outputBusName){
	// 対象を取得
	TrackBase* targ = searchSnd(_src);

	// 対象を取得できていなければバスから探す
	if (targ);
	else{
		targ = searchBus(_src);
	}

	// ここまでで対象が取得できていなければfalseを返す
	if (targ);
	else{
		AssertMsgBox(false, "[ERROR] SndHandle(_src) is not available");
		return false;
	}

	// 出力バスを取得
	Bus* out_bus = searchBus(_outputBusName);
	if (_outputBusName && out_bus == NULL)
		return false; // バス名が指定されたにもかかわらずNULLの場合はfalseを返す

	// 出力バスを設定して結果を返す
	return targ->setOutputBus(out_bus, 1);
}



/*!
 * スレッド関数
 */
int threadFunc(std::future<bool>& _initWait){
	// 初期化完了待ちとエラーチェック
	if(_initWait.get());
	else
		return -1;

	while(isReqFinalize_ == false){
		// コアの実行処理
		core_->exec();

		// 音源オブジェクトの実行処理
		for(Snd*& targ : sndArray_){
			if(targ)
				targ->exec();
		}

		// バスの実行処理
		for(Bus*& targ : busArray_){
			if(targ)
				targ->exec();
		}
	}

	isReqFinalize_ = false;
	return 0;
}

/*!
 * 音源オブジェクトの検索インライン関数
 */
static inline Snd* searchSnd(SndHandle _hdl){
	for (Snd*& targ : sndArray_){
		// オブジェクトがNULLなら処理をはじく
		if (targ);
		else
			continue;

		// ハンドルが一致すれば対象を返す
		if (targ->getHandle() == _hdl){
			return targ;
		}
	}

	return NULL;
}

/*!
 * バスオブジェクトの検索インライン関数
 */
static inline Bus* searchBus(SndHandle _hdl){
	for (Bus*& targ : busArray_){
		// オブジェクトがNULLなら処理をはじく
		if (targ);
		else
			continue;

		// ハンドルが一致すれば対象を返す
		if (targ->getHandle() == _hdl){
			return targ;
		}
	}

	return NULL;
}

/*!
 * バスオブジェクトの検索インライン関数(文字列版)
 */
static inline Bus* searchBus(const wchar_t* _busName){
	for (Bus*& targ : busArray_){
		// オブジェクトがNULLなら処理をはじく
		if (targ);
		else
			continue;

		// 名前が一致すれば対象を返す
		if (wcscmp(targ->getBusName(), _busName) == 0){
			return targ;
		}
	}

	return NULL;
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