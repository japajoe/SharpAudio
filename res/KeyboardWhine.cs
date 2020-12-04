using System;
using SharpAudio;

public class ProtoType : AudioBaseType
{	
	[Inspectable]
	public float distortionAmount = 50;

	[Inspectable]
	public float distortionDepth = 0.1f;

	[Inspectable]
	public float gain = 0.5f;

	private float frequency = 440f;

	private long timecount = 0;
	private float x = 0;
	private	int[] keys;
	private float t = (1.0f/128);
	private float amp = 1;
	private int dataLength = 4096;

	private float attackRate;
	private float decayRate;
	private float sustainLevel;
	private float releaseRate;

	Envelope[] envelopesAmp = new Envelope[128];
	Envelope[] envelopesFreq = new Envelope[128];

	WavetableCalculator wavetable;
	WavetableCalculator[] wavetables = new WavetableCalculator[128];

	public override void Start()
	{       	
		t = (1.0f/128);
		amp = 1;

		attackRate = 0.2f;
		decayRate = 1f;
		sustainLevel = 1f;
		releaseRate = 1f;

		for(int i = 0; i < envelopesAmp.Length; i++)
		{
			envelopesAmp[i] = new Envelope();
			envelopesFreq[i] = new Envelope();
		}

		wavetable = new WavetableCalculator(new SquareWaveCalculator());

		for(int i = 0; i < wavetables.Length; i++)
		{
			wavetables[i] = new WavetableCalculator(new SquareWaveCalculator());
		}		


		audioSource.Play();
	}

    public override void MidiKeyboardDown(int keyIndex, int velocity)
    {
        SetEnvelope(keyIndex);
    }

    public override void MidiKeyboardUp(int keyIndex, int velocity)
    {

    }

	float envAmp = 0;
	float envFreq = 0;
	float startFrequency = 440;
	float freq = 0;
	float targetFrequency = 0;

	public override void OnAudioRead(float[] buffer, int channels, int bufferSize)
	{
		keys = Midi.keys;
		x = 0;

		envAmp = 0;
		amp = 0;
		envFreq = 0;
		frequency = 0;
		startFrequency = 440;
		freq = startFrequency - targetFrequency;

		for(int i = 0; i < buffer.Length; i+=channels)
		{
			for(int y = 0; y < 128; y++)
			{
				if(keys[y] > 0)
				{
					startFrequency = AudioUtility.GetPianoFrequency(y, 440);
					targetFrequency = AudioUtility.GetPianoFrequency(y+24, 440);
					freq = startFrequency - targetFrequency;
					envFreq = envelopesFreq[y].Process();
					envAmp = envelopesAmp[y].Process();
					frequency = startFrequency - (envFreq * freq);
					amp = keys[y] * envAmp;
					//x += wavetable.Calculate((int)timecount, frequency, 44100) * envAmp;
					x += wavetables[y].Calculate((int)timecount, frequency, 44100) * envAmp;
					x = (float)AudioUtility.Distort(x, distortionAmount, distortionDepth);
				}
			}

			x *= gain;
			buffer[i] = x;
			if(channels == 2)
				buffer[i+1] = x;
			timecount++;

			x = 0;
		}

		x = 0;
	}

	private void SetEnvelope(int index)
	{
		envelopesFreq[index].Reset();
		envelopesFreq[index].AttackRate = attackRate * 44100;
		envelopesFreq[index].DecayRate = decayRate * 44100;
		envelopesFreq[index].SustainLevel = sustainLevel;
		envelopesFreq[index].ReleaseRate = releaseRate * 44100;
		envelopesFreq[index].State = Envelope.EnvelopeState.Attack;

		envelopesAmp[index].Reset();
		envelopesAmp[index].AttackRate = 0.1f * 44100;
		envelopesAmp[index].DecayRate = 3 * 44100;
		envelopesAmp[index].SustainLevel = 0f;
		envelopesAmp[index].ReleaseRate = 3 * 44100;
		envelopesAmp[index].State = Envelope.EnvelopeState.Attack;
	}

	private void SetEnvelopes()
	{
		for(int i = 0; i < envelopesAmp.Length; i++)
		{
			envelopesFreq[i].Reset();
			envelopesFreq[i].AttackRate = attackRate * 44100;
			envelopesFreq[i].DecayRate = decayRate * 44100;
			envelopesFreq[i].SustainLevel = sustainLevel;
			envelopesFreq[i].ReleaseRate = releaseRate * 44100;
			envelopesFreq[i].State = Envelope.EnvelopeState.Attack;

			envelopesAmp[i].Reset();
			envelopesAmp[i].AttackRate = 0.1f * 44100;
			envelopesAmp[i].DecayRate = 3 * 44100;
			envelopesAmp[i].SustainLevel = 0f;
			envelopesAmp[i].ReleaseRate = 3 * 44100;
			envelopesAmp[i].State = Envelope.EnvelopeState.Attack;
		}
	}

}
