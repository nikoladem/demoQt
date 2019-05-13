#include "mainwindow.h"
#include <QtWidgets>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QMessageBox>

#include <QtGui/QScreen>

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurfaceDataArray>
//#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>

#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

#include "machine.h"

using namespace QtDataVisualization;

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(Machine *m, QWidget *parent) :
    QMainWindow(parent),
    workDir(tr("f:\\temp")), // workDir(QDir::currentPath()),  - TODO: вернуть в релизе QDir::currentPath
    machine(m),
    chart(new QChart),
    chartSeries(new QLineSeries),
    chartView(new QChartView(chart)),
    axisX(new QValueAxis),
    axisY(new QValueAxis)
{
    // Build Q3DSurface

    Q3DSurface *surface = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(surface);
    surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);

//    QSurfaceDataArray *data = new QSurfaceDataArray;
    surfaceSeries = new QSurface3DSeries;
//    surfaceSeries->dataProxy()->resetArray(data);
    surface->addSeries(surfaceSeries);

    QSize screenSize = surface->screen()->size();
    const int xSize = screenSize.width() / 2;
    container->setMinimumSize(QSize(xSize, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Build Chart

    *chartSeries << QPointF(0, 7) << QPointF(1, 3) << QPointF(2, 6) << QPointF(3, 2) << QPointF(4, 4);
    *chartSeries << QPointF(5, 1) << QPointF(6, 3) << QPointF(7, 0) << QPointF(8, 3) << QPointF(9, 2);

    chart->legend()->hide();
    chart->addSeries(chartSeries);
    axisX->setRange(0, chartSeries->pointsVector().size());
    axisY->setRange(0, 15);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chartSeries->attachAxis(axisX);
    chartSeries->attachAxis(axisY);

//    chart->setMinimumWidth(1200);
//    chart->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

//    QChartView *chartView = new QChartView(chart);

    chartView->setMinimumSize(/*xSize*/800, 200);
    chartView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    chartView->setRenderHint(QPainter::Antialiasing);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    scrollArea->setWidget(chartView);
//    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumSize(/*xSize*/800, 200);

    // Build layout

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *vLayout = new QVBoxLayout(centralWidget);
    vLayout->addWidget(scrollArea);
    vLayout->addWidget(container);
    setCentralWidget(centralWidget);

    CreateMenus();
}


MainWindow::~MainWindow()
{
}


void MainWindow::CreateMenus()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::on_actionOpen_file_triggered);

    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setShortcut(QKeySequence::HelpContents);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::on_actionAbout_triggered);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionOpen_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                workDir,
                                "Text (*.txt);;");  // "Text (*.txt);;All files (*.*)");

    QFileInfo fileInfo(fileName);
    workDir = fileInfo.path();

    if (!fileName.isEmpty())
    {
        if (machine->loadFile(fileName))
        {
            surfaceSeries->dataProxy()->resetArray(machine->getSurfaceDataArray());

            chartSeries->replace(machine->getChartSeriesData());
            axisX->setRange(0, chartSeries->pointsVector().size());
            axisY->setRange(0, machine->getMaxHeight());

            chartView->resize(1200, chartView->height());
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About", "Описание программы");  //TODO: доделать
}
