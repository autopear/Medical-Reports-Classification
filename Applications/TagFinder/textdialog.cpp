#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextCharFormat>
#include <QTextFormat>
#include "textdialog.h"

TextDialog::TextDialog(const QString &path, const QString keyword, const Qt::CaseSensitivity &sensitivity, bool select, QWidget *parent) :
    QDialog(parent)
{
    QFile *file = new QFile(path);
    file->open(QFile::ReadOnly | QFile::Text);
    QString content;
    int begin = -1, end = -1;
    while (!file->atEnd())
    {
        QString line = QString::fromUtf8(file->readLine());
        line = line.replace("\r\n", "\n").replace("\r", "\n");
        if (end == -1)
            begin = content.size();
        content = content.append(line);

        if (select && end == -1)
        {
            line = line.trimmed();
            if (line.startsWith(keyword, sensitivity))
                end = begin + keyword.size();
        }
    }
    file->close();
    delete file;

    m_browser = new QTextBrowser(this);
    m_browser->setAcceptRichText(false);
    m_browser->setLineWrapMode(QTextBrowser::NoWrap);
    m_browser->setText(content);
    if (select && end > -1)
    {
        QTextCursor c = m_browser->textCursor();
        c.setPosition(begin);
        c.setPosition(end, QTextCursor::KeepAnchor);
        m_browser->setTextCursor(c);
    }

    m_buttonCopy = new QPushButton(tr("Copy &Path"), this);
    m_buttonCopy->setDefault(false);
    connect(m_buttonCopy, SIGNAL(clicked(bool)),
            this, SLOT(copyPath()));

    m_buttonClose = new QPushButton(tr("&Close"), this);
    m_buttonClose->setDefault(true);
    connect(m_buttonClose, SIGNAL(clicked(bool)),
            this, SLOT(accept()));

    int w = qMax(m_buttonCopy->sizeHint().width(), m_buttonClose->sizeHint().width());
    m_buttonCopy->setFixedWidth(w);
    m_buttonClose->setFixedWidth(w);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_browser, 0, 0, 1, 4);
    layout->addWidget(m_buttonCopy, 1, 1, 1, 1);
    layout->addWidget(m_buttonClose, 1, 2, 1, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 0);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 0);
    layout->setColumnStretch(3, 1);
    setLayout(layout);

    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setWindowTitle(path);
    setSizeGripEnabled(true);
}

TextDialog::~TextDialog()
{
    delete m_browser;
    delete m_buttonCopy;
    delete m_buttonClose;
}

void TextDialog::copyPath()
{
    qApp->clipboard()->setText(windowTitle());
}
