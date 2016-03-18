#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QGroupBox>

class QLabel;
class QTextBrowser;

class TextBrowser : public QGroupBox
{
    Q_OBJECT
public:
    explicit TextBrowser(QWidget* parent = 0);
    explicit TextBrowser(const QString &title, QWidget* parent = 0);
    ~TextBrowser();

    QString text() const;

signals:
    void fileChanged(const QString &file);

public slots:
    void loadFile(const QString &file);
    void setText(const QString &text);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);

private:
    QTextBrowser *m_browser;
    QLabel *m_label;
};

#endif // TEXTBROWSER_H
