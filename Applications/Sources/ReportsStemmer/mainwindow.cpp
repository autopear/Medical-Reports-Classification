#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
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
#include <QListWidget>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QPushButton>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QTextBrowser>
#include <QUrl>
#include <QVBoxLayout>
#include "reportsparser.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_labelDir = new QLabel(tr("Directory:"), this);

    m_editDir = new QLineEdit(this);
    m_editDir->setPlaceholderText(tr("Drag a directory to the main window..."));

    m_btnDir = new QPushButton(tr("Select..."), this);
    connect(m_btnDir, SIGNAL(clicked(bool)),
            this, SLOT(selectDir()));

    m_listFiles = new QListWidget(this);
    m_listFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listFiles->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listFiles->setTextElideMode(Qt::ElideMiddle);
    m_listFiles->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_listFiles, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(onDoubleClicked(QListWidgetItem*)));
    connect(m_listFiles, SIGNAL(itemSelectionChanged()),
            this, SLOT(onReportSelected()));

    m_groupFiles = new QGroupBox(tr("Reports"), this);
    QVBoxLayout *groupLayout = new QVBoxLayout(m_groupFiles);
    groupLayout->addWidget(m_listFiles);
    m_groupFiles->setLayout(groupLayout);

    m_textFile = new QTextBrowser(this);
    m_textFile->setLineWrapMode(QTextBrowser::WidgetWidth);
    m_textFile->setAcceptRichText(false);

    m_groupFile = new QGroupBox(tr("Content"), this);
    groupLayout = new QVBoxLayout(m_groupFile);
    groupLayout->addWidget(m_textFile);
    m_groupFile->setLayout(groupLayout);

    m_textStem = new QTextBrowser(this);
    m_textStem->setLineWrapMode(QTextBrowser::WidgetWidth);
    m_textStem->setAcceptRichText(false);

    m_groupStem = new QGroupBox(tr("Stemmed Content"), this);
    groupLayout = new QVBoxLayout(m_groupStem);
    groupLayout->addWidget(m_textStem);
    m_groupStem->setLayout(groupLayout);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->addWidget(m_groupFiles);
    m_splitter->addWidget(m_groupFile);
    m_splitter->addWidget(m_groupStem);
    m_splitter->setChildrenCollapsible(false);

    m_bar = new QProgressBar(this);
    m_bar->setTextVisible(true);
    m_bar->setAlignment(Qt::AlignCenter);
    m_bar->setRange(0, 100);
    m_bar->setValue(0);

    m_checkSortStems = new QCheckBox(tr("Sorted Stems"), this);
    m_checkSortStems->setChecked(false);
    connect(m_checkSortStems, SIGNAL(clicked(bool)),
            this, SLOT(onChecked()));

    m_checkUniqueStems = new QCheckBox(tr("Unique Stems"), this);
    m_checkUniqueStems->setChecked(false);
    connect(m_checkUniqueStems, SIGNAL(clicked(bool)),
            this, SLOT(onChecked()));

    m_checkUniqueReport = new QCheckBox(tr("Unique Reports"), this);
    m_checkUniqueReport->setChecked(false);

    m_btnStem = new QPushButton(tr("Stem"), this);
    m_btnStem->setDefault(true);
    connect(m_btnStem, SIGNAL(clicked(bool)),
            this, SLOT(stem()));

    m_btnSave = new QPushButton(tr("Save"), this);
    connect(m_btnSave, SIGNAL(clicked(bool)),
            this, SLOT(save()));

    int w = qMax(m_btnStem->sizeHint().width(), m_btnSave->sizeHint().width());
    m_btnStem->setFixedWidth(w);
    m_btnSave->setFixedWidth(w);

    QHBoxLayout *top = new QHBoxLayout();
    top->setContentsMargins(0, 0, 0, 0);
    top->addWidget(m_labelDir, 0);
    top->addWidget(m_editDir, 1);
    top->addWidget(m_btnDir, 0);

    QHBoxLayout *bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 0, 0, 0);
    bottom->addWidget(m_checkSortStems);
    bottom->addWidget(m_checkUniqueStems);
    bottom->addWidget(m_checkUniqueReport);
    bottom->addStretch();
    bottom->addWidget(m_btnStem);
    bottom->addWidget(m_btnSave);

    m_main = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_main);
    layout->addLayout(top, 0);
    layout->addWidget(m_splitter, 1);
    layout->addWidget(m_bar);
    layout->addLayout(bottom);
    m_main->setLayout(layout);

    setCentralWidget(m_main);
    setWindowTitle(tr("Reports Stemmer"));
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setAcceptDrops(true);

    m_groupFiles->setEnabled(false);
    m_groupFile->setEnabled(false);
    m_groupStem->setEnabled(false);
    m_btnSave->setEnabled(false);

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete m_labelDir;
    delete m_editDir;
    delete m_btnDir;

    delete m_listFiles;
    delete m_groupFiles;

    delete m_textFile;
    delete m_groupFile;

    delete m_textStem;
    delete m_groupStem;

    delete m_splitter;

    delete m_bar;

    delete m_checkSortStems;
    delete m_checkUniqueStems;
    delete m_checkUniqueReport;

    delete m_btnStem;
    delete m_btnSave;

    delete m_main;
}

void MainWindow::selectDir()
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

void MainWindow::onDoubleClicked(QListWidgetItem *item)
{
    if (item)
        qApp->clipboard()->setText(item->text());
}

void MainWindow::onReportSelected()
{
    QList<QListWidgetItem *> selection = m_listFiles->selectedItems();

    if (selection.size() == 1)
    {
        QString report = selection.first()->text();
        QFile *file = new QFile(formatPath(QString("%1/%2").arg(m_editDir->text().trimmed()).arg(report)));
        file->open(QFile::ReadOnly);
        m_textFile->setText(QString::fromUtf8(file->readAll()));
        file->close();
        delete file;

    }
    else
        m_textFile->clear();

    refreshStemedContent();
}

void MainWindow::refreshStemedContent()
{
    QList<QListWidgetItem *> selection = m_listFiles->selectedItems();

    if (selection.size() == 1)
    {
        QString report = formatPath(selection.first()->text());

        QStringList stems = m_map.value(report);

        if (m_checkSortStems->isChecked())
            stems.sort();
        if (m_checkUniqueStems->isChecked())
            stems.removeDuplicates();

        m_textStem->setText(stems.join(" "));
    }
    else
        m_textStem->clear();
}

void MainWindow::onChecked()
{
    saveSettings();
    refreshStemedContent();
}

void MainWindow::stem()
{
    QString dir = formatPath(m_editDir->text().trimmed());

    if (dir.isEmpty())
    {
        QMessageBox::warning(this,
                             windowTitle(),
                             tr("The directory is not set."));
        m_editDir->setFocus();
        return;
    }

    saveSettings();

    m_map.clear();

    m_listFiles->clear();
    m_textFile->clear();
    m_textStem->clear();

    m_labelDir->setEnabled(false);
    m_editDir->setEnabled(false);
    m_btnDir->setEnabled(false);
    m_groupFiles->setEnabled(false);
    m_groupFile->setEnabled(false);
    m_groupStem->setEnabled(false);
    m_btnSave->setEnabled(false);
    m_btnStem->setEnabled(false);

    m_bar->setValue(0);

    ReportsParser *parser = new ReportsParser(dir, 0);
    connect(parser, SIGNAL(reportsListed(QStringList)),
            this, SLOT(onReporsListed(QStringList)));
    connect(parser, SIGNAL(reportStemmed(QString,QStringList)),
            this, SLOT(onReportStemmed(QString,QStringList)));
    connect(parser, SIGNAL(finished()),
            this, SLOT(onFinished()));

    parser->start();
}

void MainWindow::save()
{
    QString defaultFile = m_lastSave;
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

    QString saveFile = QFileDialog::getSaveFileName(this,
                                                    tr("Save"),
                                                    defaultFile);
    if (!saveFile.isEmpty())
    {
        saveSettings();

        QFile *file = new QFile(saveFile);
        file->open(QFile::WriteOnly);
        if (m_checkUniqueReport->isChecked())
        {
            QStringList stemsList;
            foreach (QString report, m_map.keys())
            {
                QStringList stems = m_map.value(report);

                if (m_checkSortStems->isChecked())
                    stems.sort();
                if (m_checkUniqueStems->isChecked())
                    stems.removeDuplicates();

                QString stemsStr = stems.join(" ");
                if (!stemsList.contains(stemsStr))
                {
                    stemsList.append(stemsStr);
                    file->write(QString("%1: %2\n").arg(report).arg(stemsStr).toUtf8());
                }
            }
        }
        else
        {
            foreach (QString report, m_map.keys())
            {
                QStringList stems = m_map.value(report);

                if (m_checkSortStems->isChecked())
                    stems.sort();
                if (m_checkUniqueStems->isChecked())
                    stems.removeDuplicates();

                file->write(QString("%1: %2\n").arg(report).arg(stems.join(" ")).toUtf8());
            }
        }
        file->close();
        delete file;
    }
}

void MainWindow::onReporsListed(const QStringList &reports)
{
    foreach (QString report, reports)
    {
        QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(report));
        item->setToolTip(item->text());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_listFiles->addItem(item);
    }

    m_bar->setValue(1);
}

void MainWindow::onReportStemmed(const QString &report, const QStringList &stems)
{
    m_map.insert(report, stems);

    m_bar->setValue((qreal)(m_map.size() * 99) / m_listFiles->count() + 1);
}

void MainWindow::onFinished()
{
    m_bar->setValue(100);

    m_labelDir->setEnabled(true);
    m_editDir->setEnabled(true);
    m_btnDir->setEnabled(true);
    m_btnStem->setEnabled(true);

    if (!m_map.isEmpty())
    {
        m_groupFiles->setEnabled(true);
        m_groupFile->setEnabled(true);
        m_groupStem->setEnabled(true);
        m_btnSave->setEnabled(true);

        m_listFiles->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
    }

    if (sender())
        sender()->deleteLater();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (m_editDir->isEnabled() &&  event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
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
    event->ignore();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (m_editDir->isEnabled() &&  event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
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
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (m_editDir->isEnabled() &&  event->mimeData()->urls().size() == 1)
    {
        QUrl url = event->mimeData()->urls().first();
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
    event->ignore();
}

QString MainWindow::formatPath(const QString &path)
{
    QString copy = path;
    return copy.replace("\\", "/");
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
    if (settings.contains("Save"))
        m_lastSave = settings.value("Save").toString();
    if (settings.contains("SortedStems"))
        m_checkSortStems->setChecked(settings.value("SortedStems").toBool());
    if (settings.contains("UniqueStems"))
        m_checkUniqueStems->setChecked(settings.value("UniqueStems").toBool());
    if (settings.contains("UniqueReportes"))
        m_checkUniqueReport->setChecked(settings.value("UniqueReportes").toBool());
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
    settings.setValue("Save", m_lastSave);
    settings.setValue("SortedStems", m_checkSortStems->isChecked());
    settings.setValue("UniqueStems", m_checkUniqueStems->isChecked());
    settings.setValue("UniqueReportes", m_checkUniqueReport->isChecked());
}
