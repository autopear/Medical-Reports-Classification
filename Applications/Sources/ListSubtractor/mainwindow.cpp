#include <QPushButton>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QFile>
#include <QFileDialog>
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
    QString save = QFileDialog::getSaveFileName(this,
                                                tr("Save"),
                                                m_last.isEmpty() ? QDir::currentPath() : m_last,
                                                "*");
    if (!save.isEmpty())
    {
        QFile *file = new QFile(save);
        file->open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
        file->write(formatText(m_browser3->text()).toUtf8());
        file->close();
        delete file;
    }
}

void MainWindow::updateResult()
{
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
