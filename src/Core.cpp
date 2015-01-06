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

	// マスタリングボイス(マスタートラック)の生成
	xaudioEngine_->CreateMasteringVoice(&masterTrack_, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL);
	if(masterTrack_);
	else{
		AssertMsgBox(false, "Core : creating mastering voice is failed.");
		return false;
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

	if(masterTrack_){
		masterTrack_->DestroyVoice();
		masterTrack_ = NULL;
	}

	xaudioEngine_->Release();
	xaudioEngine_ = NULL;

	return true;
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
	XAUDIO2_BUFFER buffer = { 0 };
	{
		buffer.pAudioData = static_cast<const BYTE*>(_bufAddr);
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.AudioBytes = _wave->getWaveSize();
		buffer.LoopBegin = _wave->getLoopBegin();
		buffer.LoopLength = _wave->getLoopLength();
		buffer.LoopCount = _wave->getLoopCount();
	}

	// バッファを転送する
	voice->SubmitSourceBuffer(&buffer);

	// 音源オブジェクトを作成してインスタンスを返す
	return new Snd(voice, callback);
}


/*!
 * 出力バスの生成
 */
Bus* Core::createBus(){
	// サブミクスボイスのインスタンス生成
	IXAudio2SubmixVoice* voice = NULL;
	xaudioEngine_->CreateSubmixVoice(&voice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE);

	// 出力バスインスタンスを生成して返す
	return new Bus(voice);
}


/*!
 * マスターボリュームの取得
 */
float Core::getMasterVolume() const{
	// 戻り値用変数
	float ret = 0.f;

	// 初期化チェック
	if(xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return ret;
	}

	// マスターからボリュームを取得する
	masterTrack_->GetVolume(&ret);

	return ret;
}

/*!
 * マスターボリュームのセット
 */
void Core::setMasterVolume(float _volume){
	// 初期化チェック
	if (xaudioEngine_ && masterTrack_);
	else{
		AssertMsgBox(false, "Core : not initialized of invalid status.");
		return;
	}

	masterTrack_->SetVolume(_volume);
}
}
