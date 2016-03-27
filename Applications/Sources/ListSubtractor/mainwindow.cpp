#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QVBoxLayout>
#include "filepicker.h"
#include "textbrowser.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_picker1 = new FilePicker(tr("List 1:"), this);

    m_picker2 = new FilePicker(tr("List 2:"), this);

    m_browser1 = new TextBrowser(tr("List 1"), this);

    m_browser2 = new TextBrowser(tr("List 2"), this);

    m_browser3 = new TextBrowser(tr("List 1 - 2"), this);
    m_browser3->setAcceptDrops(false);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->addWidget(m_browser1);
    m_splitter->addWidget(m_browser2);
    m_splitter->addWidget(m_browser3);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setStretchFactor(2, 1);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    m_splitter->setCollapsible(2, false);

    m_btnSave = new QPushButton(tr("&Save"), this);
    m_btnSave->setAutoDefault(false);

    m_btnClose = new QPushButton(tr("&Close"), this);
    m_btnClose->setAutoDefault(false);
    m_btnClose->setDefault(true);

    int w = qMax(m_btnClose->sizeHint().width(), m_btnSave->sizeHint().width());
    m_btnClose->setFixedWidth(w);
    m_btnSave->setFixedWidth(w);

    m_widget = new QWidget(this);

    QHBoxLayout *btnsLayout = new QHBoxLayout();
    btnsLayout->setContentsMargins(0, 0, 0, 0);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnSave);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnClose);
    btnsLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(m_widget);
    layout->addWidget(m_picker1, 0);
    layout->addWidget(m_picker2, 0);
    layout->addWidget(m_splitter, 1);
    layout->addLayout(btnsLayout, 0);
    m_widget->setLayout(layout);

    setCentralWidget(m_widget);
    setWindowTitle(tr("List Subtractor"));

    connect(m_picker1, SIGNAL(fileChanged(QString)),
            this, SLOT(onFileChanged(QString)));
    connect(m_picker2, SIGNAL(fileChanged(QString)),
            this, SLOT(onFileChanged(QString)));
    connect(m_browser1, SIGNAL(fileChanged(QString)),
            this, SLOT(onFileChanged(QString)));
    connect(m_browser2, SIGNAL(fileChanged(QString)),
            this, SLOT(onFileChanged(QString)));
    connect(m_btnSave, SIGNAL(clicked(bool)),
            this, SLOT(saveResult()));

    m_btnSave->setEnabled(false);

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete m_picker1;
    delete m_picker2;
    delete m_browser1;
    delete m_browser2;
    delete m_browser3;
    delete m_splitter;
    delete m_btnSave;
    delete m_btnClose;
    delete m_widget;
}

void MainWindow::onFileChanged(const QString &file)
{
    if (sender() == m_picker1)
    {
        m_browser1->loadFile(file);
        updateResult();
    }
    else if (sender() == m_picker2)
    {
        m_browser2->loadFile(file);
        updateResult();
    }
    else if (sender() == m_browser1)
    {
        QString newFile = QDir::toNativeSeparators(file);
        if (newFile.compare(m_picker1->file()) != 0)
        {
            m_picker1->setFile(newFile);
            m_browser1->loadFile(newFile);
            updateResult();
        }
    }
    else if (sender() == m_browser2)
    {
        QString newFile = QDir::toNativeSeparators(file);
        if (newFile.compare(m_picker2->file()) != 0)
        {
            m_picker2->setFile(newFile);
            m_browser2->loadFile(newFile);
            updateResult();
        }
    }
    else
    {
    }
}

static QString formatText(const QString &text)
{
    QString ret = text;
    return ret.replace("\r\n", "\n").replace("\r", "\n");
}

void MainWindow::saveResult()
{
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

    QString save = QFileDialog::getSaveFileName(this,
                                                tr("Save"),
                                                defaultFile,
                                                "*");
    if (!save.isEmpty())
    {
        m_last = save;

        QFile *file = new QFile(save);
        file->open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
        file->write(formatText(m_browser3->text()).toUtf8());
        file->close();
        delete file;

        saveSettings();
    }
}

void MainWindow::updateResult()
{
    saveSettings();

    QStringList list1 = formatText(m_browser1->text()).split("\n", QString::KeepEmptyParts);
    QStringList list2 = formatText(m_browser2->text()).split("\n", QString::KeepEmptyParts);

    foreach (QString line, list2)
    {
        if (line.isEmpty() || line.startsWith("//"))
            continue;

        list1.removeAll(line);
    }

    if (list1.isEmpty())
    {
        m_browser3->setText(QString());
        m_btnSave->setEnabled(false);
    }
    else
    {
        m_browser3->setText(list1.join("\n"));
        m_btnSave->setEnabled(true);
    }
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
    if (settings.contains("List1"))
        m_picker1->setFile(settings.value("List1").toString());
    if (settings.contains("List2"))
        m_picker2->setFile(settings.value("List2").toString());
    if (settings.contains("Last"))
        m_last = settings.value("Last").toString();
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
    settings.setValue("List1", m_picker1->file());
    settings.setValue("List2", m_picker2->file());
    settings.setValue("Last", m_last);
}
