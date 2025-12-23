#pragma once
#include <QIODevice>
#include <vector>
#include "../structures/appData.h"
class AudioBufferDevice : public QIODevice
{
    Q_OBJECT
public:
    AudioBufferDevice(const AudioData& data, int& indexRef,std::function<void(int)> setIndexFunc, QObject* parent=nullptr)
        : QIODevice(parent), audioData(data), currentSampleIndex(indexRef), setCurrentSampleIndex(setIndexFunc) {}

    bool open(OpenMode mode) override {
        setCurrentSampleIndex(0);
        return QIODevice::open(mode);
    }

    // QAudioOutput will call this repeatedly
    qint64 readData(char* out, qint64 maxLen) override {
        const int bytesPerSample = sizeof(float);
        int samplesRequested = maxLen / bytesPerSample;

        int samplesAvailable = audioData.samples.size() - currentSampleIndex;
        int samplesToCopy = std::min(samplesRequested, samplesAvailable);

        if (samplesToCopy <= 0)
            return 0; // finished

        memcpy(out,
               audioData.samples.data() + currentSampleIndex,
               samplesToCopy * bytesPerSample);

        setCurrentSampleIndex(currentSampleIndex + samplesToCopy);

        return samplesToCopy * bytesPerSample;
    }

    qint64 writeData(const char*, qint64) override {
        return -1; // not used
    }

    qint64 bytesAvailable() const override {
        return (audioData.samples.size() - currentSampleIndex) * sizeof(float)
             + QIODevice::bytesAvailable();
    }

private:
    const AudioData& audioData;
    int& currentSampleIndex;
    std::function<void(int)> setCurrentSampleIndex;
};
