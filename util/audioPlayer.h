#pragma once
#include <QObject>
#include <QtMultimedia/QAudioSink>
#include <functional>
#include "audioBufferDevice.h"
#include "../structures/appData.h"

class AudioPlayer : public QObject
{
    Q_OBJECT
public:

    AudioPlayer(int& indexRef, std::function<void(int)> setIndexFunc, QObject* parent = nullptr)
        : QObject(parent), audioData(nullptr), device(nullptr), audioOutput(nullptr),
          currentSampleIndexRef(indexRef), setCurrentSampleIndex(setIndexFunc)
    {
    }

    // Set the audio data
    void setAudio(AudioData* data)
    {
        audioData = data;

        // Delete old objects if they exist
        if (device) { delete device; device = nullptr; }
        if (audioOutput) { delete audioOutput; audioOutput = nullptr; }

        if (!audioData) return;

        QAudioFormat format;
        format.setSampleRate(audioData->sampleRate);
        format.setChannelCount(audioData->channels);
        format.setSampleFormat(QAudioFormat::Float);

        audioOutput = new QAudioSink(format, this);

        // Pass the index reference to the device
        device = new AudioBufferDevice(*audioData, currentSampleIndexRef,setCurrentSampleIndex, this);
        device->open(QIODevice::ReadOnly);
    }

    void play() {
        if (!audioOutput || !device) return;
        if (audioOutput->state() == QAudio::SuspendedState)
            audioOutput->resume();
        else
            audioOutput->start(device);
    }

    void pause() {
        if (audioOutput) audioOutput->suspend();
    }

    void stop() {
        if (audioOutput) audioOutput->stop();
        if (setCurrentSampleIndex) setCurrentSampleIndex(0);
        if (device) device->open(QIODevice::ReadOnly);
    }

private:
    AudioData* audioData = nullptr;
    QAudioSink* audioOutput = nullptr;
    AudioBufferDevice* device = nullptr;
    int& currentSampleIndexRef;
    std::function<void(int)> setCurrentSampleIndex;
};
