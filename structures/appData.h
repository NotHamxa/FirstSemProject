#pragma once
#pragma once
#include <QString>

struct AudioData {
    std::vector<float> samples;
    int sampleRate;
    int channels;
};
struct AppData {
    QString currentAudioFile;
    AudioData audioData;
};
