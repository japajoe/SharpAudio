using System;
using System.Collections.Generic;
using SharpAudio;

public class KeyboardHarp : AudioBaseType
{
    [Inspectable] public float gain = 0.5f;
    [Inspectable] public float attackAmp = 0.01f;
    [Inspectable] public float decayAmp = 3f;
    [Inspectable] public float sustainAmp = 0;
    [Inspectable] public float releaseAmp = 2f;
    [Inspectable] public float attackFreq = 0.1f;
    [Inspectable] public float decayFreq = 3f;
    [Inspectable] public float sustainFreq = 1f;
    [Inspectable] public float releaseFreq = 2f;

    private float frequency = 440f;
    private int sampleRate = 44100;
    private long timecount = 0;
    private float x = 0;
    private int[] keys;
    private float t = (1.0f / 128);
    private float amp = 1;
    private int dataLength = 4096;
	private List<WaveOperator[]> waveOperators = new List<WaveOperator[]>();
	private	Envelope envelopeAmp = new Envelope();
	private WavetableCalculator wavetable;

    public override void Start()
    {
        t = (1.0f / 128);
        amp = 1;
        CreateWaveOperators();
        wavetable = new WavetableCalculator(new SinewaveCalculator());
		audioSource.Play();
    }

    private void CreateWaveOperators()
    {
        for (int i = 0; i < 128; i++)
        {
            int[] keyIndexes = new int[3];
            keyIndexes[0] = i;
            keyIndexes[1] = keyIndexes[0] + 12;
            keyIndexes[2] = keyIndexes[0] - 12;

            WaveOperator[] operators = new WaveOperator[keyIndexes.Length];

            for (int y = 0; y < operators.Length; y++)
            {
                operators[y] = new WaveOperator(frequency, 1);

                operators[y].KeyIndex = keyIndexes[y];

                operators[y].EnvelopeFrequency.Reset();
                operators[y].EnvelopeFrequency.AttackRate = 0;
                operators[y].EnvelopeFrequency.DecayRate = decayAmp * sampleRate;
                operators[y].EnvelopeFrequency.SustainLevel = 1;
                operators[y].EnvelopeFrequency.ReleaseRate = releaseAmp * sampleRate;
                operators[y].EnvelopeFrequency.State = Envelope.EnvelopeState.Attack;

                operators[y].EnvelopeAmplitude.Reset();
                operators[y].EnvelopeAmplitude.AttackRate = attackAmp * sampleRate;
                operators[y].EnvelopeAmplitude.DecayRate = decayAmp * sampleRate;
                operators[y].EnvelopeAmplitude.SustainLevel = sustainAmp;
                operators[y].EnvelopeAmplitude.ReleaseRate = releaseAmp * sampleRate;
                operators[y].EnvelopeAmplitude.State = Envelope.EnvelopeState.Attack;
            }
            waveOperators.Add(operators);
        }
    }

    public override void MidiKeyboardDown(int keyIndex, int velocity)
    {
        SetEnvelope((int)keyIndex);
	    timecount = 0;
    }

    public override void MidiKeyboardUp(int keyIndex, int velocity)
    {

    }

    public override void OnAudioRead(float[] buffer, int channels, int bufferSize)
    {
        keys = Midi.keys;
        x = 0;

        for (int i = 0; i < buffer.Length; i += channels)
        {
            for (int y = 0; y < 128; y++)
            {
                if (keys[y] > 0)
                {
                    amp = keys[y] * t;
                    x += AudioUtility.ModulatedWave(frequency, sampleRate, (int)timecount, waveOperators[y]) * amp;
                }
            }

            x *= gain * envelopeAmp.Process();
            buffer[i] = x;
            if (channels == 2)
                buffer[i + 1] = x;
            timecount++;

            x = 0;
        }

        x = 0;
    }

    private void SetEnvelope(int index)
    {
        for (int i = 0; i < waveOperators[index].Length; i++)
        {
            waveOperators[index][i].EnvelopeAmplitude.Reset();
            waveOperators[index][i].EnvelopeAmplitude.AttackRate = attackAmp * sampleRate;
            waveOperators[index][i].EnvelopeAmplitude.DecayRate = decayAmp * sampleRate;
            waveOperators[index][i].EnvelopeAmplitude.SustainLevel = sustainAmp;
            waveOperators[index][i].EnvelopeAmplitude.ReleaseRate = releaseAmp * sampleRate;
            waveOperators[index][i].EnvelopeAmplitude.State = Envelope.EnvelopeState.Attack;

            waveOperators[index][i].EnvelopeFrequency.Reset();
            waveOperators[index][i].EnvelopeFrequency.AttackRate = 0;
            waveOperators[index][i].EnvelopeFrequency.DecayRate = decayAmp * sampleRate;
            waveOperators[index][i].EnvelopeFrequency.SustainLevel = 1;
            waveOperators[index][i].EnvelopeFrequency.ReleaseRate = releaseAmp * sampleRate;
            waveOperators[index][i].EnvelopeFrequency.State = Envelope.EnvelopeState.Attack;
        }

		envelopeAmp.Reset();
		envelopeAmp.AttackRate = attackAmp * sampleRate;
		envelopeAmp.DecayRate = decayAmp * sampleRate;
		envelopeAmp.SustainLevel = sustainAmp;
		envelopeAmp.ReleaseRate = releaseAmp * sampleRate;
		envelopeAmp.State = Envelope.EnvelopeState.Attack;
    }
}
