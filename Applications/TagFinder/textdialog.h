#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QDialog>

class QPushButton;
class QTextBrowser;

class TextDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TextDialog(const QString &path,
                        const QString keyword,
                        const Qt::CaseSensitivity &sensitivity,
                        bool select = false,
                        QWidget *parent = 0);
    ~TextDialog();

private slots:
    void copyPath();

private:
    QTextBrowser *m_browser;
    QPushButton *m_buttonCopy;
    QPushButton *m_buttonClose;
};

#endif // TEXTDIALOG_H
