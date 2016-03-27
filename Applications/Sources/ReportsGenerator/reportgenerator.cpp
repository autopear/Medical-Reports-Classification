#include <QApplication>
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
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QStringListModel>
#include <QUrl>
#include <QVBoxLayout>
#include "elidiblelabel.h"
#include "reportparser.h"
#include "reportgenerator.h"

ReportGenerator::ReportGenerator(QWidget *parent) :
    QWidget(parent),
    m_running(false)
{
    m_labelDir = new QLabel(tr("Directory:"), this);

    m_editDir = new QLineEdit(this);
    m_editDir->setPlaceholderText(tr("Drag a directory to the main window..."));

    m_btnDir = new QPushButton(tr("Select..."), this);
    m_btnDir->setAutoDefault(false);
    connect(m_btnDir, SIGNAL(clicked(bool)),
            this, SLOT(selectDir()));

    m_labelTags = new QLabel(tr("Tags Config:"), this);

    m_editTags = new QLineEdit(this);
    m_editTags->setPlaceholderText(tr("Drag the tags configuration file to the main window..."));
    m_editTags->setMinimumWidth(m_editTags->sizeHint().width() * 1.5);

    m_btnTags = new QPushButton(tr("Select..."), this);
    m_btnTags->setAutoDefault(false);
    connect(m_btnTags, SIGNAL(clicked(bool)),
            this, SLOT(selectTags()));

    m_labelXml = new QLabel(tr("XML:"), this);

    m_editXml = new QLineEdit(this);
    m_editXml->setPlaceholderText(tr("Use the right button..."));

    m_btnXml = new QPushButton(tr("Select..."), this);
    m_btnXml->setAutoDefault(false);
    connect(m_btnXml, SIGNAL(clicked(bool)),
            this, SLOT(selectXml()));

    m_list = new QListWidget(this);
    m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setTextElideMode(Qt::ElideMiddle);
    m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_labelInfo = new ElidibleLabel(this);

    m_bar = new QProgressBar(this);
    m_bar->setTextVisible(true);
    m_bar->setAlignment(Qt::AlignCenter);

    m_btnStart = new QPushButton(tr("&Start"), this);
    m_btnStart->setAutoDefault(false);
    m_btnStart->setDefault(true);
    connect(m_btnStart, SIGNAL(clicked(bool)),
            this, SLOT(start()));

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setContentsMargins(0, 0, 0, 0);
    layout1->addWidget(m_labelDir, 0);
    layout1->addWidget(m_editDir, 1);
    layout1->addWidget(m_btnDir, 0);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->addWidget(m_labelTags, 0);
    layout2->addWidget(m_editTags, 1);
    layout2->addWidget(m_btnTags, 0);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addWidget(m_labelXml, 0);
    layout3->addWidget(m_editXml, 1);
    layout3->addWidget(m_btnXml, 0);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(layout1, 0);
    layout->addLayout(layout2, 0);
    layout->addLayout(layout3, 0);
    layout->addWidget(m_list, 1);
    layout->addWidget(m_labelInfo, 0);
    layout->addWidget(m_bar, 0);
    layout->addWidget(m_btnStart, 0, Qt::AlignCenter);
    setLayout(layout);

    setAcceptDrops(true);

    loadSettings();
}

ReportGenerator::~ReportGenerator()
{
    delete m_labelDir;
    delete m_editDir;
    delete m_btnDir;

    delete m_labelTags;
    delete m_editTags;
    delete m_btnTags;

    delete m_labelXml;
    delete m_editXml;
    delete m_btnXml;

    delete m_list;

    delete m_labelInfo;

    delete m_bar;

    delete m_btnStart;
}

bool ReportGenerator::isRunning() const
{
    return m_running;
}

void ReportGenerator::selectDir()
{
    QString dir = m_editDir->text().trimmed();
    if (dir.isEmpty() || !QFile::exists(dir))
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        dir = current.absolutePath();
    }

    dir = QFileDialog::getExistingDirectory(this,
                                            tr("Directory"),
                                            dir);

    if (!dir.isEmpty())
        m_editDir->setText(QDir::toNativeSeparators(dir));
}

void ReportGenerator::selectTags()
{
    QString ini = m_editTags->text().trimmed();
    if (ini.isEmpty() || !QFile::exists(ini))
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        ini = current.absolutePath();
    }

    ini = QFileDialog::getOpenFileName(this,
                                       tr("Tags Configuration"),
                                       ini,
                                       "*.*");

    if (!ini.isEmpty())
        m_editTags->setText(QDir::toNativeSeparators(ini));
}

void ReportGenerator::selectXml()
{
    QString xml = m_editXml->text().trimmed();
    if (xml.isEmpty())
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        xml = current.absolutePath();
    }

    xml = QFileDialog::getSaveFileName(this,
                                       tr("Save"),
                                       xml,
                                       "*.xml");

    if (!xml.isEmpty())
        m_editXml->setText(QDir::toNativeSeparators(xml));
}

void ReportGenerator::start()
{
    QString dir = QDir::toNativeSeparators(m_editDir->text().trimmed());
    QString ini = m_editTags->text().trimmed();
    QString xml = m_editXml->text().trimmed();

    if (dir.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Directory is not set."));
        m_editDir->setFocus();
        return;
    }

    if (ini.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Tags configuration file is not set."));
        m_editTags->setFocus();
        return;
    }

    QSettings settings(ini, QSettings::IniFormat);
    QVariantMap map;
    if (settings.contains("Tags"))
        map = settings.value("Tags").toMap();
    if (map.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Cannot find any tags configuration in %1.").arg(QDir::toNativeSeparators(ini)));
        m_editTags->setFocus();
        return;
    }
    QMap<QString, QString> tags;
    foreach (QString tag, map.keys())
    {
        QStringList vars = map.value(tag).toStringList();
        foreach (QString var, vars)
            tags.insert(var, tag);
    }

    if (xml.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("XML is not set."));
        m_editXml->setFocus();
        return;
    }

    QStringList reports;
    findAllReports(dir, &reports);

    if (reports.isEmpty())
    {
        QMessageBox::information(this,
                                 tr("No Report"),
                                 tr("No report found in %1").arg(QDir::toNativeSeparators(dir)));
        return;
    }

    saveSettings();

    m_running = true;

    m_bar->setRange(0, reports.size());

    QStringList list;
    foreach (QString report, reports)
    {
        report = QDir::toNativeSeparators(report);
        report = report.remove(0, dir.size());
        while (report.startsWith("/") || report.startsWith("\\"))
            report = report.remove(0, 1);
        list.append(report);
    }
    list.sort();

    m_list->clear();
    foreach (QString report, list)
    {
        QListWidgetItem *item = new QListWidgetItem(report);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setToolTip(item->text());

        m_list->addItem(item);
    }

    m_labelDir->setEnabled(false);
    m_editDir->setEnabled(false);
    m_btnDir->setEnabled(false);
    m_labelTags->setEnabled(false);
    m_editTags->setEnabled(false);
    m_btnTags->setEnabled(false);
    m_labelXml->setEnabled(false);
    m_editXml->setEnabled(false);
    m_btnXml->setEnabled(false);
    m_btnStart->setEnabled(false);

    m_btnStart->setText(tr("Processing"));

    dir = dir.replace("\\", "/").replace("//", "/");
    if (!dir.endsWith("/"))
        dir = dir.append("/");

    ReportParser *parser = new ReportParser(list, tags, dir, xml, 0);
    connect(parser, SIGNAL(reportParsed(int)),
            this, SLOT(onReportParsed(int)));
    connect(parser, SIGNAL(finished()),
            parser, SLOT(deleteLater()));

    parser->start();
}

void ReportGenerator::onReportParsed(int index)
{
    m_bar->setValue(index + 1);

    m_labelInfo->setText(m_list->item(index)->text());

    if (m_bar->value() == m_bar->maximum())
    {
        //Finished
        QMessageBox::information(this,
                                 tr("Finished"),
                                 tr("Database created at %1").arg(QDir::toNativeSeparators(m_editXml->text().trimmed())));

        m_btnStart->setText(tr("&Start"));

        m_labelDir->setEnabled(true);
        m_editDir->setEnabled(true);
        m_btnDir->setEnabled(true);
        m_labelTags->setEnabled(true);
        m_editTags->setEnabled(true);
        m_btnTags->setEnabled(true);
        m_labelXml->setEnabled(true);
        m_editXml->setEnabled(true);
        m_btnXml->setEnabled(true);
        m_btnStart->setEnabled(true);

        m_running = false;
    }
}

void ReportGenerator::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
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
    }
    event->ignore();
}

void ReportGenerator::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
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
    }
    event->ignore();
}

void ReportGenerator::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        bool foundFile = false, foundDir = false;
        foreach (QUrl url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QFileInfo info(url.toLocalFile());
                if (info.isFile() && !foundFile)
                {
                    m_editTags->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
                    foundFile = true;
                }
                else if (info.isDir() && !foundDir)
                {
                    m_editDir->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
                    foundDir = true;
                }
                else
                {

                }
                if (foundDir && foundFile)
                    break;
            }
        }
        if (foundDir || foundFile)
        {
            event->acceptProposedAction();
            return;
        }
    }
    event->ignore();
}

void ReportGenerator::findAllReports(const QString &dir, QStringList *reports)
{
    foreach (QFileInfo info, QDir(dir).entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot))
    {
        if (info.isDir())
            findAllReports(info.absoluteFilePath(), reports);
        else if (info.isFile() && info.fileName().endsWith(".txt", Qt::CaseInsensitive))
            reports->append(info.absoluteFilePath());
        else
        {
        }
    }
}

void ReportGenerator::loadSettings()
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
    if (settings.contains("Tags"))
        m_editTags->setText(settings.value("Tags").toString());
    if (settings.contains("XML"))
        m_editXml->setText(settings.value("XML").toString());
}

void ReportGenerator::saveSettings()
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
    settings.setValue("Tags", m_editTags->text().trimmed());
    settings.setValue("XML", m_editXml->text().trimmed());
}
