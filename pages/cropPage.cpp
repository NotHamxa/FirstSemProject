#include "cropPage.h"

CropPage::CropPage(QWidget* parent) : QWidget(parent) {

    player = new AudioPlayer(
        currentSample,
        [&](int val) { setCurrentSample(val); }
    );

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

    connect(startSlider, &QSlider::valueChanged,
            this, &CropPage::onStartSliderChanged);
    connect(endSlider, &QSlider::valueChanged,
            this, &CropPage::onEndSliderChanged);

    mainLayout->addWidget(startSlider);
    mainLayout->addWidget(endSlider);

    // Play button
    playButton = new QPushButton("Play", this);
    playButton->setFixedSize(120, 40);
    connect(playButton, &QPushButton::clicked,
            this, &CropPage::onPlayClicked);

    auto* playLayout = new QHBoxLayout;
    playLayout->addStretch();
    playLayout->addWidget(playButton);
    playLayout->addStretch();
    mainLayout->addLayout(playLayout);

    // Apply Crop
    applyButton = new QPushButton("Apply Crop", this);
    applyButton->setFixedHeight(40);
    connect(applyButton, &QPushButton::clicked,
            this, &CropPage::onApplyCrop);

    mainLayout->addWidget(applyButton);
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
    if (index >= endSample()) {
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
        setCurrentSample(startSample());
        player->play();
        playButton->setText("Pause");
    } else {
        player->pause();
        playButton->setText("Play");
    }
}

void CropPage::onStartSliderChanged(int v) {
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

    history->add(appData->audioData);

    auto& samples = appData->audioData.samples;
    samples.erase(samples.begin() + s, samples.begin() + e);

    changeWindow("main");
}
