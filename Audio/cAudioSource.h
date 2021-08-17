#pragma once
#include "olcPixelGameEngine.h"
#include "cAudioListener.h"

class cAudioSource
{
public:
	// Pointer to the Audio Listener for this object
	cAudioListener* AL;

	// Handle for this particular copy of the sound
	int handle = 255;

	// Maximum copies of this sound allowed for this audio source
	//const int nMaxSamples = 4;
	//int nSampleHandles[4] = { 255, 255, 255, 255 };

	// Audio Sample ID used to locate which sound to play
	int nID;

	// Convenient BOOL to determine playback status
	bool bIsPlaying = false;

	// Current Playback Speed
	float fPlaySpeed = 1.0f;

	// Volume
	float fVolume = 1.0f;
	float fMinVolume = 0.0f;
	float fMaxVolume = 1.0f;

	// Does the Audio Sample Loop?
	bool bLooping = false;

	// Paused status
	bool bPaused = false;

	// Object position, used for calculating volume
	olc::vf2d pos = { 0.0f, 0.0f };



	// Instruct Audio Listener to load this sound (if not loaded already)
	void LoadAudioSample(int ID, const char* fileName);

	// Play the Audio Sample, with given parameters
	void Play(float speed = 1.0f, float vol = 1.0f, bool looping = false, bool paused = false);

	// Pause or Un-Pause - maintains the playback position and handle
	void Pause(bool pauseState = true);

	// Stop - playback position and handle will be lost
	void Stop();

	// Audio Modulation - control the speed of playback
	void ModulateAudio(float minPlaySpeed, float maxPlaySpeed, float modulation, bool precise = false, bool deferred = false);

	// Adjust Volume
	void SetVolume(float vol, float minVol = 0.0f, float maxVol = 1.0f);

	// Set Default Parameters
	void SetDefaults(float speed, float vol, float minVol, float maxVol, bool looping);
};

