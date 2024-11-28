extern int InitSoundSystem();
extern int TerminateSoundSystem();
extern int LoadWavFromFile(char* file);
extern int PlayWav(int sound, float vol, float pos[3], float vel[3], int looping);
extern int SetSoundFrequency(int channel, int sound, float freqscale);
extern int SetSoundPosition(int channel, float pos[3]);
extern int SetListenerPosition(float pos[3]);
extern int StopSound(int channel);