#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSplitter;
class TagListWidget;
class VariantListWidget;

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
    void selectFile();
    void saveFile();
    void onTagChanged(const QString &oldTag, const QString &newTag);
    void onTagDeleted(const QString &tag);
    void onTagAdded(const QString &tag);
    void onCurrentTagChanged(const QString &tag);
    void onVariantsChanged();
    bool loadFile();

private:
    QLabel *m_labelFile;
    QLineEdit *m_editFile;
    QPushButton *m_btnFile;

    TagListWidget *m_listTags;
    QGroupBox *m_groupTags;

    VariantListWidget *m_listVars;
    QGroupBox *m_groupVars;

    QSplitter *m_splitter;

    QPushButton *m_btnSave;
    QPushButton *m_btnClose;

    QWidget *m_main;

    QMap<QString, QStringList> m_tags;
};

#endif // MAINWINDOW_H
