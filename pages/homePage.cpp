#include "homePage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QFileDialog>
#include <QScrollArea>
#include <QWidget>
#include <QDir>
#include <QDebug>
#include "../util/readAudioFile.h"
#include <iostream>
#include<fstream>
#include "../structures/history.h"

QString openAudioFileDialog(QWidget *parent = nullptr) {
    QString filter = "Audio Files (*.mp3 *.wav *.flac *.ogg *.aac)";
    return QFileDialog::getOpenFileName(parent,
                                        "Select an audio file",
                                        QDir::homePath(),
                                        filter);
}

HomePage::HomePage(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setAlignment(Qt::AlignTop);

    // Title
    QFont titleFont("Segoe UI", 28, QFont::Bold);
    auto *title = new QLabel("Audio Editor");
    title->setFont(titleFont);
    title->setStyleSheet("color: #ffffff;");
    title->setAlignment(Qt::AlignCenter);
    title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    title->setMaximumHeight(50);

    mainLayout->addWidget(title);

    // Recent Projects
    auto *recentRow = new QHBoxLayout();
    recentRow->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont("Segoe UI", 20, QFont::Bold);
    auto *recentLabel = new QLabel("Recent Projects");
    recentLabel->setFont(sectionFont);
    recentLabel->setStyleSheet("color: #dddddd;");

    // New Project Button
    addButton = new QPushButton("New Project");
    addButton->setFixedSize(140, 36);
    addButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #1DB954;"
        "  color: white;"
        "  font-weight: bold;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover { background-color: #1ed760; }"
        "QPushButton:pressed { background-color: #1aa34a; }"
    );

    connect(addButton, &QPushButton::clicked, [this]() {
        QString file = openAudioFileDialog(this);
        if (!file.isEmpty()) {
            qDebug() << "Selected file:" << file;
            appData->currentAudioFile = file;
            std::string fileName = file.toStdString();
            try {
                addButton->setText("Loading...");
                appData->audioData.samples = AudioLoader::loadAudio(
                    fileName,
                    appData->audioData.sampleRate,
                    appData->audioData.channels
                );
                history->add(appData->audioData);
                addRecentProject(fileName);
                if (changeWindow) changeWindow("main");
            } catch (const std::exception& e) {
                std::cerr << e.what() << "\n";
            }
            addButton->setText("New Project");
        }
    });

    recentRow->addWidget(recentLabel);
    recentRow->addStretch();
    recentRow->addWidget(addButton);

    mainLayout->addLayout(recentRow);

    // Recent Projects Scroll
    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "  border: none;"
        "  border-radius: 6px;"
        "}"
    );
    auto *container = new QWidget();
    recentProjectsLayout = new QVBoxLayout(container);
    recentProjectsLayout->setSpacing(10);
    recentProjectsLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(container);
    scrollArea->setFixedHeight(250); // demo height
    mainLayout->addWidget(scrollArea);

    placeholderLabel = new QLabel("No recent projects yet.");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setStyleSheet("color: #888888; font-style: italic;");
    recentProjectsLayout->addWidget(placeholderLabel);
}

void HomePage::setData(AppData *data, AudioHistory *historyData) {
    appData = data;
    history = historyData;
    loadRecentProjects();
}

void HomePage::setChangeWindow(std::function<void(QString)> func) {
    changeWindow = std::move(func);
}


void HomePage::clearRecentProjects()
{
    QLayoutItem *item;
    while ((item = recentProjectsLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }
}
void HomePage::loadRecentProjects()
{
    clearRecentProjects();

    std::vector<std::string> projects;
    std::ifstream inFile("recentProjects.txt");

    if (!inFile.is_open()) {
        std::ofstream("recentProjects.txt").close();
        recentProjectsLayout->addWidget(placeholderLabel);
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty())
            projects.push_back(line);
    }

    if (projects.empty()) {
        recentProjectsLayout->addWidget(placeholderLabel);
        return;
    }

    for (const std::string &pathStr : projects) {
        QString path = QString::fromStdString(pathStr);

        if (!QFile::exists(path))
            continue;

        auto *item = new QPushButton(QFileInfo(path).fileName());
        item->setFixedHeight(44);
        item->setCursor(Qt::PointingHandCursor);

        item->setStyleSheet(
            "QPushButton {"
            "  text-align: left;"
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

        connect(item, &QPushButton::clicked, [this, pathStr]() {
            try {
                addButton->setText("Loading...");
                appData->currentAudioFile = QString::fromStdString(pathStr);
                appData->audioData.samples = AudioLoader::loadAudio(
                    pathStr,
                    appData->audioData.sampleRate,
                    appData->audioData.channels
                );
                history->add(appData->audioData);
                addRecentProject(pathStr); // bump to top
                if (changeWindow) changeWindow("main");
            } catch (const std::exception& e) {
                std::cerr << e.what() << "\n";
            }
            addButton->setText("New Project");
        });

        recentProjectsLayout->addWidget(item);
    }
}

void HomePage::addRecentProject(const std::string &path)
{
    std::vector<std::string> projects;
    {
        std::ifstream inFile("recentProjects.txt");
        std::string line;
        while (std::getline(inFile, line)) {
            if (!line.empty())
                projects.push_back(line);
        }
    }
    std::erase(projects, path);
    projects.insert(projects.begin(), path);
    constexpr size_t MAX_RECENT = 10;
    if (projects.size() > MAX_RECENT)
        projects.resize(MAX_RECENT);

    std::ofstream outFile("recentProjects.txt", std::ios::trunc);
    for (const auto &p : projects) {
        outFile << p << '\n';
    }
    outFile.close();
    loadRecentProjects();
}