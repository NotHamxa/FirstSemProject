#pragma once
#include <QMainWindow>
#include <QStackedWidget>

#include "structures/appData.h"
#include "structures/history.h"

class HomePage;
class MainPage;
class EqPage;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void switchWindow(QString);


private:
    QStackedWidget *stack;
    HomePage *homePage;
    MainPage *mainPage;
    EqPage *eqPage;
    AppData *appData;
    AudioHistory *history;
};

