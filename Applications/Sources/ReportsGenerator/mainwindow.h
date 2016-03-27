#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ReportGenerator;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    ReportGenerator *m_generator;
};

#endif // MAINWINDOW_H
