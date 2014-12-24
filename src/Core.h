/*!
 *=================================================================================
 * @file	Core.h
 * @author	kawatanik
 * @date	20141216
 * @brief	XAudio2練習用サウンドドライバのコア部分.
 *=================================================================================
 */
#pragma once

//! XAudio2インターフェイスの前方宣言
struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace xaudio_drv{
//! シングルトンインスタンス型の宣言
typedef class Core* const& CoreInstanceRef;

//! オブジェクトの前方宣言
class Master;
class Bus;
class Snd;
class Wave;


//! サウンドドライバコアクラス(シングルトン)
class Core final{
//! コンストラクタとデストラクタ
private:
	Core();
	~Core();

//! シングルトンインターフェイス
public:
	static void				createInstance();
	static void				deleteInstance();
	static CoreInstanceRef	getInstanceRef();

//! アクセス関数
public:
	bool			initialize();
	bool			finalize();
	void			setMasterVolume(float _volume);
	float			getMasterVolume() const;
	Snd*			createSnd(const Wave* _wave, const void* _bufAddr);



//! メンバ変数
protected:
	IXAudio2*				xaudioEngine_;	//!< XAudio2のエンジンインスタンスへのポインタ
	IXAudio2MasteringVoice*	masterTrack_;	//!< マスタリングボイス(マスタートラック)へのポインタ
};
}
