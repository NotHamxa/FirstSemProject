#pragma once
#include<vector>
#include <stdexcept>
#include"appData.h"

struct AudioHistory {
    std::vector<AudioData> history;
    int currentIndex = -1;
    void add(const AudioData& data) {
        if (currentIndex + 1 < static_cast<int>(history.size())) {
            history.erase(history.begin() + currentIndex + 1, history.end());
        }
        history.push_back(data);
        currentIndex = history.size() - 1;
    }
    void clear() {
        history.clear();
        currentIndex = -1;
    }
    AudioData undo() {
        if (canUndo()) {
            --currentIndex;
            return history[currentIndex];
        }
        throw std::out_of_range("Cannot undo: already at the oldest entry");
    }
    AudioData redo() {
        if (canRedo()) {
            ++currentIndex;
            return history[currentIndex];
        }
        throw std::out_of_range("Cannot redo: already at the newest entry");
    }
    bool canUndo() const {
        return currentIndex > 0;
    }
    bool canRedo() const {
        return currentIndex + 1 < static_cast<int>(history.size());
    }
    AudioData current() const {
        if (currentIndex >= 0 && currentIndex < static_cast<int>(history.size())) {
            return history[currentIndex];
        }
        throw std::out_of_range("No current audio data");
    }
    int size() const {
        return history.size();
    }
};