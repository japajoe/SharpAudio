using System;
using SharpAudio;

using Random = SharpAudio.Random;

public class RandomSoundMaker : AudioBaseType
{
    [Inspectable] public float gain = 0.5f;
    [Inspectable] public float carrierFrequency = 440;
    [Inspectable] public float frequencyMin = 40;
    [Inspectable] public float frequencyMax = 400;

    [Inspectable] public float attackMin = 0.01f;
    [Inspectable] public float attackMax = 2.0f;
    [Inspectable] public float decayMin = 0.1f;
    [Inspectable] public float decayMax = 2.0f;
    [Inspectable] public float sustainMin = 0.0f;
    [Inspectable] public float sustainMax = 1.0f;
    [Inspectable] public float releaseMin = 0.1f;
    [Inspectable] public float releaseMax = 3.0f;

    public static readonly int sampleRate = 44100;
    private WavetableCalculator wavetable;
    long timer = 0;

    private WaveOperator[] waveOperators = null;


    public override void Start()
    {
        wavetable = new WavetableCalculator(new SinewaveCalculator());
    }

    public void CreateSound()
    {
        int randomNumber = Random.Range(1, 3);

        waveOperators = new WaveOperator[randomNumber];

        carrierFrequency = Random.Range(frequencyMin, frequencyMax);
        //      Debug.Log("Carrier Frequency " + carrierFrequency);

        for (int i = 0; i < waveOperators.Length; i++)
        {
            float frequency = Random.Range(frequencyMin, frequencyMax);
            //          Debug.Log("Frequency " + carrierFrequency);
            waveOperators[i] = new WaveOperator(frequency, 1.0f);
            SetEnvelope(waveOperators[i].EnvelopeAmplitude);
            SetEnvelope(waveOperators[i].EnvelopeFrequency);
        }
    }

    public void SetEnvelope(Envelope envelope)
    {
        envelope.Reset();
        envelope.AttackRate = Random.Range(attackMin, attackMax) * sampleRate;
        envelope.DecayRate = Random.Range(decayMin, decayMax) * sampleRate;
        envelope.SustainLevel = Random.Range(sustainMin, sustainMax);
        envelope.ReleaseRate = Random.Range(releaseMin, releaseMax) * sampleRate;
        envelope.State = Envelope.EnvelopeState.Attack;
    }
    
	public void ResetEnvelopes()
	{
		if(waveOperators == null)
			return;

        for (int i = 0; i < waveOperators.Length; i++)
		{
            waveOperators[i].EnvelopeAmplitude.Reset();
            waveOperators[i].EnvelopeFrequency.Reset();
            waveOperators[i].EnvelopeAmplitude.State = Envelope.EnvelopeState.Attack;
            waveOperators[i].EnvelopeFrequency.State = Envelope.EnvelopeState.Attack;
		}
	}

    public override void KeyboardDown(KeyCode vkCode)
    {
        if (vkCode == KeyCode.Space)
        {
            if (audioSource.IsPlaying)
            {
                audioSource.Stop();
            }
            else
            {
                timer = 0;
                CreateSound();
                audioSource.Play();
            }
        }
        else if (vkCode == KeyCode.A)
		{
			audioSource.Stop();
			timer = 0;
			ResetEnvelopes();
			audioSource.Play();
		}
    }

    private float sample = 0;

    public override void OnAudioRead(float[] buffer, int channels)
    {
        for (int i = 0; i < buffer.Length; i += channels)
        {
            sample = AudioUtility.ModulatedWave(carrierFrequency, sampleRate, (int)timer, waveOperators) * gain;
            buffer[i] = sample;

            if (channels == 2)
                buffer[i + 1] = sample;

            timer++;
        }
    }
}
