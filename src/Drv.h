/*!
 *=================================================================================
 * @file	Drv.h
 * @author	kaz-jgs
 * @date	20141216
 * @brief	XAudio2���K�p�T�E���h�h���C�o�̖{��.
 *=================================================================================
 */
#pragma once

namespace xaudio_drv{
	bool initialize();
	bool finalize();

	void sample_play(const char* _fileName);
}
