/*!
 *=================================================================================
 * @file	Core.cpp
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2練習用サウンドドライバのコア部分.
 *=================================================================================
 */
#include <XAudio2.h>

#include "Core.h"
#include "Wave.h"
#include "Snd.h"
#include "Bus.h"
#include "Master.h"
#include "SndCallback.h"

#include "util/macros_debug.h"

namespace xaudio_drv{

//! シングルトンインスタンス
static Core* s_instance = NULL;


/*!
 * シングルトンインスタンスの生成
 */
void Core::createInstance(){
	if(s_instance)
		return;

	s_instance = new Core();
}

/*!
 * シングルトンインスタンスの取得
 */
CoreInstanceRef Core::getInstanceRef(){
	return s_instance;
}

/*!
 * シングルトンインスタンスの削除
 */
void Core::deleteInstance(){
	if(s_instance);
	else
		return;

	delete s_instance;
	s_instance = NULL;
}

/*!
 * コンストラクタ
 */
Core::Core() :	xaudioEngine_(NULL),
				masterTrack_(NULL)
{
}



/*!
 * デストラクタ
 */
Core::~Core(){
	if(xaudioEngine_)
		finalize();
}



/*!
 * 初期化処理
 */
bool Core::initialize(){
	// 過初期化チェック
	if(xaudioEngine_)
		return false;

	// エンジンの初期化
	{
		// フラグ(デバッグ時はデバッグ用フラグを立てるため
		unsigned int flags = 0;
#ifdef _DEBUG
		flags |= XAUDIO2_DEBUG_ENGINE;
#endif

		// エンジンの初期化
		if (XAudio2Create(&xaudioEngine_, flags) == S_OK);
		else{
			return false;
		}
	}

	// マスタートラックの生成
	{
		// XAudio2マスタリングボイスインスタンスの生成
		IXAudio2MasteringVoice* mastering_voice = NULL;
		xaudioEngine_->CreateMasteringVoice(&mastering_voice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL);
		if (mastering_voice);
		else{
			AssertMsgBox(false, "Core : creating mastering voice is failed.");
			return false;
		}

		// マスタートラックオブジェクトインスタンスの生成
		masterTrack_ = new Master(mastering_voice);
	}

	return true;
}



/*!
 * 終了処理
 */
bool Core::finalize(){
	// 過解放チェック
	if(xaudioEngine_);
	else
		return false;

	// マスタートラックの開放
	if(masterTrack_){
		delete masterTrack_;
		masterTrack_ = NULL;
	}

	// XAudioエンジンの開放
	xaudioEngine_->Release();
	xaudioEngine_ = NULL;

	return true;
}


/*!
 * 実行処理
 */
bool Core::exec(){
	return masterTrack_->exec();
}


/*!
 * 音源オブジェクトの生成
 */
Snd* Core::createSnd(const Wave* _wave, const void* _bufAddr){
	// コールバックオブジェクトの生成
	SndCallback* callback = new SndCallback();

	// 音源ボイスのインスタンス生成
	IXAudio2SourceVoice* voice = NULL;
	xaudioEngine_->CreateSourceVoice(&voice, _wave->getWaveFormatEx(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, callback, NULL);


	// XAudio2バッファ構造体を作る
	XAUDIO2_BUFFER* buffer = new XAUDIO2_BUFFER();
	{
		buffer->pAudioData = static_cast<const BYTE*>(_bufAddr);
		buffer->Flags = XAUDIO2_END_OF_STREAM;
		buffer->AudioBytes = _wave->getWaveSize();
		buffer->LoopBegin = _wave->getLoopBegin();
		buffer->LoopLength = _wave->getLoopLength();
		buffer->LoopCount = _wave->getLoopCount();
	}

	// 音源オブジェクトを作成してインスタンスを返す
	return new Snd(voice, callback, buffer);
}


/*!
 * 出力バスの生成
 */
Bus* Core::createBus(const wchar_t* _busName /* = NULL */){
	// サブミクスボイスのインスタンス生成
	IXAudio2SubmixVoice* voice = NULL;
	xaudioEngine_->CreateSubmixVoice(&voice, 2, 48000);

	// 出力バスインスタンスを生成して返す
	return new Bus(voice, _busName);
}


/*!
 * マスターボリュームの取得
 */
float Core::getMasterVolume() const{
	// 初期化チェック
	if(xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return 0.f;
	}

	// マスターからボリュームを取得する
	return masterTrack_->getVolume();
}

/*!
 * マスターボリュームのセット
 */
void Core::setMasterVolume(float _volume, float _fadeTime/* = 0.f*/){
	// 初期化チェック
	if (xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return;
	}

	masterTrack_->setVolume(_volume, _fadeTime);
}
}
