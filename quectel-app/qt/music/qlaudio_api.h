#ifndef QL_Audio_API_H
#define QL_Audio_API_H

typedef enum AudioOutputDevice
{
	DefaultOutputDevice = 0,
	Speaker = 2,
	Headphone = 4,
}OutputDevice;

typedef enum AudioInputDevice
{
	DefaultInputDevice = 0,
	Builtmic = 4,
	HeadMic = 18,
}InputDevice;

typedef enum AudioChannel
{
	DefaultChannel = 0,
	Mono = 1,
	Stereo = 2,

}Channel;

typedef enum AudioFileType
{
	Wav = 1,
	Mp3 = 2,

}FileType;

typedef enum Samp_rate
{
	DefualtPlayRate = 0,
	DefaultRecordRate = 0,
	rate_44100 = 44100,
	rate_48000 = 48000,

}Rate;

struct Audio_Playback
{
	OutputDevice device;
	Channel channel;
	FileType      filetype;
	Rate          rate;
	int volume;
};

struct Audio_Record
{
	InputDevice device;
	Channel channel;
	Rate      rate;

};

void QL_Record_Stop();
void QL_Audio_stopback();
void QL_Audio_startback();
void QL_Audio_Record(struct Audio_Record record,char *file);
void QL_Audio_Playback(struct Audio_Playback playback, char *file);
#endif
