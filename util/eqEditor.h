#pragma once

#include <vector>
#include "../structures/appData.h"
#include "biquad.h"
class EqEditor {
public:
    static constexpr int BandCount = 8;

    EqEditor(int sampleRate, int channels);

    void setBandGain(int bandIndex, float gainDb);
    void process(AudioData& audio);

private:
    struct Band {
        float frequency;
        float gainDb;
    };

    int m_sampleRate;
    int m_channels;

    std::vector<Band> m_bands;
    std::vector<std::vector<Biquad>> m_filters;
};
