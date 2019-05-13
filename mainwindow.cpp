#include "mainwindow.h"
#include <QtWidgets>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QMessageBox>

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurfaceDataArray>
#include <QtDataVisualization/QSurface3DSeries>

#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

#include "machine.h"

using namespace QtDataVisualization;

QT_CHARTS_USE_NAMESPACE


MainWindow::MainWindow(Machine *m, QWidget *parent) :
    QMainWindow(parent),
    workDir(QDir::currentPath()),  // workDir(tr("f:\\temp")),
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

    surfaceSeries = new QSurface3DSeries;
    surface->addSeries(surfaceSeries);

    QSize screenSize = surface->screen()->size();
    container->setMinimumSize(QSize(WINDOW_MIN_WIDTH, SURFACE_MIN_HEIGHT));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Build Chart

    chart->legend()->hide();
    chart->addSeries(chartSeries);
    int seriesLength = chartSeries->pointsVector().size();
    axisX->setTitleText("Text");
    axisY->setTitleText("Frequency");
    axisX->setRange(0, seriesLength);
    axisY->setRange(0, 1);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chartSeries->attachAxis(axisX);
    chartSeries->attachAxis(axisY);

    chartView->setMinimumSize(getViewWidth(seriesLength, WINDOW_MIN_WIDTH), CHARTVIEW_HEIGHT - CHARTVIEW_FIELD_Y);
    chartView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    chartView->setRenderHint(QPainter::Antialiasing);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    scrollArea->setWidget(chartView);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumSize(WINDOW_MIN_WIDTH, CHARTVIEW_HEIGHT);

    // Build layout

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *vLayout = new QVBoxLayout(centralWidget);
    vLayout->addWidget(scrollArea);
    vLayout->addWidget(container);
    setCentralWidget(centralWidget);

    CreateMenus();
}


int MainWindow::getViewWidth(int seriesLen, int currWidth)
{
    if (seriesLen < currWidth) {
        return currWidth - CHARTVIEW_FIELD_X;
    } else {
        return seriesLen * 2;
    }
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
                                "Text (*.txt);;");

    QFileInfo fileInfo(fileName);
    workDir = fileInfo.path();

    if (!fileName.isEmpty())
    {
        if (machine->loadFile(fileName))
        {
            surfaceSeries->dataProxy()->resetArray(machine->getSurfaceDataArray());

            chartSeries->replace(machine->getChartSeriesData());
            int seriesLength = chartSeries->pointsVector().size();
            axisX->setRange(0, seriesLength);
            axisY->setRange(0, machine->getMaxHeight());

            int width = getViewWidth(seriesLength, WINDOW_MIN_WIDTH);
            chartView->setMinimumSize(width, CHARTVIEW_HEIGHT - CHARTVIEW_FIELD_Y);
            chartView->resize( width, chartView->height() - CHARTVIEW_FIELD_Y );
        }
    }
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About", "Программа дает визуальное представление частоты "
       "символов в загруженном текстовом файле (его части, установленного размера). \n"
       "Используется обратная зависимость высоты графика от частоты, таким образом редко "
       "используемые символы образуют пики.\n"
       "В программе используется таблица частотности букв русского языка (по НКРЯ).");
}
