using System;
using SharpAudio;
using SharpAudio.Filters;

public class ProtoType : AudioBaseType
{	
	[Inspectable] public float filterFrequency = 440;
	[Inspectable] public float filterMod = 3.3f;
	[Inspectable] public float resonance = 16f;

	private float frequency = 440f;
	private long timecount = 0;
	private float x = 0;
	private	uint[] keys;
	private float t = (1.0f/128);
	private float amp = 1;
	private int dataLength = 4096;
	private LowpassFilter filter;

	public override void Start()
	{
	    filter = new LowpassFilter(44100, 440);
		filter.Q = resonance;
        	
		t = (1.0f/128);
		amp = 1;
	}

	public override void KeyboardDown(KeyCode vkCode)
	{
		if(Input.GetKeyDown(KeyCode.LeftControl))
		{
			if(Input.GetKeyDown(KeyCode.P))
			{
				if(!audioSource.IsPlaying)
					audioSource.Play();
				else
					audioSource.Stop();
			}
		}
	}



    public override void OnAudioRead(float[] buffer, int channels)
    {
		keys = MidiInput.keys;
		x = 0;

		for(int i = 0; i < buffer.Length; i+=channels)
		{
			for(int y = 0; y < 128; y++)
			{
				if(keys[y] > 0)
				{
					amp = keys[y] * t;
					frequency = AudioUtility.GetPianoFrequency(y, 440);
					x += (float)Math.Sin(2 * Math.PI * timecount * frequency / 44100) * amp;
				}
			}

			filter.Frequency = 880 + (float)(Math.Sin(2 * Math.PI * filterMod * timecount / 44100) * filterFrequency);
			filter.Q = resonance;
			x = filter.Process(x);

			x *= 0.1f;
			buffer[i] = x;
			if(channels == 2)
				buffer[i+1] = x;
			timecount++;

			x = 0;
		}

		x = 0;
    }
}
