#include "eqPage.h"
#include <cmath>
#include <algorithm>
#include <QPushButton>
#include <QMessageBox>

EqPage::EqPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);

    auto* title = new QLabel("Audio Equalizer", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold;");
    root->addWidget(title);

    // Canvas
    canvas = new WaveformCanvas(this);
    root->addWidget(canvas);

    // Sliders
    auto* bands = new QHBoxLayout;
    bands->addWidget(makeBand("Low",      low));
    bands->addWidget(makeBand("Low-Mid",  lowMid));
    bands->addWidget(makeBand("Mid",      mid));
    bands->addWidget(makeBand("High-Mid", highMid));
    bands->addWidget(makeBand("High",     high));

    root->addLayout(bands);

    auto* btns = new QHBoxLayout;
    auto* back = new QPushButton("Back");
    auto* apply = new QPushButton("Apply EQ");

    QString buttonStyle =
        "QPushButton {"
        "  background-color: #1f1f1f;"
        "  color: #f0f0f0;"
        "  border: 1px solid #2a2a2a;"
        "  border-radius: 8px;"
        "  padding: 10px 16px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2a2a2a;"
        "  border-color: #3a3a3a;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #181818;"
        "}";

    back->setStyleSheet(buttonStyle);
    apply->setStyleSheet(buttonStyle);
    back->setCursor(Qt::PointingHandCursor);
    apply->setCursor(Qt::PointingHandCursor);


    back->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    apply->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    btns->addWidget(back);
    btns->addSpacing(20);
    btns->addWidget(apply);

    root->addLayout(btns);

    connect(back, &QPushButton::clicked, this, &EqPage::onBackClicked);
    connect(apply, &QPushButton::clicked, this, &EqPage::onApplyClicked);

    // Connect Sliders
    auto updateFunc = [this]() {
        if (!appData || appData->audioData.samples.empty()) return;

        // Copy original samples
        modifiedSamples = appData->audioData.samples;

        // Apply demo gain based on sliders (simple scaling)
        for (float& s : modifiedSamples) {
            s *= 1.0f + (low->value() + lowMid->value() + mid->value() +
                         highMid->value() + high->value()) / 60.0f; // scale for demo
            s = std::clamp(s, -1.0f, 1.0f);
        }

        canvas->setSamples(modifiedSamples); // assume canvas has setSamples()
        canvas->update();
    };

    connect(low, &QSlider::valueChanged, this, updateFunc);
    connect(lowMid, &QSlider::valueChanged, this, updateFunc);
    connect(mid, &QSlider::valueChanged, this, updateFunc);
    connect(highMid, &QSlider::valueChanged, this, updateFunc);
    connect(high, &QSlider::valueChanged, this, updateFunc);
}

void EqPage::setData(AppData *data, AudioHistory *audioHistory) {
    appData = data;
    history = audioHistory;
}

void EqPage::setChangeWindow(std::function<void(QString)> fn)
{
    changeWindow = fn;
}

QWidget* EqPage::makeBand(const QString& name, QSlider*& slider)
{
    auto* box = new QVBoxLayout;
    auto* label = new QLabel(name);
    label->setAlignment(Qt::AlignCenter);

    slider = new QSlider(Qt::Vertical);
    slider->setRange(-12, 12);
    slider->setValue(0);
    slider->setTickInterval(3);
    slider->setTickPosition(QSlider::TicksBelow);

    box->addWidget(label);
    box->addWidget(slider);

    auto* w = new QWidget;
    w->setLayout(box);
    return w;
}

void EqPage::onBackClicked()
{
    if (changeWindow)
        changeWindow("main");
}

void EqPage::onApplyClicked()
{
    if (!appData || appData->audioData.samples.empty())
        return;


    applyEq(appData->audioData.samples);

    if (canvas)
        canvas->update();
    history->add(appData->audioData);
    changeWindow("main");
}
float EqPage::Biquad::process(float in)
{
    float out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    return out;
}

EqPage::Biquad EqPage::makePeakingEQ(float freq, float q, float gainDb, int sr)
{
    Biquad b;

    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * M_PI * freq / sr;
    float alpha = std::sin(w0) / (2.0f * q);

    float a0 = 1 + alpha / A;
    b.a0 = (1 + alpha * A) / a0;
    b.a1 = (-2 * std::cos(w0)) / a0;
    b.a2 = (1 - alpha * A) / a0;
    b.b1 = (-2 * std::cos(w0)) / a0;
    b.b2 = (1 - alpha / A) / a0;

    return b;
}

void EqPage::applyEq(std::vector<float>& samples)
{
    int sr = appData->audioData.sampleRate;
    if (sr <= 0) return;

    Biquad filters[] = {
        makePeakingEQ(80,    0.7f, low->value(),     sr),
        makePeakingEQ(250,   0.8f, lowMid->value(),  sr),
        makePeakingEQ(1000,  1.0f, mid->value(),     sr),
        makePeakingEQ(4000,  1.0f, highMid->value(), sr),
        makePeakingEQ(10000, 0.7f, high->value(),    sr)
    };

    for (float& s : samples) {
        float x = s;
        for (auto& f : filters)
            x = f.process(x);

        s = std::clamp(x, -1.0f, 1.0f);
    }
}
