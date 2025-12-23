//
// Created by Hamza on 11/12/2025.
//

#ifndef QTTEST_READAUDIOFILE_H
#define QTTEST_READAUDIOFILE_H
#include <vector>
#include <string>

class AudioLoader {
public:

    static std::vector<float> loadAudio(
        const std::string& path,
        int &sampleRate,
        int &channels
    );
};

#endif //QTTEST_READAUDIOFILE_H