#include "cropPage.h"

#include <iostream>

CropPage::CropPage(QWidget* parent) : QWidget(parent) {

    player = new AudioPlayer(currentSample,[&](int val){setCurrentSample(val);});

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto* title = new QLabel("Crop Audio", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold;");
    mainLayout->addWidget(title);

    // Waveform
    canvas = new WaveformCanvas(this);
    canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(canvas);

    // Range label
    rangeLabel = new QLabel("0% - 100%", this);
    rangeLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(rangeLabel);

    // Sliders
    startSlider = new QSlider(Qt::Horizontal, this);
    endSlider   = new QSlider(Qt::Horizontal, this);

    startSlider->setRange(0, 1000);
    endSlider->setRange(0, 1000);

    startSlider->setValue(0);
    endSlider->setValue(1000);

    // Modern slider style
    QString sliderStyle =
        "QSlider::groove:horizontal {"
        "  height: 8px;"
        "  background: #2a2a2a;"
        "  border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #4CAF50;"
        "  border: 1px solid #3a3a3a;"
        "  width: 16px;"
        "  height: 16px;"
        "  margin: -4px 0;"
        "  border-radius: 8px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: #4CAF50;"
        "  border-radius: 4px;"
        "}"
        "QSlider::add-page:horizontal {"
        "  background: #1f1f1f;"
        "  border-radius: 4px;"
        "}";
    startSlider->setStyleSheet(sliderStyle);
    endSlider->setStyleSheet(sliderStyle);

    connect(startSlider, &QSlider::valueChanged,
            this, &CropPage::onStartSliderChanged);
    connect(endSlider, &QSlider::valueChanged,
            this, &CropPage::onEndSliderChanged);

    auto *cropStartLabel = new QLabel("Crop Start Point",this);
    auto *cropEndLabel = new QLabel("Crop End Point",this);

    mainLayout->addWidget(cropStartLabel);
    mainLayout->addWidget(startSlider);
    mainLayout->addWidget(cropEndLabel);
    mainLayout->addWidget(endSlider);

    // Play button
    playButton = new QPushButton("Play", this);
    playButton->setFixedSize(120, 40);
    playButton->setStyleSheet(
    "QPushButton {"
    "  font-size: 16px;"
    "  font-weight: bold;"
    "  background-color: #4CAF50;"
    "  color: white;"
    "  border-radius: 8px;"
    "}"
    "QPushButton:hover {"
    "  background-color: #45a049;"
    "}"
    "QPushButton:pressed {"
    "  background-color: #3e8e41;"
    "}"
    );
    connect(playButton, &QPushButton::clicked,
            this, &CropPage::onPlayClicked);

    auto* playLayout = new QHBoxLayout;
    playLayout->addStretch();
    playLayout->addWidget(playButton);
    playLayout->addStretch();
    mainLayout->addLayout(playLayout);

    // Bottom buttons layout (Apply & Back)
    auto* btnLayout = new QHBoxLayout;

    applyButton = new QPushButton("Apply Crop", this);
    applyButton->setFixedHeight(40);
    applyButton->setStyleSheet(
        "QPushButton {"
        "  text-align: center;"
        "  padding: 10px 14px;"
        "  background-color: #1f1f1f;"
        "  color: #f0f0f0;"
        "  border: 1px solid #2a2a2a;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2a2a2a;"
        "  border-color: #3a3a3a;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #181818;"
        "}"
    );
    connect(applyButton, &QPushButton::clicked,
            this, &CropPage::onApplyCrop);

    auto* backButton = new QPushButton("Back", this);
    backButton->setFixedHeight(40);
    backButton->setStyleSheet(
        "QPushButton {"
        "  text-align: center;"
        "  padding: 10px 14px;"
        "  background-color: #1f1f1f;"
        "  color: #f0f0f0;"
        "  border: 1px solid #2a2a2a;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2a2a2a;"
        "  border-color: #3a3a3a;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #181818;"
        "}"
    );
    connect(backButton, &QPushButton::clicked,
            this, &CropPage::back);
    btnLayout->addWidget(backButton);
    btnLayout->addWidget(applyButton);
    mainLayout->addLayout(btnLayout);
}
void CropPage::setData(AppData* data, AudioHistory* h) {
    appData = data;
    history = h;
}

void CropPage::setChangeWindow(std::function<void(QString)> func) {
    changeWindow = std::move(func);
}

void CropPage::updatePage() {
    currentSample = startSample();
    canvas->setSamples(appData->audioData.samples);
    canvas->setCurrentSample(currentSample);
    canvas->update();
    player->setAudio(&appData->audioData);
}

int CropPage::startSample() const {
    return static_cast<int>(
        appData->audioData.samples.size() *
        (startSlider->value() / 1000.0f)
    );
}

int CropPage::endSample() const {
    return static_cast<int>(
        appData->audioData.samples.size() *
        (endSlider->value() / 1000.0f)
    );
}

void CropPage::setCurrentSample(int index) {
    std::cout << index << std::endl;
    if (index >= endSample()) {
        std::cout << "Paused" << std::endl;
        playing = false;
        player->pause();
        playButton->setText("Play");
        currentSample = startSample();
    } else {
        currentSample = index;
    }
    canvas->setCurrentSample(currentSample);
    canvas->update();
}

void CropPage::onPlayClicked() {
    playing = !playing;
    if (playing) {
        player->play();
        playButton->setText("Pause");
    } else {
        player->pause();
        playButton->setText("Play");
    }
}

void CropPage::onStartSliderChanged(int v) {
    if (playing) {
        player->pause();
        playing= false;
        playButton->setText("Play");
    }
    if (v > endSlider->value())
        endSlider->setValue(v);

    rangeLabel->setText(
        QString("%1% - %2%")
            .arg(startSlider->value() / 10.0f)
            .arg(endSlider->value() / 10.0f)
    );

    setCurrentSample(startSample());
}

void CropPage::onEndSliderChanged(int v) {
    if (v < startSlider->value())
        startSlider->setValue(v);

    rangeLabel->setText(
        QString("%1% - %2%")
            .arg(startSlider->value() / 10.0f)
            .arg(endSlider->value() / 10.0f)
    );
}

void CropPage::onApplyCrop() {
    int s = startSample();
    int e = endSample();

    if (e <= s)
        return;

    auto& samples = appData->audioData.samples;
    samples.erase(samples.begin() + s, samples.begin() + e);
    history->add(appData->audioData);

    // Clean up
    currentSample = 0;
    startSlider->setValue(0);
    endSlider->setValue(1000);

    changeWindow("main");
}
void CropPage::back() {
    // Clean up
    currentSample = 0;
    startSlider->setValue(0);
    endSlider->setValue(1000);

    changeWindow("main");
}
