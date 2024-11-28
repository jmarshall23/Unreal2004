#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "qmdx.hpp"
#include "carsound.hpp"

#define MAX_SOUNDS 16
static LPMIXWAVE gSounds[MAX_SOUNDS];
static int gNumSounds = 0;
static HQMIXER gSoundHandle = NULL;

int InitSoundSystem() {
	// init sound
	gSoundHandle = QMDX_Init();
	if(gSoundHandle) {
		QMDX_Activate(gSoundHandle, TRUE);

		QMIX_DISTANCES distances;
		distances.cbSize = sizeof(QMIX_DISTANCES);
		QMDX_GetDistanceMapping(gSoundHandle,0,&distances);
		distances.cbSize = sizeof(QMIX_DISTANCES);
		distances.minDistance = 20.0f;
		distances.maxDistance = 1500.0f;
		distances.scale = 1.0f;
		QMDX_SetDistanceMapping(gSoundHandle,0,QMIX_ALL,&distances);

	} else {
		return -1;
	}

	return 1;
}

int TerminateSoundSystem() {
	if(!gSoundHandle) return -1;

	// destroy all the sounds
	int i;
	for(i = 0; i < gNumSounds; i++) {
		QMDX_FreeWave(gSoundHandle, gSounds[i]);
	}

	QMDX_CloseSession(gSoundHandle);

	return 1;

}

// returns integer handle to sound or -1 on error
int LoadWavFromFile(char* file) {
	if(!gSoundHandle) return -1;
	if(gNumSounds >= MAX_SOUNDS) return -1; // gone over max sounds

	LPMIXWAVE lpWave = QMDX_OpenWave(gSoundHandle, file, 0, WMIX_FILE);

	if(lpWave) {
		gSounds[gNumSounds] = lpWave;
	} else {
		return -1;
	}

	return gNumSounds++;

}

// returns the channel the sound is playing on or -1 on error
int PlayWav(int sound, float vol_scale, float pos[3], float vel[3], int looping) {
	if(!gSoundHandle) return -1;
	if(sound < 0 || sound >= gNumSounds || sound >= MAX_SOUNDS) return -1;

    QSVECTOR position = { pos[0], pos[1], pos[2] };
    QSVECTOR velocity = { vel[0], vel[1], vel[2] };

    int channel = QMDX_OpenChannel(gSoundHandle, 0, WMIX_OPENAVAILABLE);
	int volume = ((int)(vol_scale * 32767.0));
    QMDX_SetVolume(gSoundHandle, channel, 0, volume);
    QMDX_SetSourcePosition(gSoundHandle, channel, 0, &position);
    QMDX_SetSourceVelocity(gSoundHandle, channel, 0, &velocity);

	if(QMDX_PlayEx(gSoundHandle, channel, 0, gSounds[sound], (looping ? -1 : 1), 0)) {
		return -1;
	}

	return channel;

}

// returns current frequency or -1 on error
int SetSoundFrequency(int channel, int sound, float freqscale) {

	if(!gSoundHandle) return -1;
	if(sound < 0 || sound >= gNumSounds || sound >= MAX_SOUNDS) return -1;

	unsigned basefreq = gSounds[sound]->pcm.wf.nSamplesPerSec;
	unsigned newfreq = (unsigned) ( ( (float)basefreq) * freqscale);

	if(newfreq < 1 || newfreq > 165000) {
		newfreq = basefreq;

	}

	QMDX_SetFrequency(gSoundHandle, channel, 0, newfreq);

	return newfreq;
}

int SetSoundPosition(int channel, float pos[3]) {
	if(!gSoundHandle) return -1;

	QSVECTOR position = { pos[0], pos[1], pos[2] };
	QMDX_SetSourcePosition(gSoundHandle, channel, 0, &position);

	return 1;
}

int SetListenerPosition(float pos[3]) {
	if(!gSoundHandle) return -1;

	return 1;
}

int StopSound(int channel) {
	if(!gSoundHandle) return -1;

	QMDX_StopChannel(gSoundHandle,channel,0);
	QMDX_CloseChannel(gSoundHandle, channel, 0);

	return 1;
}