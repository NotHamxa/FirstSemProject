#include "mainwindow.h"
#include "pages/homePage.h"
#include "pages/mainPage.h"
#include "pages/eqPage.h"
#include "pages/cropPage.h"

void switchToMain(QString window);
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Global Variables
    appData = new AppData();
    history = new AudioHistory();

    stack = new QStackedWidget(this);

    homePage = new HomePage();
    mainPage = new MainPage();
    eqPage = new EqPage();
    cropPage = new CropPage();

    // Setting App Data
    homePage->setData(appData, history);
    mainPage->setData(appData, history);
    eqPage->setData(appData, history);
    cropPage->setData(appData, history);

    // Setting switch window function
    homePage->setChangeWindow([this](QString window){switchWindow(window);});
    mainPage->setChangeWindow([this](QString window){switchWindow(window);});
    eqPage->setChangeWindow([this](QString window){switchWindow(window);});
    cropPage->setChangeWindow([this](QString window){switchWindow(window);});

    // Adding pages to stack
    stack->addWidget(homePage);
    stack->addWidget(mainPage);
    stack->addWidget(eqPage);
    stack->addWidget(cropPage);

    setCentralWidget(stack);

}
void MainWindow::switchWindow(QString window) {
    if (window=="home") {
        stack->setCurrentIndex(0);
        homePage->loadRecentProjects();
    }
    else if (window=="main") {
        mainPage->canvas->setSamples(appData->audioData.samples);
        stack->setCurrentIndex(1);
        mainPage->updatePage();
        eqPage->setData(appData,history);
    }
    else if (window=="eq") {
        stack->setCurrentIndex(2);
        eqPage->setData(appData,history);
        eqPage->modifiedSamples = appData->audioData.samples;
        eqPage->canvas->setSamples(eqPage->modifiedSamples);
    }
    else if (window=="crop") {
        cropPage->setData(appData,history);
        cropPage->updatePage();
        stack->setCurrentIndex(3);
    }
}
