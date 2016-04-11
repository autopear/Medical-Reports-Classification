#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStringList>

class QLabel;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QGroupBox;
class QListWidget;
class QListWidgetItem;
class QTextBrowser;
class QProgressBar;
class QSplitter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void selectDir();
    void onDoubleClicked(QListWidgetItem *item);
    void onReportSelected();
    void refreshStemedContent();
    void onChecked();
    void stem();
    void save();
    void onReporsListed(const QStringList &reports);
    void onReportStemmed(const QString &report, const QStringList &stems);
    void onFinished();

private:
    static QString formatPath(const QString &path);
    void saveSettings();
    void loadSettings();

    QLabel *m_labelDir;
    QLineEdit *m_editDir;
    QPushButton *m_btnDir;

    QListWidget *m_listFiles;
    QGroupBox *m_groupFiles;

    QTextBrowser *m_textFile;
    QGroupBox *m_groupFile;

    QTextBrowser *m_textStem;
    QGroupBox *m_groupStem;

    QSplitter *m_splitter;

    QProgressBar *m_bar;

    QCheckBox *m_checkSortStems;
    QCheckBox *m_checkUniqueStems;
    QCheckBox *m_checkUniqueReport;

    QPushButton *m_btnStem;
    QPushButton *m_btnSave;

    QWidget *m_main;

    QString m_lastSave;

    QMap<QString, QStringList> m_map;
};

#endif // MAINWINDOW_H
