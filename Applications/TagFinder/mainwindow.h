#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListView;
class QMutex;
class QPushButton;
class QSplitter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void selectDir(); //Open file dialog to select a directory to be searched in
    void exportList(); //Export selected list
    void search(); //Start search in directory
    void onDoubleClicked(const QModelIndex &index); //Copy full path of the selected file to clipboard

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);

private:
    void processDirectory(const QString &dir); //Scan directory recursively and analyze txt files
    void refreshLists(); //Refresh all available list views
    QString formatPath(const QString &path); //Remove directory prefix, display and save relative path only

    QWidget *m_main;

    QLabel *m_labelDir;
    QLineEdit *m_editDir;
    QPushButton *m_btnDir;

    QLabel *m_labelTag;
    QLineEdit *m_editTag;
    QCheckBox *m_checkTag;

    QLabel *m_labelText;
    QLineEdit *m_editText;
    QCheckBox *m_checkText;

    QLabel *m_labelExport;
    QCheckBox *m_checkExport;

    QSplitter *m_splitter;

    QGroupBox *m_groupLeft;
    QListView *m_listLeft;
    QPushButton *m_btnLeft;

    QGroupBox *m_groupMiddle;
    QListView *m_listMiddle;
    QPushButton *m_btnMiddle;

    QGroupBox *m_groupRight;
    QListView *m_listRight;
    QPushButton *m_btnRight;

    QPushButton *m_button;

    int m_timerId;
    QStringList m_listTagAndText; //Buffer for files containing both tag and text (if not empty)
    QStringList m_listTagOnly; //Buffer for files containing only tag but not text (not available for empty text)
    QStringList m_listOthers; //Buffer for files not containing tag

    QMutex *m_mutex;
};

class StringListModel : public QStringListModel
{
public:
    void append(const QString &string) {
        insertRows(rowCount(), 1);
        setData(index(rowCount() - 1), string);
    }
    StringListModel &operator<<(const QString &string){
        append(string);
        return *this;
    }
};

#endif // MAINWINDOW_H
