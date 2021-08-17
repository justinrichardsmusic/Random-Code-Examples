#pragma once
#include "olcPixelGameEngine.h"

#include "soloud.h"
#include "soloud_wav.h"

class cAudioListener
{
public:
	// Enum for custom named sound samples
	enum SampleNames
	{
		NULL_SND,
		MUS_RACE_01,
		MUS_MENU,

		SND_ENGINE_01,
	};

	// Struct to keep the Audio Sample data together
	struct sAudioSample
	{
		sAudioSample(int ID, SoLoud::Wav* wavPtr)
		{
			nSampleID = ID;
			wav = wavPtr;
		}

		int nSampleID = NULL_SND;
		SoLoud::Wav* wav;
	};

	// SoLoud Audio Engine Object
	SoLoud::Soloud soloud;

	// Position used for volume calculations
	olc::vf2d vecPos = { 0.0f, 0.0f };

	// Global volume settings
	float fMusicVolume = 0.2f;
	float fEngineVolume = 0.1f;
	float fSoundFXVolume = 0.4f;

	bool bMusicOn = true;
	bool bSoundOn = true;

	// Vector of Audio Samples
	std::vector<sAudioSample> audioSamples;
	std::list<SoLoud::Wav> wavs;



	// Initialise the Audio Engine, and Destroy it when done
	void AudioSystemInit();
	void AudioSystemDestroy();

	// Load a file and store it in the list of wavs
	void LoadAudioSample(int ID, const char* fileName);

	// Identify a particular Audio Sample based on its ID
	sAudioSample* GetAudioSampleByID(int ID);

	// Update the spacial position of the Audio Listener
	void UpdatePosition(olc::vf2d pos);

	// Calculate distance between listener and source
	float GetDistance(olc::vf2d sourcePos, bool returnRoot = true);

};

