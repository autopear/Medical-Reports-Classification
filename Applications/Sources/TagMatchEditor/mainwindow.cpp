#include <QApplication>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QUrl>
#include <QVariant>
#include <QVBoxLayout>
#include "variantlistwidget.h"
#include "taglistwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_labelFile = new QLabel(tr("File:"), this);

    m_editFile = new QLineEdit(this);
    m_editFile->setPlaceholderText(tr("Drag the settings file to the main window..."));

    connect(m_editFile, SIGNAL(returnPressed()),
            this, SLOT(loadFile()));

    m_btnFile = new QPushButton(tr("Select..."), this);
    m_btnFile->setAutoDefault(false);
    connect(m_btnFile, SIGNAL(clicked(bool)),
            this, SLOT(selectFile()));

    m_listTags = new TagListWidget(this);
    connect(m_listTags, SIGNAL(tagAdded(QString)),
            this, SLOT(onTagAdded(QString)));
    connect(m_listTags, SIGNAL(tagChanged(QString,QString)),
            this, SLOT(onTagChanged(QString,QString)));
    connect(m_listTags, SIGNAL(tagDeleted(QString)),
            this, SLOT(onTagDeleted(QString)));
    connect(m_listTags, SIGNAL(currentTextChanged(QString)),
            this, SLOT(onCurrentTagChanged(QString)));
    connect(m_listTags, SIGNAL(requestClear()),
            this, SLOT(onClearTags()));

    m_groupTags = new QGroupBox(tr("Tags"), this);
    QVBoxLayout *groupLayout = new QVBoxLayout(m_groupTags);
    groupLayout->addWidget(m_listTags);
    m_groupTags->setLayout(groupLayout);

    m_listVars = new VariantListWidget(this);
    m_listVars->setEnabled(false);
    connect(m_listVars, SIGNAL(variantsChanged(QStringList)),
            this, SLOT(onVariantsChanged()));
    connect(m_listVars, SIGNAL(requestClear()),
            this, SLOT(onClearVariants()));

    m_groupVars = new QGroupBox(tr("Tag Variants"), this);
    groupLayout = new QVBoxLayout(m_groupVars);
    groupLayout->addWidget(m_listVars);
    m_groupVars->setLayout(groupLayout);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->addWidget(m_groupTags);
    m_splitter->addWidget(m_groupVars);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);

    m_btnSave = new QPushButton(tr("&Save"), this);
    m_btnSave->setAutoDefault(false);
    m_btnSave->setShortcut(QKeySequence::Save);
    connect(m_btnSave, SIGNAL(clicked(bool)),
            this, SLOT(saveFile()));

    m_btnClose = new QPushButton(tr("&Close"), this);
    m_btnClose->setAutoDefault(false);
    m_btnClose->setDefault(true);
    connect(m_btnClose, SIGNAL(clicked(bool)),
            this, SLOT(close()));

    int w = qMax(m_btnSave->sizeHint().width(), m_btnClose->sizeHint().width());
    m_btnSave->setFixedWidth(w);
    m_btnClose->setFixedWidth(w);

    m_main = new QWidget(this);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLayout->setContentsMargins(0, 0, 0, 0);
    fileLayout->addWidget(m_labelFile, 0);
    fileLayout->addWidget(m_editFile, 1);
    fileLayout->addWidget(m_btnFile, 0);

    QHBoxLayout *btnsLayout = new QHBoxLayout();
    btnsLayout->setContentsMargins(0, 0, 0, 0);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnSave);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnClose);
    btnsLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(m_main);
    layout->addLayout(fileLayout, 0);
    layout->addWidget(m_splitter, 1);
    layout->addLayout(btnsLayout, 0);
    m_main->setLayout(layout);

    setCentralWidget(m_main);

    setWindowTitle(tr("Tag Match Editor"));
    setAcceptDrops(true);

    loadSettings();
    loadFile();
}

MainWindow::~MainWindow()
{
    delete m_labelFile;
    delete m_editFile;
    delete m_btnFile;

    delete m_listTags;
    delete m_groupTags;

    delete m_listVars;
    delete m_groupVars;

    delete m_splitter;

    delete m_btnSave;
    delete m_btnClose;

    delete m_main;
}

void MainWindow::selectFile()
{
    QString defaultFile = m_editFile->text().trimmed();
    if (defaultFile.isEmpty() || !QFile::exists(defaultFile))
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        defaultFile = current.absolutePath();
    }


    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Open"),
                                                defaultFile,
                                                "*.*");
    if (!file.isEmpty())
    {
        file = QDir::toNativeSeparators(file);
        if (file.compare(m_editFile->text()) != 0)
        {
            m_editFile->setText(file);
            loadFile();
        }
    }
}

void MainWindow::saveFile()
{    
    QString saveFile = m_editFile->text().trimmed();
    if (saveFile.isEmpty())
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        saveFile = QFileDialog::getSaveFileName(this,
                                                tr("Save"),
                                                current.absolutePath(),
                                                "*.*");
    }

    if (saveFile.isEmpty())
        return;

    QSettings settings(saveFile, QSettings::IniFormat, this);

    QVariantMap map;
    foreach (QString tag, m_tags.keys())
        map.insert(tag, QVariant(m_tags.value(tag)));

    settings.setValue("Tags", map);

    if (map.size()  < 2)
        QMessageBox::information(this,
                                 tr("Save"),
                                 tr("%1 tag saved to %2.")
                                 .arg(map.size())
                                 .arg(QDir::toNativeSeparators(saveFile)));
    else
        QMessageBox::information(this,
                                 tr("Save"),
                                 tr("%1 tags saved to %2.")
                                 .arg(map.size())
                                 .arg(QDir::toNativeSeparators(saveFile)));
}

bool MainWindow::loadFile()
{
    m_listTags->clearAllTags();
    m_listVars->clearAllVariants();

    QString file = m_editFile->text().trimmed();
    m_editFile->setText(QDir::toNativeSeparators(file));

    if (file.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Open"),
                             tr("You must specify a file to open."));
        return false;
    }

    if (!QFile::exists(file))
    {
        QMessageBox::warning(this,
                             tr("Open"),
                             tr("Cannot find %1.").arg(file));
        return false;
    }

    saveSettings();

    QSettings settings(file, QSettings::IniFormat, this);

    if (settings.contains("Tags"))
    {
        QVariantMap map = settings.value("Tags").toMap();

        if (!map.isEmpty())
        {
            foreach (QString tag, map.keys())
                m_tags.insert(tag, map.value(tag).toStringList());

            m_listTags->addNewTags(QStringList(m_tags.keys()));

            m_listTags->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
        }
    }

    m_listTags->setFocus();

    return true;
}

void MainWindow::onTagChanged(const QString &oldTag, const QString &newTag)
{
    QStringList vars = m_tags.take(oldTag);
    if (!vars.contains(newTag))
    {
        vars.append(newTag);
        vars.sort();
    }

    m_tags.insert(newTag, vars);
}

void MainWindow::onTagDeleted(const QString &tag)
{
    m_tags.remove(tag);
}

void MainWindow::onTagAdded(const QString &tag)
{
    m_tags.insert(tag, QStringList() << tag);
}

void MainWindow::onCurrentTagChanged(const QString &tag)
{
    if (tag.isEmpty())
    {
        m_listVars->clearAllVariants();
        m_groupVars->setTitle(tr("Tag Variants"));
        m_listVars->setEnabled(false);
    }
    else
    {
        m_listVars->setVariants(tag, m_tags.value(tag));
        m_groupVars->setTitle(tr("Variants of \"%1\"").arg(tag));
        m_listVars->setEnabled(true);
    }
}

void MainWindow::onVariantsChanged()
{
    QString currentTag = m_listTags->currentTag();
    if (currentTag.isEmpty())
        return;

    m_tags.insert(currentTag, m_listVars->variants());
}

void MainWindow::onClearTags()
{
    if (m_tags.isEmpty())
        return;

    m_listTags->clearAllTags();
    m_listVars->clearAllVariants();
    m_tags.clear();
}

void MainWindow::onClearVariants()
{
    //Not implemented
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
        if (url.isLocalFile())
        {
            QFileInfo info(url.toLocalFile());
            if (info.isFile() && info.fileName().contains("."))
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
            if (info.isFile() && info.fileName().contains("."))
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
            if (info.isFile() && info.fileName().contains("."))
            {
                event->acceptProposedAction();
                m_editFile->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
                loadFile();
                return;
            }
        }
    }
    event->ignore();
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
    if (settings.contains("File"))
        m_editFile->setText(settings.value("File").toString());
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
    settings.setValue("File", m_editFile->text().trimmed());
}
