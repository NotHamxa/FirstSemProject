#pragma once
#include <cmath>

class Biquad {
public:
    void setupPeakingEQ(float sampleRate,
                        float centerFreq,
                        float Q,
                        float gainDb)
    {
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * float(M_PI) * centerFreq / sampleRate;
        float alpha = std::sin(w0) / (2.0f * Q);
        float cosw0 = std::cos(w0);

        float b0 = 1 + alpha * A;
        float b1 = -2 * cosw0;
        float b2 = 1 - alpha * A;
        float a0 = 1 + alpha / A;
        float a1 = -2 * cosw0;
        float a2 = 1 - alpha / A;

        m_b0 = b0 / a0;
        m_b1 = b1 / a0;
        m_b2 = b2 / a0;
        m_a1 = a1 / a0;
        m_a2 = a2 / a0;
    }

    float process(float x)
    {
        float y = m_b0 * x + m_z1;
        m_z1 = m_b1 * x - m_a1 * y + m_z2;
        m_z2 = m_b2 * x - m_a2 * y;
        return y;
    }

private:
    float m_b0{}, m_b1{}, m_b2{}, m_a1{}, m_a2{};
    float m_z1{}, m_z2{};
};
