#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QStringList>

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QProgressBar;
class QPushButton;
class QSplitter;
class QTextBrowser;
class QTextEdit;

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
    void saveReports();
    void saveWords();
    void exportToText();
    void exportToFiles();
    void exportToSVM();
    void exportToLDA();
    void onReporsListed(const QStringList &reports);
    void onReportStemmed(const QString &report, const QStringList &stems);
    void onFinished();
    void importNegative();
    void importNeutral();
    void importPositive();

    friend class DocumentsLoader;
    friend class ExportSVMDialog;
private:
    static QString formatPath(const QString &path);
    void importFile(int sign);
    void saveSettings();
    void loadSettings();

    QLabel *m_labelDir;
    QLineEdit *m_editDir;
    QPushButton *m_btnDir;

    QAction *m_actionNegative;
    QAction *m_actionNeutral;
    QAction *m_actionPositive;
    QMenu *m_menuImport;

    QPushButton *m_btnImport;

    QListWidget *m_listFiles;
    QGroupBox *m_groupFiles;

    QTextBrowser *m_textFile;
    QGroupBox *m_groupFile;

    QTextBrowser *m_textStem;
    QGroupBox *m_groupStem;

    QSplitter *m_splitter1;

    QTextEdit *m_textIgnoreWord;
    QGroupBox *m_groupIgnoreWord;

    QTextEdit *m_textIgnoreSuffix;
    QGroupBox *m_groupIgnoreSuffix;

    QSplitter *m_splitter2;

    QSplitter *m_splitter3;

    QProgressBar *m_bar;

    QCheckBox *m_checkSortWords;
    QCheckBox *m_checkUniqueWords;
    QCheckBox *m_checkUniqueReport;

    QPushButton *m_btnStem;    
    QPushButton *m_btnSave;
    QPushButton *m_btnExport;

    QAction *m_actionText;
    QAction *m_actionFiles;
    QAction *m_actionSVM;
    QAction *m_actionLDA;
    QMenu *m_menuExport;

    QAction *m_actionReports;
    QAction *m_actionWords;
    QMenu *m_menuSave;

    QWidget *m_main;

    QString m_lastReports;
    QString m_lastSaveWords;
    QString m_lastText;
    QString m_lastFiles;
    QString m_lastLDA;
    QString m_lastOpenWords;
    QString m_lastImport;

    QMap<QString, QStringList> m_map;
};

#endif // MAINWINDOW_H
