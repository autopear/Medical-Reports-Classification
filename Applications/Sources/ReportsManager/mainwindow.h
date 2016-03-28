#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "report.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSplitter;
class PropertyListWidget;
class QListWidget;
class QTextEdit;
class QCheckBox;
class QListWidgetItem;

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
    void selectXml();
    void saveXml();
    void importFiles();
    void onXmlChanged();
    void onStateChanged();
    void loadXml();
    void onReportSelected(QListWidgetItem *current, QListWidgetItem *previous);
    void onPropertySelected(const QString &prop);
    void onPropertyChanged(const QString &oldProp, const QString &newProp);
    void onPropertyDeleted(const QString &prop);
    void onPropertyAdded(const QString &prop);
    void onValueEdited();

private:
    void importAndUpdate(const QStringList &files);
    void loadSettings();
    void saveSettings();
    static QString formatPath(const QString &path);

    QLabel *m_labelXml;
    QLineEdit *m_editXml;
    QPushButton *m_btnOpen;
    QPushButton *m_btnSave;
    QPushButton *m_btnImport;

    QListWidget *m_listReports;

    QGroupBox *m_groupReports;

    QCheckBox *m_checkState;

    QLabel *m_labelState;

    PropertyListWidget *m_listProps;

    QGroupBox *m_groupProps;

    QTextEdit *m_editValue;

    QGroupBox *m_groupValue;

    QSplitter *m_splitter;

    QWidget *m_main;

    QMap<QString, Report> m_reports;

    Report m_currentReport;

    QString m_importDir;
};

#endif // MAINWINDOW_H
