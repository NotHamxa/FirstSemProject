#pragma once
#include <QWidget>
#include <vector>
#include <functional>

class WaveformCanvas : public QWidget {
    Q_OBJECT

public:
    explicit WaveformCanvas(QWidget *parent = nullptr);

    void setSamples(const std::vector<float> &samples);
    void clearSamples();
    void setCurrentSample(int index);

    // Set a callback for clicks on the waveform
    void setClickCallback(std::function<void(float)> callback);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;  // Handle clicks

private:
    std::vector<float> m_samples;
    int m_currentSample = -1;

    std::function<void(float)> m_clickCallback; // Called with 0.0-1.0 fraction
};
