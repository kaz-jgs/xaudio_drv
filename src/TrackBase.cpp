/*!
 *=================================================================================
 * @file	TrackBase.h
 * @author	kaz-jgs
 * @date	20141217
 * @brief	XAudio2練習用サウンドドライバ トラックオブジェクト用基底クラス.
 *=================================================================================
 */
#include "TrackBase.h"
#include <XAudio2.h>
#include <time.h>

#include "util/macros_debug.h"

namespace xaudio_drv{
//! 定数宣言
static const int TICKS_PER_SECOND = 1000;	// 秒あたりのticks数(Windowsはmsec精度)

/*!
 * @brief	ハンドル生成
 *
 * インスタンスアドレスの下位32bitと32bitの種をバイト単位で並べ替えた8バイト.
 * 種は0xffffffffスタートでインスタンス生成毎にデクリメント.
 * 並べ替え規則に特に意味はなく
 * 「(種がオーバーフローしなければ)被らない」
 * 「外部で容易に数値を加工して生成できない」
 * を満たせるようにと.
 */
unsigned long TrackBase::handleSeed__ = 0xffffffff;
#define HANDLE_CREATE(_seed)														\
	static_cast<SndHandle>(															\
		((static_cast<unsigned __int64>(_seed)		& 0x0000000000ff0000) >> 16) |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x00000000ff000000) >> 16) |	\
		((static_cast<unsigned __int64>(_seed)		& 0x00000000ff000000) >> 8)  |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x000000000000ff00) << 16) |	\
		((static_cast<unsigned __int64>(_seed)		& 0x00000000000000ff) << 40) |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x00000000000000ff) << 40) |	\
		((static_cast<unsigned __int64>(_seed)		& 0x000000000000ff00) << 40) |	\
		((reinterpret_cast<unsigned __int64>(this)	& 0x0000000000ff0000) << 40)	\
	)

/*!
 * @brief		コンストラクタ.
 * @param[in]	_voice	子クラスのXAudioボイスインターフェイスへの参照
 */
TrackBase::TrackBase(IXAudio2Voice* const& _voice) :
refVoiceBase_(_voice), 
handle_(HANDLE_CREATE(handleSeed__--)), 
lastTicks_(GetTickCount())
{
	// ハンドルの種のオーバーフロー(普通に使ってたらしないと思う)対策
	if (handleSeed__ > 0);
	else{
		AssertMsgBox(false, "[TrackBase] warning : handle counter has overflowed.");
		handleSeed__ = 0xffffffff;
	}
}


/*!
 * @brief		Ticksの更新.
 * @retval		Ticks更新に成功したかどうか.
 *
 * lastTicks_を更新します.
 * 前回更新時からDELTA_TICKS_THRESHOLD_以上のTicksが経過していれば更新
 * そうでなければ更新しない処理になっています.
 * 子クラスのexec関数先頭で呼び出してfalseが返ったら処理を打ち切ることで
 * サウンドスレッドの過負荷防止に使います.
 */
bool TrackBase::updateTicks(){
	// 前回からのデルタを取得
	unsigned long delta_ticks = GetTickCount() - lastTicks_;

	// デルタタイムが足りてない or オーバーフローが起きていれば更新せずにfalseを返す
	if (delta_ticks < DELTA_TICKS_THRESHOLD_ || static_cast<signed>(delta_ticks) < 0){
		return false;
	}

	// ticks更新してtrueを返す
	lastTicks_ += delta_ticks;
	return true;
}


/*!
 * @brief		音量更新.
 * @retval		音量が変更されたかどうか.
 *
 * 線形更新のみです.
 * 子クラスのexec関数内で毎回呼ばれる想定です.
 */
bool TrackBase::updateVolume(){
	// ボイスが解放済みの場合はfalseを返す
	if(refVoiceBase_);
	else{
		AssertMsgBox(false, "voice instance is not exist.");
		return false;
	}

	// 音量更新がなければfalseを返す
	if(volInfo_.vol == volInfo_.target || volInfo_.delta == 0.f){
		return false;
	}

	// デルタticksの取得とticksの更新
	unsigned long delta_ticks = GetTickCount() - volInfo_.lastTicks;
	volInfo_.lastTicks += delta_ticks;

	// デルタタイムがオーバーフローしてた場合(Windows起動から約50日経過ごとに発生)の処理
	// @note	ここでの検出条件は「unsignedをsignedにキャストした際に負数になるほどdeltaが長かった場合」とします.
	//			tickを現在値に指定して成功扱いで処理を打ち切るのでフェード時間が指定値より若干伸びます
	if(static_cast<signed>(delta_ticks) < 0){
		volInfo_.lastTicks = GetTickCount();
		return true;
	}

	// ボリューム情報の更新
	volInfo_.vol += (volInfo_.delta * delta_ticks);

	// リミットチェック
	if (	(volInfo_.vol > volInfo_.target && volInfo_.delta > 0) ||	// デルタが正数で適用ボリュームがターゲットを上回った場合
			(volInfo_.vol < volInfo_.target && volInfo_.delta < 0) )	// デルタが負数で適用ボリュームがターゲットを下回った場合
	{
		volInfo_.vol = volInfo_.target;
		volInfo_.delta = 0.f;
	}

	// ボリューム変更を反映
	refVoiceBase_->SetVolume(volInfo_.vol);

	// trueを返す
	return true;
}


/*!
 * @brief		ボリュームの設定
 * @param[in]	_targetVol	設定するボリュームの目標値(1.0 = 0dB)
 * @param[in]	_fadeTime	フェード時間[sec]
 */
void TrackBase::setVolume(float _targetVol, float _fadeTime /* = 0.f */){
	// フェードタイムが0の場合は直接設定
	if (_fadeTime <= 0.f){
		volInfo_.vol = volInfo_.target = _targetVol;
		volInfo_.delta = 0.f;
		refVoiceBase_->SetVolume(_targetVol);
	}
	// フェードタイムに依ってdeltaの計算
	else{
		volInfo_.target = _targetVol;
		refVoiceBase_->GetVolume(&volInfo_.vol);
		volInfo_.delta = (volInfo_.target - volInfo_.vol) * (1.f / (_fadeTime * TICKS_PER_SECOND));
		volInfo_.lastTicks = GetTickCount();
	}
}
}
