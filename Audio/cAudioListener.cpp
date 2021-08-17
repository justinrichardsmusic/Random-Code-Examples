#include "cAudioListener.h"

void cAudioListener::AudioSystemInit()
{
	// Initialise the SoLoud backend
	soloud.init();
}

void cAudioListener::AudioSystemDestroy()
{
	// Clean up the SoLoud engine
	soloud.deinit();
}

void cAudioListener::LoadAudioSample(int ID, const char* fileName)
{
	// Search for any matching IDs
	for (auto& a : audioSamples)
		if (a.nSampleID == ID) return; // Audio Sample is already loaded, no need to load the same file twice!

	// Add a new wav to the list of wavs and get a pointer to it
	wavs.push_back(SoLoud::Wav());
	SoLoud::Wav* wavPtr = &wavs.back();

	// Use the pointer to load the file into the back of the wav list
	wavPtr->load(fileName);

	// Create a new Audio sample object in the vector of samples that links both the ID and wav file itself, for convenience
	audioSamples.emplace_back(sAudioSample ( ID, wavPtr ));
}

cAudioListener::sAudioSample* cAudioListener::GetAudioSampleByID(int ID)
{
	// Look for matching IDs
	for (auto& a : audioSamples)
		if (ID == a.nSampleID) return &a; // Match found! Return it...

	// No match found, need to return a reference so we return the first sample in the list
	return &audioSamples[0];
}

void cAudioListener::UpdatePosition(olc::vf2d pos)
{
	vecPos = pos;
}

float cAudioListener::GetDistance(olc::vf2d sourcePos, bool returnRoot)
{
	if (returnRoot)
		return sqrtf(abs(sourcePos.x * sourcePos.x - vecPos.x * vecPos.x) + abs(sourcePos.y * sourcePos.y - vecPos.y * vecPos.y));
	else
		return abs(sourcePos.x * sourcePos.x - vecPos.x * vecPos.x) + abs(sourcePos.y * sourcePos.y - vecPos.y * vecPos.y);

}


