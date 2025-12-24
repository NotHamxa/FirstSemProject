#include "mainPage.h"

#include <iostream>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QSizePolicy>
QString getFileName(QWidget *parent = nullptr) {
    QString name = QFileDialog::getSaveFileName(
        parent,
        "Save Audio File",
        "",
        "WAV Files (*.wav)"
    );




















































































    return name;
}
MainPage::MainPage(QWidget *parent) : QWidget(parent) {

    player = new AudioPlayer(currentSample,[&](int val){setCurrentSample(val);});

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // --- Menu Bar ---
    menuBar = new QMenuBar(this);
    menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    menuBar->setStyleSheet(
        "QMenuBar {"
        "  background-color: #1f1f1f;"     /* dark background */
        "  color: #f0f0f0;"                /* normal text color */
        "  font-size: 14px;"
        "  spacing: 6px;"                  /* spacing between menus */
        "}"
        "QMenuBar::item {"
        "  background-color: transparent;"
        "  padding: 4px 12px;"
        "  border-radius: 4px;"
        "}"
        "QMenuBar::item:selected {"
        "  background-color: #2a2a2a;"    /* hover highlight */
        "}"
        "QMenuBar::item:disabled {"
        "  color: #555555;"                /* gray text for disabled actions */
        "}"
        "QMenu {"
        "  background-color: #1f1f1f;"
        "  color: #f0f0f0;"
        "  border: 1px solid #2a2a2a;"
        "}"
        "QMenu::item:selected {"
        "  background-color: #4CAF50;"     /* highlight for selected menu item */
        "  color: #ffffff;"
        "}"
        "QMenu::item:disabled {"
        "  color: #555555;"                /* gray for disabled menu items */
        "}"
    );


    // File Menu
    fileMenu = menuBar->addMenu("File");
    saveAction = new QAction("Save", this);
    connect(saveAction, &QAction::triggered, this, &MainPage::saveFile);
    fileMenu->addAction(saveAction);

    closeAction = new QAction("Close", this);
    connect(closeAction, &QAction::triggered, this, &MainPage::closeFile);
    fileMenu->addAction(closeAction);

    exitAction = new QAction("Exit", this);
    exitAction->setObjectName("exitApplication");
    fileMenu->addAction(exitAction);

    // Edit Menu
    eqActionItem = new QAction("Eq", this);
    connect(eqActionItem, &QAction::triggered, this, &MainPage::eqAction);
    menuBar->addAction(eqActionItem);

    editActionItem = new QAction("Crop", this);
    connect(editActionItem, &QAction::triggered, this, &MainPage::editAction);
    menuBar->addAction(editActionItem);

    // --- Top-Level Actions ---
    undoAction = new QAction("Undo", this);
    undoAction->setDisabled(true);
    connect(undoAction, &QAction::triggered, this, &MainPage::onUndo);
    menuBar->addAction(undoAction);

    redoAction = new QAction("Redo", this);
    redoAction->setDisabled(true);
    connect(redoAction, &QAction::triggered, this, &MainPage::onRedo);
    menuBar->addAction(redoAction);


    mainLayout->setMenuBar(menuBar);

    // --- Title ---
    titleLabel = new QLabel("Audio Viewer", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    titleLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    titleLabel->setStyleSheet(
        "font-size: 22px;"
        "font-weight: bold;"
        "max-height: 100px;"
        "color: #ffffff;"
    );
    mainLayout->addWidget(titleLabel);

    // --- Waveform Canvas ---
    canvas = new WaveformCanvas(this);
    canvas->setClickCallback([&](float val){handleUserClick(val);});
    canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(canvas);

    // --- Current Time Label ---
    currentTimeLabel = new QLabel("0:0 - 0:0", this);
    currentTimeLabel->setAlignment(Qt::AlignCenter);
    currentTimeLabel->setStyleSheet("font-size: 14px; color: gray;");
    mainLayout->addWidget(currentTimeLabel);

    // --- Play Button ---
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
    connect(playButton, &QPushButton::clicked, this, [&]() {
        playing = !playing;
        if (playing) {
            player->play();
            playButton->setText("Pause");
        } else {
            player->pause();
            playButton->setText("Play");
        }
    });

    // Center the button
    auto* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(playButton);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);
}


void MainPage::saveFile() {
    QString fileName = getFileName(this);

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".wav", Qt::CaseInsensitive))
        fileName += ".wav";

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    const int bitsPerSample = 16;
    const int byteRate = appData->audioData.sampleRate * appData->audioData.channels * bitsPerSample / 8;
    const int blockAlign = appData->audioData.channels * bitsPerSample / 8;
    const int dataSize = static_cast<int>(appData->audioData.samples.size() * sizeof(int16_t));
    const int chunkSize = 36 + dataSize;

    // ===== WAV HEADER =====
    out.writeRawData("RIFF", 4);
    out << quint32(chunkSize);
    out.writeRawData("WAVE", 4);

    // fmt subchunk
    out.writeRawData("fmt ", 4);
    out << quint32(16);
    out << quint16(1);
    out << quint16(appData->audioData.channels);
    out << quint32(appData->audioData.sampleRate);
    out << quint32(byteRate);
    out << quint16(blockAlign);
    out << quint16(bitsPerSample);

    // data subchunk
    out.writeRawData("data", 4);
    out << quint32(dataSize);

    // ===== AUDIO DATA =====
    std::vector<int16_t> pcmData(appData->audioData.samples.size());
    for (size_t i = 0; i < appData->audioData.samples.size(); ++i) {
        float sample = std::clamp(appData->audioData.samples[i], -1.0f, 1.0f);
        pcmData[i] = static_cast<int16_t>(sample * 32767.0f);
    }
    out.writeRawData(reinterpret_cast<const char*>(pcmData.data()), dataSize);
    file.close();
    QMessageBox::information(this, "Saved", "File successfully saved");

}
void MainPage::closeFile() {
    history->clear();
    changeWindow("home");
}
void MainPage::eqAction() {
    changeWindow("eq");
}
void MainPage::editAction() {
    changeWindow("crop");
}
void MainPage::onUndo() {
    if (!history->canUndo())
        return;
    appData->audioData = history->undo();
    canvas->setSamples(appData->audioData.samples);
    updatePage();
}
void MainPage::onRedo() {
    if (!history->canRedo())
        return;
    appData->audioData = history->redo();
    canvas->setSamples(appData->audioData.samples);
    updatePage();
}

// --- Rest of your existing functions ---
void MainPage::setData(AppData *data, AudioHistory *audioHistory) {
    appData = data;
    history = audioHistory;
}
void MainPage::setChangeWindow(std::function<void(QString)> func) { changeWindow = std::move(func); }
void MainPage::redraw() { canvas->update(); }
void MainPage::updatePage() {
    currentSample = 0;
    auto array = appData->currentAudioFile.split("/");
    titleLabel->setText(array.last());
    std::cout <<"Title" <<array.last().toStdString() << std::endl;
    int time = appData->audioData.samples.size()/(appData->audioData.sampleRate * appData->audioData.channels);
    int minutes = time/60;
    int seconds = time%60;
    currentTimeLabel->setText(QString("0:0 - %1:%2").arg(minutes).arg(seconds));
    canvas->update();
    player->setAudio(&appData->audioData);
    redoAction->setDisabled(!history->canRedo());
    undoAction->setDisabled(!history->canUndo());
}
void MainPage::setCurrentSample(int index) {
    if (index >= appData->audioData.samples.size()) {
        playing = false;
        player->pause();
        playButton->setText("Play");
        index = 0;
    }
    int time = appData->audioData.samples.size()/(appData->audioData.sampleRate * appData->audioData.channels);
    int minutes = time/60;
    int seconds = time%60;

    int currentTime = index/(appData->audioData.sampleRate * appData->audioData.channels);
    int currentSeconds = currentTime%60;
    int currentMinutes = currentTime/60;

    currentTimeLabel->setText(QString("%1:%2 - %3:%4").arg(currentMinutes).arg(currentSeconds).arg(minutes).arg(seconds));

    currentSample = index;
    canvas->setCurrentSample(index);
    canvas->update();
}
void MainPage::handleUserClick(float val) {
    playing = false;
    player->pause();
    playButton->setText("Play");
    setCurrentSample(static_cast<int>(appData->audioData.samples.size() * val));
}
