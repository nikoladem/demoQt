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
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>

#include "machine.h"

using namespace QtDataVisualization;

MainWindow::MainWindow(Machine *m, QWidget *parent) :
    QMainWindow(parent),
    workDir(tr("f:\\temp")), // workDir(QDir::currentPath()),
    machine(m)
{
    Q3DSurface *surface = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(surface);
    setCentralWidget(container);

    surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);

    QSurfaceDataArray *data = new QSurfaceDataArray;
    series = new QSurface3DSeries;
    series->dataProxy()->resetArray(data);
    surface->addSeries(series);

    QSize screenSize = surface->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

    qDebug() << fileName;

    if (!fileName.isEmpty())
    {
        QSurfaceDataArray *data = machine->loadFile(fileName);
        if (data)
        {
            series->dataProxy()->resetArray(data);
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About", "Описание программы");  //TODO: доделать
}
