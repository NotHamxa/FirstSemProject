#pragma once

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <functional>
#include <vector>

#include "../components/WaveformCanvas.h"
#include "../structures/appData.h"
#include "../structures/history.h"
class EqPage : public QWidget
{
    Q_OBJECT

public:
    explicit EqPage(QWidget* parent = nullptr);

    void setData(AppData*,AudioHistory*);
    void setChangeWindow(std::function<void(QString)> fn);
    std::vector<float> modifiedSamples;
    WaveformCanvas *canvas;

private slots:
    void onApplyClicked();
    void onBackClicked();

private:
    struct Biquad {
        float a0=1, a1=0, a2=0;
        float b1=0, b2=0;
        float z1=0, z2=0;

        float process(float in);
    };

    Biquad makePeakingEQ(float freq, float q, float gainDb, int sampleRate);
    void applyEq(std::vector<float>& samples);

    QWidget* makeBand(const QString& name, QSlider*& slider);

private:
    AppData* appData = nullptr;
    AudioHistory *history = nullptr;


    std::function<void(QString)> changeWindow;

    QSlider *low, *lowMid, *mid, *highMid, *high;



};
