#pragma once
#include <QLabel>
#include <QWidget>
#include<QPushButton>
#include<QVBoxLayout>

#include "../structures/appData.h"
#include "../structures/history.h"

class HomePage : public QWidget {
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);
    void setData(AppData*,AudioHistory*);
    void setChangeWindow(std::function<void(QString)> func);
    void loadRecentProjects();
private:
    AppData *appData = nullptr;
    AudioHistory *history = nullptr;
    std::function<void(QString)> changeWindow;
    QVBoxLayout *recentProjectsLayout;
    QPushButton *changeWindowButton;
    QLabel *placeholderLabel = nullptr;
    QPushButton *addButton;

    void clearRecentProjects();
    void addRecentProject(const std::string &path);

};
