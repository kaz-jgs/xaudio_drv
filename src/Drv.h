/*!
 *=================================================================================
 * @file	Drv.h
 * @author	kawatanik
 * @date	20141216
 * @brief	XAudio2練習用サウンドドライバの本体.
 *=================================================================================
 */
#pragma once

namespace xaudio_drv{
	bool initialize();
	bool finalize();

	void sample_play(const char* _fileName);
}
