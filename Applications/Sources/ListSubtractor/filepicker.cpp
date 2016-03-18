#include <QDir>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPushButton>
#include <QUrl>
#include "filepicker.h"

FilePicker::FilePicker(const QString &text, QWidget *parent) :
    QWidget(parent)
{
    m_label = new QLabel(text, this);

    m_edit = new QLineEdit(this);
    m_edit->setPlaceholderText(tr("Drag file here..."));
    connect(m_edit, SIGNAL(editingFinished()),
            this, SLOT(onEnterPressed()));
    connect(m_edit, SIGNAL(returnPressed()),
            this, SLOT(onEnterPressed()));

    m_btn = new QPushButton(tr("Select..."), this);
    m_btn->setAutoDefault(false);
    connect(m_btn, SIGNAL(clicked(bool)),
            this, SLOT(selectFile()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_label, 0);
    layout->addWidget(m_edit, 1);
    layout->addWidget(m_btn, 0);
    setLayout(layout);

    setAcceptDrops(true);
}

FilePicker::~FilePicker()
{
    delete m_label;
    delete m_edit;
    delete m_btn;
}

QString FilePicker::file() const
{
    return m_edit->text();
}

void FilePicker::setFile(const QString &file)
{
    m_edit->setText(QDir::toNativeSeparators(file));
}

void FilePicker::dragEnterEvent(QDragEnterEvent *event)
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

void FilePicker::dragMoveEvent(QDragMoveEvent *event)
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

void FilePicker::dropEvent(QDropEvent *event)
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
                QString newFile = QDir::toNativeSeparators(info.absoluteFilePath());
                if (newFile.compare(m_edit->text()) != 0)
                {
                    m_edit->setText(newFile);
                    emit fileChanged(newFile);
                }
                return;
            }
        }
    }
    event->ignore();
}

void FilePicker::selectFile()
{
    QString dir = QDir::currentPath();
    if (!m_edit->text().isEmpty())
    {
        QFileInfo info(m_edit->text());
        if (info.exists())
            dir = info.absoluteDir().absolutePath();
    }
    QString newFile = QFileDialog::getOpenFileName(this,
                                                   tr("Select..."),
                                                   dir,
                                                   "*");
    if (!newFile.isEmpty())
    {
        newFile = QDir::toNativeSeparators(newFile);
        if (newFile.compare(m_edit->text()) != 0)
        {
            m_edit->setText(newFile);
            emit fileChanged(newFile);
        }
    }
}

void FilePicker::onEnterPressed()
{
    emit fileChanged(m_edit->text());
}
