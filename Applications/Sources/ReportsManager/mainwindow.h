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
    void loadSettings();
    void saveSettings();

    QLabel *m_labelXml;
    QLineEdit *m_editXml;
    QPushButton *m_btnOpen;
    QPushButton *m_btnSave;

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
};

#endif // MAINWINDOW_H