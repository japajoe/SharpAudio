using System;
using SharpAudio;

public class KeyboardTubeBell : AudioBaseType
{
	long timer = 0;

	[Inspectable]
	public float frequency = 220.0f;

	[Inspectable]
	public float LFO = 1.1f;

	[Inspectable]
	public float gain = 0.5f;

	private int[] midiKeys;

	private Envelope[] envelope1;
	private Envelope[] envelope2;
	private Envelope[] envelope3;
	private Envelope[] envelope4;
	private Envelope[] envelope5;
	private Envelope[] envelope6;
	private Envelope[] envelopeMain;
	private int sampleRate = 44100;
	private WavetableCalculator wavetable;


	public override void Start()
	{
		midiKeys = new int[128];

		for(int i = 0; i < midiKeys.Length; i++)
			midiKeys[i] = 0;

		wavetable = new WavetableCalculator(new TriangleCalculator());

		SetupEnvelopes();

		audioSource.Play();
	}

	void SetupEnvelopes()
	{
		envelope1 = new Envelope[128];
		envelope2 = new Envelope[128];
		envelope3 = new Envelope[128];
		envelope4 = new Envelope[128];
		envelope5 = new Envelope[128];
		envelope6 = new Envelope[128];
		envelopeMain = new Envelope[128];

		for(int i = 0; i < envelope1.Length; i++)
		{
			envelope1[i] = new Envelope();
			envelope2[i] = new Envelope();
			envelope3[i] = new Envelope();
			envelope4[i] = new Envelope();
			envelope5[i] = new Envelope();
			envelope6[i] = new Envelope();
			envelopeMain[i] = new Envelope();
			SetEnvelopes(i);
		}
	}

	void SetEnvelopes(int keyIndex)
	{
		envelope1[keyIndex].Reset();
		envelope2[keyIndex].Reset();
		envelope3[keyIndex].Reset();
		envelope4[keyIndex].Reset();

		envelope1[keyIndex].AttackRate = 0.005f * sampleRate;
        envelope1[keyIndex].DecayRate = 4 * sampleRate;
        envelope1[keyIndex].SustainLevel = 0;
        envelope1[keyIndex].ReleaseRate = 0.004f;
        envelope1[keyIndex].State = Envelope.EnvelopeState.Attack;

		envelope2[keyIndex].AttackRate = 0.005f * sampleRate;
        envelope2[keyIndex].DecayRate = 4 * sampleRate;
        envelope2[keyIndex].SustainLevel = 0;
        envelope2[keyIndex].ReleaseRate = 0.004f;
        envelope2[keyIndex].State = Envelope.EnvelopeState.Attack;

		envelope3[keyIndex].AttackRate = 0.001f * sampleRate;
        envelope3[keyIndex].DecayRate = 3* sampleRate;
        envelope3[keyIndex].SustainLevel = 0;
        envelope3[keyIndex].ReleaseRate = 0.004f;
        envelope3[keyIndex].State = Envelope.EnvelopeState.Attack;

		envelope4[keyIndex].AttackRate = 0.004f * sampleRate;
        envelope4[keyIndex].DecayRate = 4 * sampleRate;
        envelope4[keyIndex].SustainLevel = 0;
        envelope4[keyIndex].ReleaseRate = 0.004f;
        envelope4[keyIndex].State = Envelope.EnvelopeState.Attack;

		envelope5[keyIndex].AttackRate = 0.001f * sampleRate;
        envelope5[keyIndex].DecayRate = 1.50f * sampleRate;
        envelope5[keyIndex].SustainLevel = 0;
        envelope5[keyIndex].ReleaseRate = 0.04f * sampleRate;
        envelope5[keyIndex].State = Envelope.EnvelopeState.Attack;

		envelope6[keyIndex].AttackRate = 0.001f * sampleRate;
        envelope6[keyIndex].DecayRate = 1.50f * sampleRate;
        envelope6[keyIndex].SustainLevel = 0;
        envelope6[keyIndex].ReleaseRate = 0.04f * sampleRate;
        envelope6[keyIndex].State = Envelope.EnvelopeState.Attack;

		envelopeMain[keyIndex].AttackRate = 0.001f * sampleRate;
        envelopeMain[keyIndex].DecayRate = 1.50f * sampleRate;
        envelopeMain[keyIndex].SustainLevel = 0.1f;
        envelopeMain[keyIndex].ReleaseRate = 0.04f * sampleRate;
        envelopeMain[keyIndex].State = Envelope.EnvelopeState.Attack;
	}

    public override void MidiKeyboardDown(int keyIndex, int velocity)
    {
		SetEnvelopes(keyIndex);
		midiKeys[keyIndex] = velocity;

    }

    public override void MidiKeyboardUp(int keyIndex, int velocity)
    {
		midiKeys[keyIndex] = 0;

    }

    public override void OnAudioRead(float[] buffer, int channels)
    {
		float x = 0;
        float t  = 1.0f / 128;
        float amp = 0.0f;
		float freq = 1.0f;
		float sampleLeft = 0;
		float sampleRight = 0;

		for(int i = 0; i < buffer.Length; i += channels)
		{
            for(int y = 0; y < midiKeys.Length; y++)
            {
                if(midiKeys[y] > 0)
                {
                    amp = (1.0f / 128) * midiKeys[y];
        			x += amp * GetModulatedWave(y, timer);
                }
            }

			//x *= (float)Math.Sin(2 * Math.PI * LFO * timer / 44100);
            x *= gain;

			sampleLeft = (float)Math.Sin(2 * Math.PI * LFO * timer / 44100) * x;
			sampleRight = (float)Math.Cos(2 * Math.PI * LFO * timer / 44100) * x;

			buffer[i] = sampleLeft;
            if(channels == 2)
    			buffer[i+1] = sampleRight;
	
    		timer++;
            x = 0.0f;
		}

        x = 0.0f;
    }

	float GetModulatedWave(int keyIndex, long time)
	{
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 0;
		float baseFreq = AudioUtility.GetPianoFrequency(keyIndex, 440);

		x = (float)Math.Sin(2 * Math.PI * time * baseFreq / 44100);
		x = (float)Math.Sin(2 * Math.PI * time * (baseFreq*(1.0 * 0.995)) / 44100 + x);
		x *= envelope1[keyIndex].Process();

		x = (float)Math.Sin(2 * Math.PI * time * (baseFreq* (1.414 * 0.995)) / 44100 + x);
		x *= envelope2[keyIndex].Process();

		y = (float)Math.Sin(2 * Math.PI * time * baseFreq / 44100);
		y = (float)Math.Sin(2 * Math.PI * time * (baseFreq * (1.0 * 0.995)) / 44100 + y);
		y *= envelope3[keyIndex].Process();

		y = (float)Math.Sin(2 * Math.PI * time * (baseFreq * (1.414 * 0.995)) / 44100 + y);
		y *= envelope4[keyIndex].Process();



		//w = (float)Math.Sin(2 * Math.PI * timer * baseFreq / 44100);
		//w *= envelopeMain[keyIndex].Process();

		return x + y + z + w;
	}
}






