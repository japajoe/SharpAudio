# SharpAudio
Realtime audio processing IDE using C# and the DotNetCore runtime.

![](SharpAudioPreview.png)

# Features
- Wavetable generators
- Several filters like lowpass/highpass/bandpass/shelf/notch
- ADSR envelope controller
- Load wavefile into AudioClip from file or from a buffer
- Midi keyboard support
- Compile code once and change values through inspector for realtime feedback

# Dependencies
- DotNetCore Runtime and SDK https://dotnet.microsoft.com/download/dotnet-core/3.1
- libgtk-3-dev
- glib 2.0
- libgtksourceview-3.0-dev
- libLinuxForms https://github.com/japajoe/LinuxForms
- libByteMe https://github.com/japajoe/ByteMe
- libasound
- libsdl2-dev

# Building
Install all dependencies and run make. Be sure to place all DLL files and style.css plus the 'res' folder in same directory as the executable.

# Generating a basic sound
```csharp
using System;
using SharpAudio;

public class Example : AudioBaseType
{	
	[Inspectable] public float gain = 0.5;
	[Inspectable] public float frequency = 440.0f;

	private long timer = 0;

	public override void Start()
	{
		audioSource.Play();
	}

	public override void OnAudioRead(float[] buffer, int channels)
	{
		float sample = 0.0f;

		for(int i = 0; i < buffer.Length; i+=channels)
		{
			sample = (float)Math.Sin(2 * Math.PI * timer * frequency / 44100) * gain;

			buffer[i] = sample;

			if(channels == 2)
				buffer[i+1] = sample;

			timer++;
		}
	}
}
```

# Disclaimer
All the information provided on this repository is provided on an “as is” and “as available” basis and you agree that you use such information entirely at your own risk.

The author gives no warranty and accepts no responsibility or liability for the accuracy or the completeness of the information and materials contained in this repository. Under no circumstances will the author be held responsible or liable in any way for any claims, damages, losses, expenses, costs or liabilities whatsoever (including, without limitation, any direct or indirect damages for loss of profits, business interruption or loss of information) resulting or arising directly or indirectly from your use of or inability to use this repository or any websites linked to it, or from your reliance on the information and material on this repository, even if the author has been advised of the possibility of such damages in advance.
