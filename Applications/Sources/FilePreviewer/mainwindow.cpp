#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSplitter>
#include <QTextBrowser>
#include <QVBoxLayout>
#include "mainwindow.h"

static QString formatPath(const QString &path)
{
    QString ret = path;
    return ret.replace("\\", "/").replace("//", "/");
}

static bool isAbsolute(const QString &path)
{
#ifdef Q_OS_WIN
    return (path.at(1) == ':');
#else
    return path.startsWith("/");
#endif
}

static QString getFullPath(const QString &dir, const QString &file)
{
    QString p1 = formatPath(file);
    if (isAbsolute(p1))
        return p1;
    else
    {
        QString p2 = formatPath(dir);
        if (p2.endsWith("/"))
            return p2.append(p1);
        else
            return p2.append("/").append(p1);
    }
}

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    QFile f1("/Users/QZMAO/Documents/Courses/CIS 5538/CIS5538-Project/split_data/No_seizures.txt");
    f1.open(QFile::ReadOnly);
    QStringList ll = QString::fromUtf8(f1.readAll()).split("\n", QString::SkipEmptyParts);
    f1.close();

    QFile f2("/Users/QZMAO/Documents/Courses/CIS 5538/CIS5538-Project/split_data/Possibly_seizures.txt");
    f2.open(QFile::ReadOnly);
    QStringList ls = QString::fromUtf8(f2.readAll()).split("\n", QString::SkipEmptyParts);
    f2.close();

    foreach (QString l, ll)
        ls.removeOne(l);

    foreach (QString l, ls)
    {
        qDebug(l.toUtf8());
    }

    m_labelDir = new QLabel(tr("Search In:"), this);

    m_editDir = new QLineEdit(this);

    m_btnDir = new QPushButton(tr("&Select..."), this);
    m_btnDir->setDefault(false);
    m_btnDir->setAutoDefault(false);
    connect(m_btnDir, SIGNAL(clicked(bool)),
            this, SLOT(selectDir()));

    m_labelFile = new QLabel(tr("List File:"), this);

    m_editFile = new QLineEdit(this);

    m_btnFile = new QPushButton(tr("&Select..."), this);
    m_btnFile->setDefault(false);
    m_btnFile->setAutoDefault(false);
    connect(m_btnFile, SIGNAL(clicked(bool)),
            this, SLOT(selectFileList()));

    m_list = new QListWidget(this);
    m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_list, SIGNAL(itemSelectionChanged()),
            this, SLOT(onCurrentItemChanged()));
    connect(m_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));

    m_browser = new QTextBrowser(this);
    m_browser->setAcceptRichText(false);
    m_browser->setLineWrapMode(QTextBrowser::NoWrap);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(m_list);
    m_splitter->addWidget(m_browser);

    m_btnLoad = new QPushButton(tr("&Load"), this);
    m_btnLoad->setDefault(false);
    connect(m_btnLoad, SIGNAL(clicked(bool)),
            this, SLOT(loadFileList()));

    m_btnClose = new QPushButton(tr("&Close"), this);
    m_btnClose->setDefault(true);
    connect(m_btnClose, SIGNAL(clicked(bool)),
            this, SLOT(close()));

    m_main = new QWidget(this);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setContentsMargins(0, 0, 0, 0);
    layout1->addWidget(m_labelDir, 0);
    layout1->addWidget(m_editDir, 1);
    layout1->addWidget(m_btnDir, 0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->addWidget(m_labelFile, 0);
    layout2->addWidget(m_editFile, 1);
    layout2->addWidget(m_btnFile, 0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addStretch();
    layout3->addWidget(m_btnLoad, 0);
    layout3->addStretch();
    layout3->addWidget(m_btnClose, 0);
    layout3->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(m_main);
    layout->addLayout(layout1, 0);
    layout->addLayout(layout2, 0);
    layout->addWidget(m_splitter);
    layout->addLayout(layout3, 0);
    m_main->setLayout(layout);

    setCentralWidget(m_main);
    setWindowTitle(tr("File Previewer"));
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete m_labelDir;
    delete m_editDir;
    delete m_btnDir;
    delete m_labelFile;
    delete m_editFile;
    delete m_btnFile;
    delete m_list;
    delete m_browser;
    delete m_splitter;
    delete m_btnLoad;
    delete m_btnClose;
    delete m_main;
}

void MainWindow::selectDir()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Select"),
                                                     QDir::currentPath());
    if (!path.isEmpty())
        m_editDir->setText(QDir::toNativeSeparators(path));
}

void MainWindow::selectFileList()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Select"),
                                                QDir::currentPath(),
                                                "*");
    if (!path.isEmpty())
        m_editFile->setText(QDir::toNativeSeparators(path));
}

void MainWindow::loadFileList()
{
    if (m_editFile->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,
                             windowTitle(),
                             tr("The list file is unspecified."));
        m_editFile->setFocus();
        return;
    }

    QFile *file = new QFile(m_editFile->text());
    if (!file->exists())
    {
        QMessageBox::warning(this,
                             windowTitle(),
                             tr("Cannot find %1.").arg(QDir::toNativeSeparators(file->fileName())));
        m_editFile->setFocus();
        delete file;
        return;
    }

    if (!file->open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,
                             windowTitle(),
                             tr("Cannot open %1.").arg(QDir::toNativeSeparators(file->fileName())));
        m_editFile->setFocus();
        delete file;
        return;
    }

    m_list->clear();
    m_browser->clear();

    while (!file->atEnd())
    {
        QString line = QString::fromUtf8(file->readLine()).remove("\r").remove("\n").trimmed();
        if (line.isEmpty() || line.startsWith("//"))
            continue;

        if (isAbsolute(line))
        {
            QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(line), m_list, QListWidgetItem::UserType);
            m_list->addItem(item);
        }
        else
        {
            if (m_editDir->text().trimmed().isEmpty())
            {
                QMessageBox::warning(this,
                                     windowTitle(),
                                     tr("The directory is unspecified."));
                m_list->clear();
                m_editDir->setFocus();
                break;
            }

            QString path = getFullPath(m_editDir->text().trimmed(), line);
            QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(path), m_list, QListWidgetItem::UserType);
            m_list->addItem(item);
        }
    }

    if (m_list->count() > 0)
        m_list->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);

    file->close();
    delete file;
}

void MainWindow::onCurrentItemChanged()
{
    if (!m_list->currentItem())
        return;

    QString path = m_list->currentItem()->text();

    if (QFile::exists(path))
    {
        m_browser->setAcceptRichText(false);
        m_browser->setLineWrapMode(QTextBrowser::NoWrap);

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

            if (end == -1)
            {
                line = line.trimmed();
                if (line.startsWith("seizures:", Qt::CaseInsensitive))
                {
                    end = begin + 9;
                }
            }
        }
        file->close();
        delete file;

        m_browser->setText(content);

        if (end > -1)
        {
            QTextCursor c = m_browser->textCursor();
            c.setPosition(begin);
            c.setPosition(end, QTextCursor::KeepAnchor);
            m_browser->setTextCursor(c);
        }
    }
    else
    {
        m_browser->setAcceptRichText(true);
        m_browser->setText(QString("<html><font color=\"red\">Cannot find <a href=\"file://%1>%1</a>.</font></html>").arg(path));
        m_browser->setLineWrapMode(QTextBrowser::WidgetWidth);
    }
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    if (item)
        qApp->clipboard()->setText(item->text());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
        {
            QFileInfo info(url.toLocalFile());
            if (info.isFile() || info.isDir())
            {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
        {
            QFileInfo info(url.toLocalFile());
            if (info.isFile() || info.isDir())
            {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
        {
            QFileInfo info(url.toLocalFile());
            if (info.isFile())
            {
                event->acceptProposedAction();
                m_editFile->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
                return;
            }
            if (info.isDir())
            {
                event->acceptProposedAction();
                m_editDir->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
                return;
            }
        }
    }
    event->ignore();
}
