#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSplitter;
class QTextBrowser;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void selectDir();
    void selectFileList();
    void loadFileList();
    void onCurrentItemChanged();
    void onItemDoubleClicked(QListWidgetItem *item);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);

private:
    QLabel *m_labelDir;
    QLineEdit *m_editDir;
    QPushButton *m_btnDir;
    QLabel *m_labelFile;
    QLineEdit *m_editFile;
    QPushButton *m_btnFile;
    QListWidget *m_list;
    QTextBrowser *m_browser;
    QSplitter *m_splitter;
    QPushButton *m_btnLoad;
    QPushButton *m_btnClose;
    QWidget *m_main;
};

#endif // MAINWINDOW_H
