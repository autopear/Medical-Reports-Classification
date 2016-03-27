#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QSplitter;
class QPushButton;
class FilePicker;
class TextBrowser;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onFileChanged(const QString &file);
    void saveResult();

private:
    void updateResult();
    void loadSettings();
    void saveSettings();

    FilePicker *m_picker1;
    FilePicker *m_picker2;
    TextBrowser *m_browser1;
    TextBrowser *m_browser2;
    TextBrowser *m_browser3;
    QSplitter *m_splitter;
    QPushButton *m_btnSave;
    QPushButton *m_btnClose;
    QWidget *m_widget;

    QString m_last;
};

#endif // MAINWINDOW_H
