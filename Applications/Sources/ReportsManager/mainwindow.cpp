#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QDomDocument>
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
#include <QTextEdit>
#include <QUrl>
#include <QVariant>
#include <QVBoxLayout>
#include "propertylistwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_labelXml = new QLabel(tr("XML:"), this);

    m_editXml = new QLineEdit(this);
    m_editXml->setPlaceholderText(tr("Drag a XML file to the main window..."));
    connect(m_editXml, SIGNAL(returnPressed()),
            this, SLOT(loadXml()));
    connect(m_editXml, SIGNAL(textChanged(QString)),
            this, SLOT(onXmlChanged()));

    m_btnOpen = new QPushButton(tr("&Open"), this);
    m_btnOpen->setAutoDefault(false);
    m_btnOpen->setShortcut(QKeySequence::Open);
    connect(m_btnOpen, SIGNAL(clicked(bool)),
            this, SLOT(selectXml()));

    m_btnSave = new QPushButton(tr("&Save"), this);
    m_btnSave->setAutoDefault(false);
    m_btnSave->setShortcut(QKeySequence::Save);
    m_btnSave->setEnabled(false);
    connect(m_btnSave, SIGNAL(clicked(bool)),
            this, SLOT(saveXml()));

    m_listReports = new QListWidget(this);
    m_listReports->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listReports->setTextElideMode(Qt::ElideMiddle);
    m_listReports->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_listReports, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(onReportSelected(QListWidgetItem*,QListWidgetItem*)));

    m_groupReports = new QGroupBox(tr("Reports"), this);
    QVBoxLayout *groupLayout = new QVBoxLayout(m_groupReports);
    groupLayout->addWidget(m_listReports);
    m_groupReports->setLayout(groupLayout);

    m_labelState = new QLabel(tr("State:"), this);

    m_checkState = new QCheckBox(tr("Unknown"), this);
    m_checkState->setTristate(true);
    m_checkState->setCheckState(Qt::PartiallyChecked);
    connect(m_checkState, SIGNAL(clicked(bool)),
            this, SLOT(onStateChanged()));

    QHBoxLayout *stateLayout = new QHBoxLayout();
    stateLayout->setContentsMargins(0, 0, 0, 0);
    stateLayout->addWidget(m_labelState, 0);
    stateLayout->addWidget(m_checkState, 1);

    m_listProps = new PropertyListWidget(this);
    connect(m_listProps, SIGNAL(currentTextChanged(QString)),
            this, SLOT(onPropertySelected(QString)));
    connect(m_listProps, SIGNAL(propertyAdded(QString)),
            this, SLOT(onPropertyAdded(QString)));
    connect(m_listProps, SIGNAL(propertyChanged(QString,QString)),
            this, SLOT(onPropertyChanged(QString,QString)));
    connect(m_listProps, SIGNAL(propertyDeleted(QString)),
            this, SLOT(onPropertyDeleted(QString)));

    m_groupProps = new QGroupBox(tr("Properties"), this);
    groupLayout = new QVBoxLayout(m_groupProps);
    groupLayout->addLayout(stateLayout, 0);
    groupLayout->addWidget(m_listProps, 1);
    m_groupProps->setLayout(groupLayout);

    m_editValue = new QTextEdit(this);
    m_editValue->setAcceptRichText(false);
    m_editValue->setLineWrapMode(QTextEdit::WidgetWidth);
    connect(m_editValue, SIGNAL(textChanged()),
            this, SLOT(onValueEdited()));

    m_groupValue = new QGroupBox(tr("Value"), this);
    groupLayout = new QVBoxLayout(m_groupValue);
    groupLayout->addWidget(m_editValue);
    m_groupValue->setLayout(groupLayout);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->addWidget(m_groupReports);
    m_splitter->addWidget(m_groupProps);
    m_splitter->addWidget(m_groupValue);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setStretchFactor(2, 1);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, false);
    m_splitter->setCollapsible(2, false);

    m_main = new QWidget(this);

    QHBoxLayout *layoutXml = new QHBoxLayout();
    layoutXml->setContentsMargins(0, 0, 0, 0);
    layoutXml->addWidget(m_labelXml, 0);
    layoutXml->addWidget(m_editXml, 1);
    layoutXml->addWidget(m_btnOpen, 0);
    layoutXml->addWidget(m_btnSave, 0);

    QVBoxLayout *layout = new QVBoxLayout(m_main);
    layout->addLayout(layoutXml, 0);
    layout->addWidget(m_splitter, 1);
    m_main->setLayout(layout);

    setCentralWidget(m_main);

    setWindowTitle(tr("Reports Manager"));
    setAcceptDrops(true);

    m_groupReports->setEnabled(false);
    m_groupProps->setEnabled(false);
    m_groupValue->setEnabled(false);

    loadSettings();
    if (!m_editXml->text().trimmed().isEmpty())
        loadXml();
}

MainWindow::~MainWindow()
{
    delete m_labelXml;
    delete m_editXml;
    delete m_btnOpen;
    delete m_btnSave;

    delete m_listReports;
    delete m_groupReports;

    delete m_labelState;
    delete m_checkState;
    delete m_listProps;
    delete m_groupProps;

    delete m_editValue;
    delete m_groupValue;

    delete m_splitter;

    delete m_main;
}

void MainWindow::selectXml()
{
    QString xml = m_editXml->text().trimmed();
    if (xml.isEmpty() || !QFile::exists(xml))
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        xml = current.absolutePath();
    }

    xml = QFileDialog::getOpenFileName(this,
                                       tr("XML"),
                                       xml,
                                       "*.xml");

    if (!xml.isEmpty())
    {
        m_editXml->setText(QDir::toNativeSeparators(xml));
        loadXml();
    }
}

void MainWindow::saveXml()
{
    QFile *file = new QFile(m_editXml->text());
    if (!file->open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
    {
        QMessageBox::warning(this,
                             tr("Save XML"),
                             tr("Cannot open %1.\n\n%2").arg(QDir::toNativeSeparators(file->fileName())).arg(file->errorString()));
        return;
    }

    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

    QDomElement root = doc.createElement("Reports");
    doc.appendChild(root);

    foreach (Report report, m_reports.values())
        root.appendChild(report.toXml(doc));

    file->write(doc.toByteArray());

    file->close();
    delete file;
}

void MainWindow::onXmlChanged()
{
    if (m_editXml->text().trimmed().isEmpty())
    {
        m_btnSave->setEnabled(false);

        m_listReports->clear();
        m_groupProps->setEnabled(false);
        m_listProps->clearAllproperties();
        m_groupValue->setEnabled(false);
        m_editValue->clear();
    }
    else
        m_btnSave->setEnabled(true);
}

void MainWindow::onStateChanged()
{
    switch (m_checkState->checkState())
    {
    case Qt::Unchecked:
    {
        m_checkState->setText(tr("None"));
        m_currentReport.setPatientState(Report::None);
        m_reports.insert(m_currentReport.report(), m_currentReport);
        break;
    }
    case Qt::PartiallyChecked:
    {
        m_checkState->setText(tr("Unknown"));
        m_currentReport.setPatientState(Report::Unknown);
        m_reports.insert(m_currentReport.report(), m_currentReport);
        break;
    }
    case Qt::Checked:
    {
        m_checkState->setText(tr("Seizures"));
        m_currentReport.setPatientState(Report::Seizures);
        m_reports.insert(m_currentReport.report(), m_currentReport);
        break;
    }
    default:
        return;
    }
}

void MainWindow::loadXml()
{
    QString xml = m_editXml->text().trimmed();

    if (sender() && sender() == m_editXml && xml.isEmpty())
        return;

    if (xml.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Load XML"),
                             tr("The XML is not set."));
        m_editXml->setFocus();
        return;
    }

    if (!QFile::exists(xml))
    {
        QMessageBox::warning(this,
                             tr("Load XML"),
                             tr("Cannot find %1.").arg(QDir::toNativeSeparators(xml)));
        m_editXml->setFocus();
        return;
    }

    QFile *file = new QFile(xml);
    if (!file->open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,
                             tr("Load XML"),
                             tr("Failed to open %1.\n\n%2").arg(QDir::toNativeSeparators(xml)).arg(file->errorString()));
        m_editXml->setFocus();
        return;
    }

    QDomDocument doc;

    QString err;
    int line, column;
    if (!doc.setContent(file->readAll(), &err, &line, &column))
    {
        QMessageBox::warning(this,
                             tr("Load XML"),
                             tr("%1 is not a valid XML.\n\n%2\n\nLine: %3 Column: %4")
                             .arg(QDir::toNativeSeparators(xml))
                             .arg(err)
                             .arg(line)
                             .arg(column));
        m_editXml->setFocus();
        file->close();
        delete file;
        return;
    }

    file->close();
    delete file;

    m_reports.clear();

    QDomElement reportsNode = doc.firstChildElement("Reports");
    QDomNodeList reports = reportsNode.elementsByTagName("Report");
    for (int i=0; i<reports.size(); i++)
    {
        QDomElement reportElement = reports.at(i).toElement();

        Report report(formatPath(reportElement.attribute("file").trimmed()));
        if (report.report().isEmpty())
        {
            qWarning(tr("Report file is undefined at line %1.").arg(reportElement.lineNumber()).toUtf8());
            continue;
        }

        QString stateStr = reportElement.attribute("state");
        bool isInt;
        int state = stateStr.toInt(&isInt, 10);
        if (isInt)
        {
            switch (state)
            {
            case -1:
                report.setPatientState(Report::None);
                break;
            case 0:
                report.setPatientState(Report::Unknown);
                break;
            case 1:
                report.setPatientState(Report::Seizures);
                break;
            default:
            {
                report.setPatientState(Report::Unknown);
                qWarning(tr("Invalid patient state value %1 at line %2.").arg(state).arg(reportElement.lineNumber()).toUtf8());
                break;
            }
            }
        }
        else
            qWarning(tr("Invalid patient state value %1 at line %2.").arg(stateStr).arg(reportElement.lineNumber()).toUtf8());

        QDomNodeList propNodes = reportElement.elementsByTagName("Property");
        for (int j=0; j<propNodes.size(); j++)
        {
            QDomElement propNode = propNodes.at(j).toElement();
            report.insert(propNode.attribute("key").toLower(), propNode.attribute("value"));
        }

        m_reports.insert(report.report(), report);
    }

    m_listReports->clear();
    foreach (QString file, m_reports.keys())
    {
        QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(file));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        item->setToolTip(file);

        m_listReports->addItem(item);
    }

    m_groupReports->setEnabled(true);

    m_listReports->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);

    saveSettings();
}

void MainWindow::onReportSelected(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (!current)
        return;

    Report report = m_reports.value(formatPath(current->text()));

    switch (report.patientState())
    {
    case Report::None:
        m_checkState->setCheckState(Qt::Unchecked);
        break;
    case Report::Unknown:
        m_checkState->setCheckState(Qt::PartiallyChecked);
        break;
    case Report::Seizures:
        m_checkState->setCheckState(Qt::Checked);
        break;
    default:
        return;
    }

    m_currentReport = report;

    onStateChanged();

    m_listProps->clearAllproperties();
    m_listProps->addNewProperties(QStringList(report.keys()));
    m_groupProps->setEnabled(true);

    m_listProps->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
}

void MainWindow::onPropertySelected(const QString &prop)
{
    if (!m_currentReport.report().isEmpty() && m_currentReport.keys().contains(prop))
    {
        m_groupValue->setEnabled(true);
        m_editValue->setText(m_currentReport.value(prop));
    }
    else
    {
        m_editValue->clear();
        m_groupValue->setEnabled(false);
    }
}

void MainWindow::onPropertyChanged(const QString &oldProp, const QString &newProp)
{
    m_currentReport.insert(newProp, m_currentReport.take(oldProp));
    m_reports.insert(m_currentReport.report(), m_currentReport);
}

void MainWindow::onPropertyDeleted(const QString &prop)
{
    m_currentReport.remove(prop);
    m_reports.insert(m_currentReport.report(), m_currentReport);
    if (m_currentReport.isEmpty())
    {
        m_groupValue->setEnabled(false);
        m_editValue->clear();
    }
}

void MainWindow::onPropertyAdded(const QString &prop)
{
    m_currentReport.insert(prop, QString());
    m_reports.insert(m_currentReport.report(), m_currentReport);
}

void MainWindow::onValueEdited()
{
    if (m_groupValue->isEnabled() && m_listProps->currentItem())
    {
        QString newValue = m_editValue->toPlainText().replace("\r\n", "\n").replace("\r", "\n");
        while (newValue.endsWith("\n"))
            newValue.chop(1);

        m_currentReport.insert(m_listProps->currentItem()->text(), newValue);
        m_reports.insert(m_currentReport.report(), m_currentReport);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QFileInfo info(url.toLocalFile());
                if (info.isFile() && info.fileName().endsWith(".xml", Qt::CaseInsensitive))
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
    if (event->mimeData()->hasUrls())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QFileInfo info(url.toLocalFile());
                if (info.isFile() && info.fileName().endsWith(".xml", Qt::CaseInsensitive))
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
    if (event->mimeData()->hasUrls())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            if (url.isLocalFile())
            {
                QFileInfo info(url.toLocalFile());
                if (info.isFile() && info.fileName().endsWith(".xml", Qt::CaseInsensitive))
                {
                    m_editXml->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
                    loadXml();
                    event->acceptProposedAction();
                    return;
                }
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
    if (settings.contains("XML"))
        m_editXml->setText(settings.value("XML").toString());
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
    settings.setValue("XML", m_editXml->text().trimmed());
}

QString MainWindow::formatPath(const QString &path)
{
    QString ret = path;
    return ret.replace("\\", "/");
}
