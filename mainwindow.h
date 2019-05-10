#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "machine.h"
#include <QtDataVisualization/QSurface3DSeries>

using namespace QtDataVisualization;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Machine *m, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_actionOpen_file_triggered();
    void on_actionAbout_triggered();

private:
    void CreateMenus();

    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *openAct;
    QAction *exitAct;
    QAction *aboutAct;

    QString workDir;
    Machine *machine;
    QSurface3DSeries *series;

};

#endif // MAINWINDOW_H
