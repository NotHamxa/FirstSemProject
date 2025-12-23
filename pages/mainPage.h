#pragma once
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

#include "../structures/appData.h"
#include "../components/WaveformCanvas.h"
#include "../structures/history.h"
#include "../util/audioPlayer.h"

class MainPage : public QWidget {
    Q_OBJECT
public:
    explicit MainPage(QWidget *parent = nullptr);
    void setData(AppData*,AudioHistory*);
    void setChangeWindow(std::function<void(QString)> func);
    void updatePage();
    void redraw();
    WaveformCanvas *canvas;
    void setCurrentSample(int);
    void handleUserClick(float val);

private slots:
    // Demo menu functions
    void saveFile();
    void closeFile();
    void eqAction();
    void editAction();
    void onUndo();
    void onRedo();

private:
    AppData *appData = nullptr;
    AudioHistory *history = nullptr;

    AudioPlayer* player = nullptr;
    std::function<void(QString)> changeWindow;

    QVBoxLayout *mainLayout;
    QMenuBar *menuBar;
    QMenu *fileMenu;

    QAction *saveAction;
    QAction *closeAction;
    QAction *exitAction;
    QAction *eqActionItem;
    QAction *editActionItem;
    QAction *undoAction;
    QAction *redoAction;

    QLabel *titleLabel;
    QPushButton *playButton;
    QLabel *currentTimeLabel;

    int currentSample=0;
    bool playing=false;
};
