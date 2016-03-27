#include <QCloseEvent>
#include <QMessageBox>
#include "reportgenerator.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_generator = new ReportGenerator(this);

    setCentralWidget(m_generator);

    setWindowTitle(tr("Report Database Generator"));
}

MainWindow::~MainWindow()
{
    delete m_generator;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_generator->isRunning())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("You cannot close the main window while the application is processing reports."));
        event->ignore();
    }
    else
        QMainWindow::closeEvent(event);
}
