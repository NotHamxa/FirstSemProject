#include "readAudioFile.h"
#include <sndfile.h>
#include <stdexcept>

std::vector<float> AudioLoader::loadAudio(
        const std::string& path,
        int &sampleRate,
        int &channels)
{
    SF_INFO info;
    info.format = 0;
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &info);
    if (!file) {
        throw std::runtime_error("Failed to open audio file: " + path);
    }

    sampleRate = info.samplerate;
    channels   = info.channels;
    std::vector<float> samples(info.frames * info.channels);
    sf_count_t count = sf_read_float(file, samples.data(), samples.size());
    sf_close(file);

    if (count != (sf_count_t)samples.size()) {
        throw std::runtime_error("Failed to read full audio data");
    }

    return samples;
}