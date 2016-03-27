#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSemaphore>
#include <QSettings>
#include <QSplitter>
#include <QTimerEvent>
#include <QUrl>
#include <QVBoxLayout>
#include "textdialog.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_timerId(-1)
{
    m_labelDir = new QLabel(tr("Search in:"), this);

    m_editDir = new QLineEdit(this);
    m_editDir->setPlaceholderText(tr("Path of the directory to be searched in..."));

    m_btnDir = new QPushButton(tr("Select..."), this);
    m_btnDir->setAutoDefault(false);
    connect(m_btnDir, SIGNAL(clicked(bool)),
            this, SLOT(selectDir()));

    m_labelTag = new QLabel(tr("Tag:"), this);

    m_editTag = new QLineEdit("SEIZURES:", this);
    m_editTag->setPlaceholderText(tr("All files must have this tag..."));

    m_checkTag = new QCheckBox(tr("Case Sensitive"), this);
    m_checkTag->setChecked(false);

    m_labelText = new QLabel(tr("Text:"), this);

    m_editText = new QLineEdit("None", this);
    m_editText->setPlaceholderText(tr("Whether a file contains this text..."));

    m_checkText = new QCheckBox(tr("Case Sensitive"), this);
    m_checkText->setChecked(false);

    m_labelExport = new QLabel(tr("Export Option:"), this);

    m_checkExport = new QCheckBox(tr("Absolute File Path"), this);
    m_checkExport->setChecked(false);

    m_listLeft = new QListWidget(this);
    m_listLeft->setUniformItemSizes(true);
    m_listLeft->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listLeft->setWordWrap(false);
    m_listLeft->setTextElideMode(Qt::ElideMiddle);
    m_listLeft->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_listLeft, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));

    m_btnLeft = new QPushButton(tr("Export"), this);
    m_btnLeft->setAutoDefault(false);
    connect(m_btnLeft, SIGNAL(clicked(bool)),
            this, SLOT(exportList()));

    m_groupLeft = new QGroupBox(this);

    m_listMiddle = new QListWidget(this);
    m_listMiddle->setUniformItemSizes(true);
    m_listMiddle->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listMiddle->setWordWrap(false);
    m_listMiddle->setTextElideMode(Qt::ElideMiddle);
    m_listMiddle->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_listMiddle, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));

    m_btnMiddle = new QPushButton(tr("Export"), this);
    m_btnMiddle->setAutoDefault(false);
    connect(m_btnMiddle, SIGNAL(clicked(bool)),
            this, SLOT(exportList()));

    m_groupMiddle = new QGroupBox(this);

    m_listRight = new QListWidget(this);
    m_listRight->setUniformItemSizes(true);
    m_listRight->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listRight->setTextElideMode(Qt::ElideMiddle);
    m_listRight->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_listRight, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QListWidgetItem*)));

    m_btnRight = new QPushButton(tr("Export"), this);
    m_btnRight->setAutoDefault(false);
    connect(m_btnRight, SIGNAL(clicked(bool)),
            this, SLOT(exportList()));

    m_groupRight = new QGroupBox(this);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Horizontal);

    m_button = new QPushButton(tr("&Search"), this);
    m_button->setDefault(true);
    connect(m_button, SIGNAL(clicked(bool)),
            this, SLOT(search()));

    m_main = new QWidget(this);

    QHBoxLayout *dirLayout = new QHBoxLayout();
    dirLayout->setContentsMargins(0, 0, 0, 0);
    dirLayout->addWidget(m_labelDir, 0);
    dirLayout->addWidget(m_editDir, 1);
    dirLayout->addWidget(m_btnDir, 0);

    QHBoxLayout *tagLayout = new QHBoxLayout();
    tagLayout->setContentsMargins(0, 0, 0, 0);
    tagLayout->addWidget(m_labelTag, 0);
    tagLayout->addWidget(m_editTag, 1);
    tagLayout->addWidget(m_checkTag, 0);

    QHBoxLayout *textLayout = new QHBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->addWidget(m_labelText, 0);
    textLayout->addWidget(m_editText, 1);
    textLayout->addWidget(m_checkText, 0);

    QHBoxLayout *exportLayout = new QHBoxLayout();
    exportLayout->setContentsMargins(0, 0, 0, 0);
    exportLayout->addWidget(m_labelExport, 0);
    exportLayout->addWidget(m_checkExport, 0);
    exportLayout->addStretch();

    QVBoxLayout *leftLayout = new QVBoxLayout(m_groupLeft);
    leftLayout->addWidget(m_listLeft, 1);
    leftLayout->addWidget(m_btnLeft, 0, Qt::AlignCenter);
    m_groupLeft->setLayout(leftLayout);

    QVBoxLayout *middleLayout = new QVBoxLayout(m_groupMiddle);
    middleLayout->addWidget(m_listMiddle, 1);
    middleLayout->addWidget(m_btnMiddle, 0, Qt::AlignCenter);
    m_groupMiddle->setLayout(middleLayout);

    QVBoxLayout *rightLayout = new QVBoxLayout(m_groupRight);
    rightLayout->addWidget(m_listRight, 1);
    rightLayout->addWidget(m_btnRight, 0, Qt::AlignCenter);
    m_groupRight->setLayout(rightLayout);

    m_splitter->addWidget(m_groupLeft);
    m_splitter->addWidget(m_groupMiddle);
    m_splitter->addWidget(m_groupRight);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setStretchFactor(2, 1);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    m_splitter->setCollapsible(2, false);

    QVBoxLayout *layout = new QVBoxLayout(m_main);
    layout->addLayout(dirLayout, 0);
    layout->addLayout(tagLayout, 0);
    layout->addLayout(textLayout, 0);
    layout->addLayout(exportLayout, 0);
    layout->addWidget(m_splitter, 1);
    layout->addWidget(m_button, 0, Qt::AlignCenter);
    m_main->setLayout(layout);

    setCentralWidget(m_main);

    m_splitter->setVisible(false);
    m_btnLeft->setEnabled(false);
    m_btnMiddle->setEnabled(false);
    m_btnRight->setEnabled(false);

    m_mutex = new QSemaphore(1);

    setAcceptDrops(true);
    setWindowTitle(tr("Tag Finder"));

    loadSettings();
}

MainWindow::~MainWindow()
{
    if (m_timerId > -1)
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }

    delete m_mutex;

    delete m_labelDir;
    delete m_editDir;
    delete m_btnDir;

    delete m_labelTag;
    delete m_editTag;
    delete m_checkTag;

    delete m_labelText;
    delete m_editText;
    delete m_checkText;

    delete m_labelExport;
    delete m_checkExport;

    delete m_listLeft;
    delete m_btnLeft;
    delete m_groupLeft;

    delete m_listMiddle;
    delete m_btnMiddle;
    delete m_groupMiddle;

    delete m_listRight;
    delete m_btnRight;
    delete m_groupRight;

    delete m_splitter;

    delete m_button;

    delete m_main;
}

void MainWindow::selectDir()
{
    QString defaultDir = m_editDir->text().trimmed();
    if (defaultDir.isEmpty() || !QFile::exists(defaultDir))
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        defaultDir = current.absolutePath();
    }

    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Directory"),
                                                    defaultDir);
    if (!dir.isEmpty())
        m_editDir->setText(QDir::toNativeSeparators(dir));
}

void MainWindow::exportList()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button)
        return;

    QListWidget *listWidget;
    if (button == m_btnLeft)
        listWidget = m_listLeft;
    else if (button == m_btnMiddle)
        listWidget = m_listMiddle;
    else
        listWidget = m_listRight;

    if (listWidget->count() == 0)
    {
        QMessageBox::information(this,
                                 tr("Export"),
                                 tr("Nothing to export."));
        return;
    }

    QString defaultFile = m_last;
    if (defaultFile.isEmpty())
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        defaultFile = current.absolutePath();
    }

    QString output = QFileDialog::getSaveFileName(this,
                                                  tr("Export to"),
                                                  defaultFile,
                                                  "*.*");
    if (output.isEmpty())
        return;

    m_last = output;

    saveSettings();

    QFile *file = new QFile(output);
    file->open(QFile::WriteOnly | QFile::Truncate);
    for (int i=0; i<listWidget->count(); i++)
    {
        file->write(listWidget->item(i)->text().toUtf8());
        file->write("\n");
    }
    file->close();
    delete file;
}

void MainWindow::search()
{
    QString dir = m_editDir->text().trimmed();
    if (dir.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("You must select a directory."));
        m_btnDir->setFocus();
        return;
    }

    if (!QDir(dir).exists())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Directory %1 does not exist.").arg(QDir::toNativeSeparators(dir)));
        m_btnDir->setFocus();
        return;
    }

    if (m_editTag->text().isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("You must specify a tag."));
        m_editTag->setFocus();
        return;
    }

    saveSettings();

    m_button->setText(tr("Searching..."));
    m_button->setEnabled(false);

    m_labelDir->setEnabled(false);
    m_editDir->setEnabled(false);
    m_btnDir->setEnabled(false);
    m_labelTag->setEnabled(false);
    m_editTag->setEnabled(false);
    m_checkTag->setEnabled(false);
    m_labelText->setEnabled(false);
    m_editText->setEnabled(false);
    m_checkText->setEnabled(false);
    m_labelExport->setEnabled(false);
    m_checkExport->setEnabled(false);

    if (m_editText->text().isEmpty())
    {
        m_groupMiddle->setVisible(false);
        m_groupLeft->setTitle(tr("With \"%1\"").arg(m_editTag->text()));
        m_groupRight->setTitle(tr("Without \"%1\"").arg(m_editTag->text()));
    }
    else
    {
        m_groupLeft->setTitle(tr("With \"%1\" and \"%2\"").arg(m_editTag->text()).arg(m_editText->text()));
        m_groupMiddle->setTitle(tr("With \"%1\", without \"%2\"").arg(m_editTag->text()).arg(m_editText->text()));
        m_groupRight->setTitle(tr("Without \"%1\"").arg(m_editTag->text()));
    }

    m_splitter->setVisible(true);

    m_timerId = startTimer(500);

    processDirectory(dir);

    killTimer(m_timerId);
    m_timerId = -1;

    refreshLists();

    m_btnLeft->setText(tr("Export (%1)").arg(m_listLeft->count()));
    m_btnMiddle->setText(tr("Export (%1)").arg(m_listMiddle->count()));
    m_btnRight->setText(tr("Export (%1)").arg(m_listRight->count()));

    m_btnLeft->setEnabled(true);
    m_btnMiddle->setEnabled(true);
    m_btnRight->setEnabled(true);

    m_button->setText(tr("Close"));
    disconnect(m_button, SIGNAL(clicked(bool)),
               this, SLOT(search()));
    connect(m_button, SIGNAL(clicked(bool)),
            this, SLOT(close()));
    m_button->setEnabled(true);
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    if (!item || !sender())
        return;

    QString text = item->text();
    if (!text.isEmpty())
    {
        if (!m_checkExport->isChecked())
        {
            text = m_editDir->text().append("/").append(text);
            text = text.replace("\\", "/").replace("//", "/");
            text = QDir::toNativeSeparators(text);
        }

        TextDialog *dialog = new TextDialog(text,
                                            m_editTag->text(),
                                            m_checkTag->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive,
                                            sender() != m_listRight,
                                            this);
        dialog->exec();
        delete dialog;
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId && m_timerId > -1)
        refreshLists();
    QMainWindow::timerEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && m_editDir->isEnabled())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QFileInfo info(url.toLocalFile());
                if (info.isDir())
                {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
    event->ignore();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls() && m_editDir->isEnabled())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QFileInfo info(url.toLocalFile());
                if (info.isDir())
                {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && m_editDir->isEnabled())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QFileInfo info(url.toLocalFile());
                if (info.isDir())
                {
                    event->acceptProposedAction();
                    m_editDir->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
                    return;
                }
            }
        }
    }
    event->ignore();
}

void MainWindow::processDirectory(const QString &dir)
{
    QDir d(dir);

    QString tag = m_editTag->text();
    QString text = m_editText->text();
    Qt::CaseSensitivity tagCase = m_checkTag->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    Qt::CaseSensitivity textCase = m_checkText->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    foreach (QFileInfo info, d.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot))
    {
        if (info.isFile())
        {
            //Process the text file
            if (info.fileName().endsWith(".txt", Qt::CaseInsensitive))
            {
                QFile *file = new QFile(info.absoluteFilePath());
                if (file->open(QFile::ReadOnly | QFile::Text))
                {
                    bool foundTag = false; //This is set to true when: 1) the file has a line that starts with the tag, can have white spaces before the tag
                    bool foundText = text.isEmpty() ? true : false; //This is set to true when: 1) text is empty; 2) text is not empty, the line contains only the tag and the text, white spaces, colon and dot can exist.

                    //Examples that have both true
                    //TAG TEXT
                    //TAG: TEXT.
                    //  TAG    TEXT
                    // TAG:   TEXT.

                    while (!file->atEnd())
                    {
                        QString line = QString::fromUtf8(file->readLine()).remove("\r").remove("\n").trimmed();
                        if (line.startsWith(tag, tagCase))
                        {
                            foundTag = true;
                            if (foundText)
                                break;
                            else
                            {
                                line = line.remove(tag, tagCase).remove(".").remove(":").trimmed();
                                if (line.compare(text, textCase) == 0)
                                {
                                    foundText = true;
                                    break;
                                }
                            }
                        }
                    }
                    file->close();
                    delete file;

                    m_mutex->acquire();
                    if (foundTag && foundText)
                        m_listTagAndText.append(QDir::toNativeSeparators(info.absoluteFilePath()));
                    else if (foundTag)
                        m_listTagOnly.append(QDir::toNativeSeparators(info.absoluteFilePath()));
                    else
                        m_listOthers.append(QDir::toNativeSeparators(info.absoluteFilePath()));
                    m_mutex->release();
                }
                else
                    delete file;
            }
        }
        else
            processDirectory(info.absoluteFilePath()); //Scan child directory recursively
    }
}

void MainWindow::refreshLists()
{
    m_mutex->acquire();
    if (!m_listTagAndText.isEmpty())
    {        
        if (m_checkExport->isChecked())
        {
            foreach (QString file, m_listTagAndText)
            {
                QListWidgetItem *item = new QListWidgetItem(file);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setToolTip(file);
                m_listLeft->addItem(item);
            }
        }
        else
        {
            foreach (QString file, m_listTagAndText)
            {
                QListWidgetItem *item = new QListWidgetItem(formatPath(file));
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setToolTip(item->text());
                m_listLeft->addItem(item);
            }
        }
        m_listTagAndText.clear();
    }
    if (!m_listTagOnly.isEmpty())
    {
        if (m_checkExport->isChecked())
        {
            foreach (QString file, m_listTagOnly)
            {
                QListWidgetItem *item = new QListWidgetItem(file);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setToolTip(file);
                m_listMiddle->addItem(item);
            }
        }
        else
        {
            foreach (QString file, m_listTagOnly)
            {
                QListWidgetItem *item = new QListWidgetItem(formatPath(file));
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setToolTip(item->text());
                m_listMiddle->addItem(item);
            }
        }
        m_listTagOnly.clear();
    }
    if (!m_listOthers.isEmpty())
    {
        if (m_checkExport->isChecked())
        {
            foreach (QString file, m_listOthers)
            {
                QListWidgetItem *item = new QListWidgetItem(file);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setToolTip(file);
                m_listRight->addItem(item);
            }
        }
        else
        {
            foreach (QString file, m_listOthers)
            {
                QListWidgetItem *item = new QListWidgetItem(formatPath(file));
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setToolTip(item->text());
                m_listRight->addItem(item);
            }
        }
        m_listTagAndText.clear();
    }
    m_mutex->release();
}

QString MainWindow::formatPath(const QString &path)
{
    QString tmp1 = path;
    tmp1 = tmp1.replace("\\", "/").replace("//", "/");
    QString tmp2 = m_editDir->text().replace("\\", "/").replace("//", "/");
    if (tmp1.startsWith(tmp2))
    {
        tmp1.remove(0, tmp2.size());
        if (tmp1.startsWith("/"))
            tmp1.remove(0, 1);
    }
    return QDir::toNativeSeparators(tmp1);
}

void MainWindow::loadSettings()
{
    QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
    current.cdUp();
    current.cdUp();
    current.cdUp();
#endif
    QFileInfo binInfo(qApp->applicationFilePath());
    QString config = current.absoluteFilePath(binInfo.baseName().append(".cfg"));
    if (!QFile::exists(config))
        return;

    QSettings settings(config, QSettings::IniFormat, this);
    if (settings.contains("Dir"))
        m_editDir->setText(settings.value("Dir").toString());
    if (settings.contains("Tag"))
        m_editTag->setText(settings.value("Tag").toString());
    if (settings.contains("Text"))
        m_editText->setText(settings.value("Text").toString());
    if (settings.contains("TagCase"))
        m_checkTag->setChecked(settings.value("TagCase").toBool());
    if (settings.contains("TextCase"))
        m_checkText->setChecked(settings.value("TextCase").toBool());
    if (settings.contains("AbsolutePath"))
        m_checkExport->setChecked(settings.value("Export").toBool());
    if (settings.contains("ExportFile"))
        m_last = settings.value("ExportFile").toString();
}

void MainWindow::saveSettings()
{
    QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
    current.cdUp();
    current.cdUp();
    current.cdUp();
#endif
    QFileInfo binInfo(qApp->applicationFilePath());
    QString config = current.absoluteFilePath(binInfo.baseName().append(".cfg"));

    QSettings settings(config, QSettings::IniFormat, this);
    settings.setValue("Dir", m_editDir->text().trimmed());
    settings.setValue("Tag", m_editTag->text().trimmed());
    settings.setValue("Text", m_editText->text().trimmed());
    settings.setValue("TagCase", m_checkTag->isChecked());
    settings.setValue("TextCase", m_checkText->isChecked());
    settings.setValue("AbsolutePath", m_checkExport->isChecked());
    settings.setValue("ExportFile", m_last);
}
