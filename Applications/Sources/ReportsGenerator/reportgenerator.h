#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QWidget>

class ElidibleLabel;
class QLabel;
class QLineEdit;
class QListWidget;
class QProgressBar;
class QPushButton;

class ReportGenerator : public QWidget
{
    Q_OBJECT
public:
    explicit ReportGenerator(QWidget *parent = 0);
    ~ReportGenerator();

    bool isRunning() const;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void selectDir();
    void selectTags();
    void selectXml();
    void start();
    void onReportParsed(int index);

private:
    static void findAllReports(const QString &dir, QStringList *reports);

    void loadSettings();
    void saveSettings();

    QLabel *m_labelDir;
    QLineEdit *m_editDir;
    QPushButton *m_btnDir;

    QLabel *m_labelTags;
    QLineEdit *m_editTags;
    QPushButton *m_btnTags;

    QLabel *m_labelXml;
    QLineEdit *m_editXml;
    QPushButton *m_btnXml;

    QListWidget *m_list;

    ElidibleLabel *m_labelInfo;

    QProgressBar *m_bar;

    QPushButton *m_btnStart;

    bool m_running;
};

#endif // REPORTGENERATOR_H
