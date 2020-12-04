#include "SharpAudioAPI.h"
#include <iostream>

DotNet SharpAudioAPI::dotnet;
DebugEvent SharpAudioAPI::onDebug = nullptr;
std::unique_ptr<RtMidiIn> SharpAudioAPI::midiIn;
std::vector<int> SharpAudioAPI::midiKeyInfo;
initialize_ptr SharpAudioAPI::initialize = nullptr;
compile_ptr SharpAudioAPI::compile = nullptr;
stop_script_ptr SharpAudioAPI::stopScript = nullptr;
stop_application_ptr SharpAudioAPI::stopApplication = nullptr;
get_field_count_ptr SharpAudioAPI::getFieldCount = nullptr;
get_field_type_ptr SharpAudioAPI::getFieldType = nullptr;
get_field_name_ptr SharpAudioAPI::getFieldName = nullptr;
get_field_value_ptr SharpAudioAPI::getFieldValue = nullptr;
set_field_value_ptr SharpAudioAPI::setFieldValue = nullptr;
get_field_min_value_ptr SharpAudioAPI::getFieldMinValue = nullptr;
get_field_max_value_ptr SharpAudioAPI::getFieldMaxValue = nullptr;
get_sample_at_offset_ptr SharpAudioAPI::getSampleAtOffset = nullptr;
get_sample_left_ptr SharpAudioAPI::getSampleLeft = nullptr;
get_sample_right_ptr SharpAudioAPI::getSampleRight = nullptr;
get_audio_source_count_ptr SharpAudioAPI::getAudioSourceCount = nullptr;
get_playback_position_ptr SharpAudioAPI::getPlaybackPosition = nullptr;
get_data_length_ptr SharpAudioAPI::getDataLength = nullptr;
is_audio_source_playing_ptr SharpAudioAPI::isAudioSourcePlaying = nullptr;
set_midi_key_state_down_ptr SharpAudioAPI::setMidiKeyStateDown = nullptr;
set_midi_key_state_up_ptr SharpAudioAPI::setMidiKeyStateUp = nullptr;
set_midi_pitchbend_state SharpAudioAPI::setMidiPitchBendState = nullptr;
set_midi_modwheel_state SharpAudioAPI::setMidiModWheelState = nullptr;
set_midi_volumeslider_state SharpAudioAPI::setMidiVolumeSliderState = nullptr;

bool SharpAudioAPI::Initialize(char* argv)
{
    SimpleCoreCLRHost* host = SharpAudioAPI::dotnet.GetHost();

    if(!dotnet.InitializeDotNetRuntime(argv))
        return false;

    initialize = reinterpret_cast<initialize_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Program", "Initialize"));

    if(!initialize)
        return false;

    initialize(LogMessage);

    stopApplication = reinterpret_cast<stop_application_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Program", "StopApplication"));
    compile = reinterpret_cast<compile_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "Compile"));
    stopScript = reinterpret_cast<stop_script_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "StopScript"));
    getFieldCount = reinterpret_cast<get_field_count_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "GetFieldCount"));
    getFieldType = reinterpret_cast<get_field_type_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "GetFieldType"));
    getFieldName = reinterpret_cast<get_field_name_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "GetFieldName"));
    getFieldValue = reinterpret_cast<get_field_value_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "GetFieldValue"));
    setFieldValue = reinterpret_cast<set_field_value_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "SetFieldValue"));
    getFieldMinValue = reinterpret_cast<get_field_min_value_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "GetFieldMinValue"));
    getFieldMaxValue = reinterpret_cast<get_field_max_value_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Compiler", "GetFieldMaxValue"));
    getSampleAtOffset = reinterpret_cast<get_sample_at_offset_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.AudioSource", "GetSampleAtOffset"));
    getSampleLeft = reinterpret_cast<get_sample_left_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.AudioSource", "GetSampleLeft"));
    getSampleRight = reinterpret_cast<get_sample_right_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.AudioSource", "GetSampleRight"));
    getAudioSourceCount = reinterpret_cast<get_audio_source_count_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.AudioSource", "GetAudioSourceCount"));
    getPlaybackPosition = reinterpret_cast<get_playback_position_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.AudioSource", "GetPlaybackPosition"));
    getDataLength = reinterpret_cast<get_data_length_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.AudioSource", "GetDataLength"));
    isAudioSourcePlaying = reinterpret_cast<is_audio_source_playing_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.AudioSource", "IsAudioSourcePlaying"));
    setMidiKeyStateDown = reinterpret_cast<set_midi_key_state_down_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Midi", "SetMidiKeyStateDown"));
    setMidiKeyStateUp = reinterpret_cast<set_midi_key_state_up_ptr>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Midi", "SetMidiKeyStateUp"));
    setMidiPitchBendState = reinterpret_cast<set_midi_pitchbend_state>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Midi", "SetMidiPitchBendState"));
    setMidiModWheelState = reinterpret_cast<set_midi_modwheel_state>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Midi", "SetMidiModWheelState"));
    setMidiVolumeSliderState = reinterpret_cast<set_midi_volumeslider_state>(host->clr->getCSharpFunctionPtr("SharpAudio", "SharpAudio.Midi", "SetMidiVolumeSliderState"));

    if(!InitializeMidi())
        std::cout << "Midi could not be initialized\n";

    return true;
}

bool SharpAudioAPI::InitializeMidi()
{
    midiIn = std::make_unique<RtMidiIn>();
	
	midiKeyInfo.resize(128);
	
	for(size_t i = 0; i < midiKeyInfo.size(); i++)
		midiKeyInfo[i] = 0;
    
    // Check available ports.
    unsigned int nPorts = midiIn->getPortCount();
    if (nPorts == 0 ) 
    {
        std::cout << "No ports available!\n";
        return false;
    }
    else
	{
        std::cout << "Number of ports: " << nPorts << '\n';
	}

    if(nPorts > 1)
	{
        midiIn->openPort(1);
	}
    else
    {
        return false;
    }
    
    // Set our callback function.  This should be done immediately after
    // opening the port to avoid having incoming messages written to the
    // queue.
    midiIn->setCallback(OnMidiRead);

    // Don't ignore sysex, timing, or active sensing messages.
    midiIn->ignoreTypes(false, false, false);

    return true;
}

void SharpAudioAPI::OnMidiRead(double deltatime, std::vector< unsigned char > *message, void *userData)
{
    //byte 1 = keyIndex
    //byte2 = velocity: if(velocity > 0) = on

    unsigned int nBytes = message->size();
    unsigned int keyIndex = 0;
    unsigned int velocity = 0;

	
	unsigned int byte1 = 0;


    for (unsigned int i=0; i<nBytes; i++)
    {
		if(i == 0)
		{
			byte1 = (unsigned int)message->at(i);
		}
        if(i == 1)
        {
            keyIndex = (int)message->at(i);
        }
        else if(i == 2)
        {
            velocity = (int)message->at(i);
        }		
		
        //std::cout << "Byte " << i << " = " << (int)message->at(i) << ", \n";
    }
    if ( nBytes > 0 )
    {
        //std::cout << "stamp = " << deltatime << std::endl;
		//std::cout << nBytes << "\n";
    }

	

	if(byte1 != 224 && byte1 != 176)
	{
		midiKeyInfo[keyIndex] = velocity;
		
		if(velocity > 0)
			setMidiKeyStateDown(keyIndex, velocity);
		else
			setMidiKeyStateUp(keyIndex, velocity);
		
		//std::cout << "Key index: " << keyIndex << " state: " << velocity << "\n";
	}
	else
	{
		switch(byte1)
		{
			case 224:
			{
				setMidiPitchBendState(velocity);
				break;
			}
			case 176:
			{
				if(keyIndex == 1)
				{
					setMidiModWheelState(velocity);
				}
				else if(keyIndex == 7)
				{
					setMidiVolumeSliderState(velocity);
				}
				break;
			}
			default:
				break;
		}

		//Byte 224 : Pitchbend
		//Byte 176 + Key index 1: Modwheel
		//Byte 176 + Key index 7: Volumeslider
		//std::cout << "Byte: " << byte1 << " Key index: " << keyIndex << " state: " << velocity << "\n";
	}
}

void SharpAudioAPI::LogMessage(char* message)
{
    std::string msg(message);
    if(onDebug != nullptr)
        onDebug(msg);
    delete message;
}

void SharpAudioAPI::StopApplication()
{
    if(stopApplication != nullptr)
        stopApplication();
}

bool SharpAudioAPI::Compile(const std::string& source)
{
    if(compile != nullptr)
        return compile(source.c_str());
    return false;
}

void SharpAudioAPI::StopScript()
{
    if(stopScript != nullptr)
        stopScript();
}

int SharpAudioAPI::GetFieldCount()
{
    if(getFieldCount != nullptr)
        return getFieldCount();
    return 0;
}

int SharpAudioAPI::GetFieldType(int fieldIndex)
{
    if(getFieldType != nullptr)
        return getFieldType(fieldIndex);
    return -1;
}

bool SharpAudioAPI::GetFieldName(int fieldIndex, std::string& name)
{
    if(getFieldName != nullptr)
    {
        char* fieldName = getFieldName(fieldIndex);
        name = std::string(fieldName);
        delete fieldName;
        return true;
    }
    return false;
}

bool SharpAudioAPI::GetFieldValue(int fieldIndex, std::string& value)
{
    if(getFieldValue != nullptr)
    {
        char* fieldValue = getFieldValue(fieldIndex);
        value = std::string(fieldValue);
        delete fieldValue;
        return true;
    }
    return false;
}

void SharpAudioAPI::SetFieldValue(int fieldIndex, const std::string& value)
{
    if(setFieldValue != nullptr)
        setFieldValue(fieldIndex, value.c_str());
}

double SharpAudioAPI::GetFieldMinValue(int fieldIndex)
{
    if(getFieldMinValue != nullptr)
        return getFieldMinValue(fieldIndex);
    return 0;
}

double SharpAudioAPI::GetFieldMaxValue(int fieldIndex)
{
    if(getFieldMaxValue != nullptr)
        return getFieldMaxValue(fieldIndex);
    return 0;
}

float SharpAudioAPI::GetSampleAtOffset(int audiosourceIndex, int sampleIndex)
{
    if(getSampleAtOffset != nullptr)
        return getSampleAtOffset(audiosourceIndex, sampleIndex);
    return 0;
}

float SharpAudioAPI::GetSampleLeft(int audiosourceIndex)
{
    if(getSampleLeft != nullptr)
        return getSampleLeft(audiosourceIndex);
    return 0;
}

float SharpAudioAPI::GetSampleRight(int audiosourceIndex)
{
    if(getSampleRight != nullptr)
        return getSampleRight(audiosourceIndex);
    return 0;
}

int SharpAudioAPI::GetAudioSourceCount()
{
    if(getAudioSourceCount != nullptr)
        return getAudioSourceCount();
    return 0;
}

long SharpAudioAPI::GetPlaybackPosition(int audiosourceIndex)
{
    if(getPlaybackPosition != nullptr)
        return getPlaybackPosition(audiosourceIndex);
    return 0;
}

int SharpAudioAPI::GetDataLength(int audiosourceIndex)
{
    if(getDataLength != nullptr)
        return getDataLength(audiosourceIndex);
    return 0;
}

bool SharpAudioAPI::IsAudioSourcePlaying(int audiosourceIndex)
{
    if(isAudioSourcePlaying != nullptr)
        return isAudioSourcePlaying(audiosourceIndex);
    return false;
}

void SharpAudioAPI::SetMidiKeyStateDown(int keyIndex, int velocity)
{
    if(setMidiKeyStateDown != nullptr)
        setMidiKeyStateDown(keyIndex, velocity);
}

void SharpAudioAPI::SetMidiKeyStateUp(int keyIndex, int velocity)
{
    if(setMidiKeyStateUp != nullptr)
        setMidiKeyStateUp(keyIndex, velocity);
}

void SharpAudioAPI::SetMidiPitchBendState(int velocity)
{
    if(setMidiPitchBendState != nullptr)
        setMidiPitchBendState(velocity);
}

void SharpAudioAPI::SetMidiModWheelState(int velocity)
{
    if(setMidiModWheelState != nullptr)
        setMidiModWheelState(velocity);
}

void SharpAudioAPI::SetMidiVolumeSliderState(int velocity)
{
    if(setMidiVolumeSliderState != nullptr)
        setMidiVolumeSliderState(velocity);
}