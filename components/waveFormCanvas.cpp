#include "WaveformCanvas.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <algorithm>
#include <iostream>

WaveformCanvas::WaveformCanvas(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(400);
    setAutoFillBackground(true);
}

void WaveformCanvas::setSamples(const std::vector<float> &samples) {
    m_samples = samples;
    update();
}

void WaveformCanvas::clearSamples() {
    m_samples.clear();
    update();
}

void WaveformCanvas::setCurrentSample(int index) {
    m_currentSample = index;
    update();
}

void WaveformCanvas::setClickCallback(std::function<void(float)> callback) {
    m_clickCallback = callback;
}

void WaveformCanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(30, 30, 30));
    painter.setPen(QPen(QColor(0, 200, 255), 1));

    if (m_samples.empty())
        return;

    int w = width();
    int h = height();
    int sampleCount = m_samples.size();

    float samplesPerPixel = (float)sampleCount / w;
    float midY = h / 2.0f;

    // Draw waveform
    for (int x = 0; x < w; x++) {
        int sampleIndex = (int)(x * samplesPerPixel);
        float s = std::clamp(m_samples[sampleIndex], -1.0f, 1.0f);

        float y1 = midY - s * (h / 2.0f);
        float y2 = midY + s * (h / 2.0f);

        painter.drawLine(x, y1, x, y2);
    }

    // Draw playhead line
    if (m_currentSample >= 0 && m_currentSample < sampleCount) {
        float x = (float)m_currentSample / samplesPerPixel;
        painter.setPen(QPen(Qt::red, 2));
        painter.drawLine(x, 0, x, h);
    }
}

// --- Handle mouse clicks ---
void WaveformCanvas::mousePressEvent(QMouseEvent* event) {
    if (!m_clickCallback)
        return;

    float fraction = static_cast<float>(event->pos().x()) / width();
    fraction = std::clamp(fraction, 0.0f, 1.0f);
    m_clickCallback(fraction);
}
