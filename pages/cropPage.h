#pragma once
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

#include "../structures/appData.h"
#include "../components/WaveformCanvas.h"
#include "../structures/history.h"
#include "../util/audioPlayer.h"

class CropPage : public QWidget {
    Q_OBJECT
public:
    explicit CropPage(QWidget* parent = nullptr);

    void setData(AppData*, AudioHistory*);
    void setChangeWindow(std::function<void(QString)> func);
    void updatePage();

    void setCurrentSample(int);

    WaveformCanvas* canvas = nullptr;

private slots:
    void onPlayClicked();
    void onApplyCrop();
    void onStartSliderChanged(int);
    void onEndSliderChanged(int);

private:
    int startSample() const;
    int endSample() const;

    AppData* appData = nullptr;
    AudioHistory* history = nullptr;

    AudioPlayer* player = nullptr;
    std::function<void(QString)> changeWindow;

    QSlider* startSlider = nullptr;
    QSlider* endSlider = nullptr;

    QLabel* rangeLabel = nullptr;
    QPushButton* playButton = nullptr;
    QPushButton* applyButton = nullptr;

    int currentSample = 0;
    bool playing = false;
};
