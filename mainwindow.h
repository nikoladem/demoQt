#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtCharts>

#include "machine.h"

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
    inline int getViewWidth(int seriesLen, int currWidth);

    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *openAct;
    QAction *exitAct;
    QAction *aboutAct;

    QString workDir;
    Machine *machine;
    QSurface3DSeries *surfaceSeries;

    QChart *chart;
    QLineSeries *chartSeries ;
    QChartView *chartView;
    QValueAxis *axisX;
    QValueAxis *axisY;

    const int CHARTVIEW_HEIGHT = 200;
    const int CHARTVIEW_FIELD_X = 30;
    const int CHARTVIEW_FIELD_Y = 20;
    const int WINDOW_MIN_WIDTH = 600;

};

#endif // MAINWINDOW_H
