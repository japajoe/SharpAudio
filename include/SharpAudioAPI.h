#ifndef SHARPAUDIOAPI_HPP
#define SHARPAUDIOAPI_HPP

#include "Runtime/DotNet.h"
#include "RtMidi.h"
#include <memory>
#include <vector>
#include <functional>

typedef std::function<void(const std::string message)> DebugEvent;

typedef void (*debug_callback_ptr)(char* message);
typedef void (*initialize_callback_ptr)();
typedef void (*initialize_ptr)(debug_callback_ptr callback);
typedef void (*stop_application_ptr)();
typedef bool (*compile_ptr)(const char* source);
typedef void (*stop_script_ptr)();
typedef int (*get_field_count_ptr)();
typedef int (*get_field_type_ptr)(int fieldIndex);
typedef char* (*get_field_name_ptr)(int fieldIndex);
typedef char* (*get_field_value_ptr)(int fieldIndex);
typedef void (*set_field_value_ptr)(int fieldIndex, const char* fieldValue);
typedef double (*get_field_min_value_ptr)(int fieldIndex);
typedef double (*get_field_max_value_ptr)(int fieldIndex);
typedef float (*get_sample_at_offset_ptr)(int audiosourceIndex, int sampleIndex);
typedef float (*get_sample_left_ptr)(int audiosourceIndex);
typedef float (*get_sample_right_ptr)(int audiosourceIndex);
typedef int (*get_audio_source_count_ptr)();
typedef long (*get_playback_position_ptr)(int audiosourceIndex);
typedef int (*get_data_length_ptr)(int audiosourceIndex);
typedef bool (*is_audio_source_playing_ptr)(int audiosourceIndex);
typedef void (*set_midi_key_state_down_ptr)(int keyIndex, int velocity);
typedef void (*set_midi_key_state_up_ptr)(int keyIndex, int velocity);
typedef void (*set_midi_pitchbend_state)(int velocity);
typedef void (*set_midi_modwheel_state)(int velocity);
typedef void (*set_midi_volumeslider_state)(int velocity);

class SharpAudioAPI
{
public:
    static DebugEvent onDebug;
    static bool Initialize(char* argv);
    static bool InitializeMidi();
    static void StopApplication();
    static bool Compile(const std::string& source);
    static void StopScript();
    static int GetFieldCount();
    static int GetFieldType(int fieldIndex);
    static bool GetFieldName(int fieldIndex, std::string& name);
    static bool GetFieldValue(int fieldIndex, std::string& value);
    static void SetFieldValue(int fieldIndex, const std::string& value);
    static double GetFieldMinValue(int fieldIndex);
    static double GetFieldMaxValue(int fieldIndex);
    static float GetSampleAtOffset(int audiosourceIndex, int sampleIndex);
    static float GetSampleLeft(int audiosourceIndex);
    static float GetSampleRight(int audiosourceIndex);
    static int GetAudioSourceCount();
    static long GetPlaybackPosition(int audiosourceIndex);
    static int GetDataLength(int audiosourceIndex);
    static bool IsAudioSourcePlaying(int audiosourceIndex);
    static void SetMidiKeyStateDown(int keyIndex, int velocity);
    static void SetMidiKeyStateUp(int keyIndex, int velocity);
    static void SetMidiPitchBendState(int velocity);
    static void SetMidiModWheelState(int velocity);
    static void SetMidiVolumeSliderState(int velocity);
private:
    static DotNet dotnet;
    static std::unique_ptr<RtMidiIn> midiIn;
    static std::vector<int> midiKeyInfo;    
    static void OnMidiRead(double deltatime, std::vector< unsigned char > *message, void *userData);    
    static void LogMessage(char* message);
    static initialize_ptr initialize;
    static compile_ptr compile;
    static stop_script_ptr stopScript;
    static stop_application_ptr stopApplication;
    static get_field_count_ptr getFieldCount;
    static get_field_type_ptr getFieldType;
    static get_field_name_ptr getFieldName;
    static get_field_value_ptr getFieldValue;
    static set_field_value_ptr setFieldValue;
    static get_field_min_value_ptr getFieldMinValue;
    static get_field_max_value_ptr getFieldMaxValue;
    static get_sample_at_offset_ptr getSampleAtOffset;
    static get_sample_left_ptr getSampleLeft;
    static get_sample_right_ptr getSampleRight;
    static get_audio_source_count_ptr getAudioSourceCount;
    static get_playback_position_ptr getPlaybackPosition;
    static get_data_length_ptr getDataLength;
    static is_audio_source_playing_ptr isAudioSourcePlaying;
    static set_midi_key_state_down_ptr setMidiKeyStateDown;
    static set_midi_key_state_up_ptr setMidiKeyStateUp;
    static set_midi_pitchbend_state setMidiPitchBendState;
    static set_midi_modwheel_state setMidiModWheelState;
    static set_midi_volumeslider_state setMidiVolumeSliderState;
};
#endif