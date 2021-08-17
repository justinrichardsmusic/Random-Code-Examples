#include "cAudioSource.h"

void cAudioSource::LoadAudioSample(int ID, const char* fileName)
{
	// Link the IDs together
	nID = ID;

	// Call the Audio Listener to load the sample
	AL->LoadAudioSample(ID, fileName);
}

void cAudioSource::Play(float speed, float vol, bool looping, bool paused)
{
	// Set parameters
	fPlaySpeed = speed;
	fVolume = vol;
	bLooping = looping;
	bPaused = paused;

	// Check to see if this sound is already being played at its maximum allowed number
	// of samples

	//if (AL->soloud.isValidVoiceHandle(handle)) AL->soloud.stop(handle);

	// Assign a handle to this instance of the sound we are about to play
	handle = AL->soloud.play(*AL->GetAudioSampleByID(nID)->wav, fVolume, 1.0f, bPaused);

	// Set speed and looping 
	AL->soloud.setRelativePlaySpeed(handle, fPlaySpeed);
	AL->soloud.setLooping(handle, looping);

	// Update Play status
	bIsPlaying = true;
}

void cAudioSource::Pause(bool pauseState)
{
	// Use the Audio Listener to pause or un-pause the sound as neccessary
	AL->soloud.setPause(handle, pauseState);

	// Update Play status
	bIsPlaying = !pauseState;
}

void cAudioSource::Stop()
{
	// Use the Audio Listener to stop the sound
	AL->soloud.stop(handle);

	// The current handle will now point to nothing, so we set it to MAX so we can test for validity if need be
	handle = 255;

	// Update Play status
	bIsPlaying = false;
}

void cAudioSource::ModulateAudio(float minPlaySpeed, float maxPlaySpeed, float modulation, bool precise, bool deferred)
{
	// Apply the modulation
	if (precise)
		fPlaySpeed = modulation;
	else
		fPlaySpeed += modulation;

	// Adjust the play speed to keep it within range
	if (fPlaySpeed < minPlaySpeed) fPlaySpeed = minPlaySpeed;
	else if (fPlaySpeed > maxPlaySpeed) fPlaySpeed = maxPlaySpeed;

	// If multiple adjustments to the playback speed are required, then the Audio Listener update itself can be
	// deferred until the very last adjustment is made... A small optimisation
	if (!deferred)
		AL->soloud.setRelativePlaySpeed(handle, fPlaySpeed);
}

void cAudioSource::SetVolume(float vol, float minVol, float maxVol)
{
	// Set volume
	fVolume = vol;

	// Clamp volume withing set bounds
	if (fVolume < minVol)
		fVolume = minVol;
	else if (fVolume > maxVol)
		fVolume = maxVol;

	// Instruct the Audio Listener to apply the volume change
	AL->soloud.setVolume(handle, fVolume);
}

void cAudioSource::SetDefaults(float speed, float vol, float minVol, float maxVol, bool looping)
{
	// Set defaults for this particular Audio Source
	fPlaySpeed = speed;
	fVolume = vol;
	fMinVolume = minVol;
	fMaxVolume = maxVol;
	bLooping = looping;
}



