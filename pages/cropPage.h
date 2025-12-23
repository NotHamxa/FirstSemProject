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
    void back();
private:
    int startSample() const;
    int endSample() const;

    AppData* appData;
    AudioHistory* history;

    AudioPlayer* player;
    std::function<void(QString)> changeWindow;

    QSlider* startSlider;
    QSlider* endSlider;

    QLabel* rangeLabel;
    QPushButton* playButton ;
    QPushButton* applyButton;

    int currentSample = 0;
    bool playing = false;
};
