#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMimeData>
#include <QTextBrowser>
#include <QTextDocument>
#include <QUrl>
#include <QVBoxLayout>
#include "textbrowser.h"

TextBrowser::TextBrowser(QWidget* parent) :
    QGroupBox(parent)
{
    m_browser = new QTextBrowser(this);
    m_browser->setAcceptRichText(false);
    m_browser->setLineWrapMode(QTextEdit::NoWrap);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_browser);
    setLayout(layout);

    setAcceptDrops(true);
}

TextBrowser::TextBrowser(const QString &title, QWidget* parent) :
    QGroupBox(title, parent)
{
    m_browser = new QTextBrowser(this);
    m_browser->setAcceptRichText(false);
    m_browser->setLineWrapMode(QTextEdit::NoWrap);

    m_label = new QLabel(tr("Line count: 0"), this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_browser, 1);
    layout->addWidget(m_label, 0);
    setLayout(layout);

    setAcceptDrops(true);
}

TextBrowser::~TextBrowser()
{
    delete m_browser;
    delete m_label;
}

QString TextBrowser::text() const
{
    return m_browser->toPlainText();
}

void TextBrowser::loadFile(const QString &file)
{
    QFile *f = new QFile(file);
    if (f->exists())
    {
        f->open(QFile::ReadOnly | QFile::Text);
        setText(QString::fromUtf8(f->readAll()));
        f->close();
        delete f;
    }
}

void TextBrowser::setText(const QString &text)
{
    m_browser->setText(text);
    m_label->setText(tr("Line count: %1").arg(m_browser->document()->lineCount()));
}

void TextBrowser ::dragEnterEvent(QDragEnterEvent *event)
{
    if (event && event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
        {
            QFileInfo info(url.toLocalFile());
            if (info.isFile())
            {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void TextBrowser ::dragMoveEvent(QDragMoveEvent *event)
{
    if (event && event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
        {
            QFileInfo info(url.toLocalFile());
            if (info.isFile())
            {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void TextBrowser ::dropEvent(QDropEvent *event)
{
    if (event && event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
        {
            QFileInfo info(url.toLocalFile());
            if (info.isFile())
            {
                event->acceptProposedAction();
                emit fileChanged(info.absoluteFilePath());
                return;
            }
        }
    }
    event->ignore();
}
