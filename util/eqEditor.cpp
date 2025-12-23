#include "EqEditor.h"
#include "biquad.h"

EqEditor::EqEditor(int sampleRate, int channels)
    : m_sampleRate(sampleRate), m_channels(channels)
{
    m_bands = {
        {  60.0f, 0.0f },
        { 170.0f, 0.0f },
        { 310.0f, 0.0f },
        { 600.0f, 0.0f },
        {1000.0f, 0.0f },
        {3000.0f, 0.0f },
        {6000.0f, 0.0f },
        {12000.0f,0.0f }
    };

    m_filters.resize(m_channels);
    for (int ch = 0; ch < m_channels; ++ch) {
        m_filters[ch].resize(BandCount);
        for (int b = 0; b < BandCount; ++b) {
            m_filters[ch][b].setupPeakingEQ(
                m_sampleRate,
                m_bands[b].frequency,
                1.0f,
                0.0f
            );
        }
    }
}

void EqEditor::setBandGain(int bandIndex, float gainDb)
{
    if (bandIndex < 0 || bandIndex >= BandCount) return;

    m_bands[bandIndex].gainDb = gainDb;

    for (int ch = 0; ch < m_channels; ++ch) {
        m_filters[ch][bandIndex].setupPeakingEQ(
            m_sampleRate,
            m_bands[bandIndex].frequency,
            1.0f,
            gainDb
        );
    }
}

void EqEditor::process(AudioData& audio)
{
    if (audio.channels != m_channels) return;

    for (size_t i = 0; i < audio.samples.size(); i += m_channels) {
        for (int ch = 0; ch < m_channels; ++ch) {
            float sample = audio.samples[i + ch];

            for (int b = 0; b < BandCount; ++b) {
                sample = m_filters[ch][b].process(sample);
            }

            audio.samples[i + ch] = sample;
        }
    }
}
