#include <QAction>
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
#include <QMenu>
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
#include "exportsvmdialog.h"
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

    m_actionNegative = new QAction(tr("As Negative (-1)"), this);
    connect(m_actionNegative, SIGNAL(triggered(bool)),
            this, SLOT(importNegative()));

    m_actionNeutral = new QAction(tr("As Neutral (0)"), this);
    connect(m_actionNeutral, SIGNAL(triggered(bool)),
            this, SLOT(importNeutral()));

    m_actionPositive = new QAction(tr("As Positive (+1)"), this);
    connect(m_actionPositive, SIGNAL(triggered(bool)),
            this, SLOT(importPositive()));

    m_menuImport = new QMenu(this);
    m_menuImport->addAction(m_actionNegative);
    m_menuImport->addAction(m_actionNeutral);
    m_menuImport->addAction(m_actionPositive);

    m_btnImport = new QPushButton(tr("Import"), this);
    m_btnImport->setMenu(m_menuImport);

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

    m_splitter1 = new QSplitter(Qt::Horizontal, this);
    m_splitter1->addWidget(m_groupFiles);
    m_splitter1->addWidget(m_groupFile);
    m_splitter1->addWidget(m_groupStem);
    m_splitter1->setChildrenCollapsible(false);

    m_textIgnoreWord = new QTextEdit(this);
    m_textIgnoreWord->setAcceptRichText(false);
    m_textIgnoreWord->setLineWrapMode(QTextEdit::WidgetWidth);

    m_groupIgnoreWord = new QGroupBox(tr("Ignored Words (Separate by space or new line)"), this);
    groupLayout = new QVBoxLayout(m_groupIgnoreWord);
    groupLayout->addWidget(m_textIgnoreWord);
    m_groupIgnoreWord->setLayout(groupLayout);

    m_textIgnoreSuffix = new QTextEdit(this);
    m_textIgnoreSuffix->setAcceptRichText(false);
    m_textIgnoreSuffix->setLineWrapMode(QTextEdit::WidgetWidth);

    m_groupIgnoreSuffix = new QGroupBox(tr("Ignored Number Suffix (Separate by space or new line)"), this);
    groupLayout = new QVBoxLayout(m_groupIgnoreSuffix);
    groupLayout->addWidget(m_textIgnoreSuffix);
    m_groupIgnoreSuffix->setLayout(groupLayout);

    m_splitter2 = new QSplitter(Qt::Horizontal, this);
    m_splitter2->addWidget(m_groupIgnoreWord);
    m_splitter2->addWidget(m_groupIgnoreSuffix);
    m_splitter2->setChildrenCollapsible(false);

    m_splitter3 = new QSplitter(Qt::Vertical, this);
    m_splitter3->addWidget(m_splitter1);
    m_splitter3->addWidget(m_splitter2);
    m_splitter3->setChildrenCollapsible(false);

    m_bar = new QProgressBar(this);
    m_bar->setTextVisible(true);
    m_bar->setAlignment(Qt::AlignCenter);
    m_bar->setRange(0, 100);
    m_bar->setValue(0);

    m_checkSortWords = new QCheckBox(tr("Sorted Words"), this);
    m_checkSortWords->setChecked(false);
    connect(m_checkSortWords, SIGNAL(clicked(bool)),
            this, SLOT(onChecked()));

    m_checkUniqueWords = new QCheckBox(tr("Unique Words"), this);
    m_checkUniqueWords->setChecked(false);
    connect(m_checkUniqueWords, SIGNAL(clicked(bool)),
            this, SLOT(onChecked()));

    m_checkUniqueReport = new QCheckBox(tr("Unique Reports"), this);
    m_checkUniqueReport->setChecked(false);

    m_btnStem = new QPushButton(tr("Stem"), this);
    m_btnStem->setDefault(true);
    connect(m_btnStem, SIGNAL(clicked(bool)),
            this, SLOT(stem()));

    m_actionReports = new QAction(tr("Reports"), this);
    connect(m_actionReports, SIGNAL(triggered(bool)),
            this, SLOT(saveReports()));

    m_actionWords = new QAction(tr("Words"), this);
    connect(m_actionWords, SIGNAL(triggered(bool)),
            this, SLOT(saveWords()));

    m_menuSave = new QMenu(this);
    m_menuSave->addAction(m_actionReports);
    m_menuSave->addAction(m_actionWords);

    m_btnSave = new QPushButton(tr("Save"), this);
    m_btnSave->setMenu(m_menuSave);

    m_actionText = new QAction(tr("To Text"), this);
    connect(m_actionText, SIGNAL(triggered(bool)),
            this, SLOT(exportToText()));

    m_actionFiles = new QAction(tr("To Files"), this);
    connect(m_actionFiles, SIGNAL(triggered(bool)),
            this, SLOT(exportToFiles()));

    m_actionSVM = new QAction(tr("To SVM"), this);
    connect(m_actionSVM, SIGNAL(triggered(bool)),
            this, SLOT(exportToSVM()));

    m_actionLDA = new QAction(tr("To LDA"), this);
    connect(m_actionLDA, SIGNAL(triggered(bool)),
            this, SLOT(exportToLDA()));

    m_menuExport = new QMenu(this);
    m_menuExport->addAction(m_actionText);
    m_menuExport->addAction(m_actionFiles);
    m_menuExport->addSeparator();
    m_menuExport->addAction(m_actionSVM);
    m_menuExport->addAction(m_actionLDA);

    m_btnExport = new QPushButton(tr("Export"), this);
    m_btnExport->setMenu(m_menuExport);

    int w = qMax(m_btnSave->sizeHint().width(), m_btnExport->sizeHint().width());
    m_btnSave->setFixedWidth(w);
    m_btnExport->setFixedWidth(w);

    QHBoxLayout *top = new QHBoxLayout();
    top->setContentsMargins(0, 0, 0, 0);
    top->addWidget(m_labelDir, 0);
    top->addWidget(m_editDir, 1);
    top->addWidget(m_btnDir, 0);
    top->addWidget(m_btnImport, 0);

    QHBoxLayout *bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 0, 0, 0);
    bottom->addWidget(m_checkSortWords);
    bottom->addWidget(m_checkUniqueWords);
    bottom->addWidget(m_checkUniqueReport);
    bottom->addStretch();
    bottom->addWidget(m_btnStem);
    bottom->addWidget(m_btnSave);
    bottom->addWidget(m_btnExport);

    m_main = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_main);
    layout->addLayout(top, 0);
    layout->addWidget(m_splitter3, 1);
    layout->addWidget(m_bar);
    layout->addLayout(bottom);
    m_main->setLayout(layout);

    setCentralWidget(m_main);
    setWindowTitle(tr("Pre-Processor"));
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setAcceptDrops(true);

    m_btnImport->setEnabled(false);
    m_groupFiles->setEnabled(false);
    m_groupFile->setEnabled(false);
    m_groupStem->setEnabled(false);
    m_btnSave->setEnabled(false);
    m_btnExport->setEnabled(false);

    loadSettings();
}

MainWindow::~MainWindow()
{
    delete m_labelDir;
    delete m_editDir;
    delete m_btnDir;

    delete m_actionNegative;
    delete m_actionNeutral;
    delete m_actionPositive;
    delete m_menuImport;

    delete m_btnImport;

    delete m_listFiles;
    delete m_groupFiles;

    delete m_textFile;
    delete m_groupFile;

    delete m_textStem;
    delete m_groupStem;

    delete m_splitter1;

    delete m_textIgnoreWord;
    delete m_groupIgnoreWord;

    delete m_textIgnoreSuffix;
    delete m_groupIgnoreSuffix;

    delete m_splitter2;

    delete m_splitter3;

    delete m_bar;

    delete m_checkSortWords;
    delete m_checkUniqueWords;
    delete m_checkUniqueReport;

    delete m_actionReports;
    delete m_actionWords;
    delete m_menuSave;

    delete m_actionText;
    delete m_actionFiles;
    delete m_actionSVM;
    delete m_actionLDA;
    delete m_menuExport;

    delete m_btnStem;
    delete m_btnSave;
    delete m_btnExport;

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

        if (m_checkSortWords->isChecked())
            stems.sort();
        if (m_checkUniqueWords->isChecked())
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
    m_groupFiles->setTitle(tr("Reports"));

    m_labelDir->setEnabled(false);
    m_editDir->setEnabled(false);
    m_btnDir->setEnabled(false);
    m_btnImport->setEnabled(false);
    m_groupFiles->setEnabled(false);
    m_groupFile->setEnabled(false);
    m_groupStem->setEnabled(false);
    m_groupIgnoreWord->setEnabled(false);
    m_btnSave->setEnabled(false);
    m_btnStem->setEnabled(false);
    m_btnExport->setEnabled(false);
    m_btnStem->setText(tr("Stemming..."));

    m_bar->setValue(0);

    QStringList ignores1 = m_textIgnoreWord->toPlainText().toLower().split(" ", QString::SkipEmptyParts);
    ignores1.removeDuplicates();
    ignores1.sort();

    QStringList ignores2 = m_textIgnoreSuffix->toPlainText().toLower().split(" ", QString::SkipEmptyParts);
    ignores2.removeDuplicates();
    ignores2.sort();

    ReportsParser *parser = new ReportsParser(dir, ignores1, ignores2, 0);
    connect(parser, SIGNAL(reportsListed(QStringList)),
            this, SLOT(onReporsListed(QStringList)));
    connect(parser, SIGNAL(reportStemmed(QString,QStringList)),
            this, SLOT(onReportStemmed(QString,QStringList)));
    connect(parser, SIGNAL(finished()),
            this, SLOT(onFinished()));

    parser->start();
}

void MainWindow::saveReports()
{
    QString defaultFile = m_lastReports;
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
                                                    tr("Save Reports"),
                                                    defaultFile);
    if (!saveFile.isEmpty())
    {
        m_lastReports = saveFile;
        saveSettings();

        QFile *file = new QFile(saveFile);
        file->open(QFile::WriteOnly);
        if (m_checkUniqueReport->isChecked())
        {
            QStringList stemsList;
            foreach (QString report, m_map.keys())
            {
                QStringList stems = m_map.value(report);

                if (m_checkSortWords->isChecked())
                    stems.sort();
                if (m_checkUniqueWords->isChecked())
                    stems.removeDuplicates();

                QString stemsStr = stems.join(" ");
                if (!stemsList.contains(stemsStr))
                {
                    stemsList.append(stemsStr);
                    file->write(report.append("\n").toUtf8());
                }
            }
        }
        else
        {
            foreach (QString report, m_map.keys())
                file->write(report.append("\n").toUtf8());
        }
        file->close();
        delete file;
    }
}

void MainWindow::saveWords()
{
    QString defaultFile = m_lastSaveWords;
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
                                                    tr("Save Words"),
                                                    defaultFile);
    if (!saveFile.isEmpty())
    {
        m_lastSaveWords = saveFile;
        saveSettings();

        QFile *file = new QFile(saveFile);
        file->open(QFile::WriteOnly);

        QStringList words;
        foreach (QString report, m_map.keys())
            words.append(m_map.value(report));

        words.removeDuplicates();
        words.sort();

        foreach (QString word, words)
            file->write(word.append("\n").toUtf8());

        file->close();
        delete file;
    }
}

void MainWindow::exportToText()
{
    QString defaultFile = m_lastText;
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
        m_lastText = saveFile;
        saveSettings();

        QFile *file = new QFile(saveFile);
        file->open(QFile::WriteOnly);
        if (m_checkUniqueReport->isChecked())
        {
            QStringList stemsList;
            foreach (QString report, m_map.keys())
            {
                QStringList stems = m_map.value(report);

                if (m_checkSortWords->isChecked())
                    stems.sort();
                if (m_checkUniqueWords->isChecked())
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

                if (m_checkSortWords->isChecked())
                    stems.sort();
                if (m_checkUniqueWords->isChecked())
                    stems.removeDuplicates();

                file->write(QString("%1: %2\n").arg(report).arg(stems.join(" ")).toUtf8());
            }
        }
        file->close();
        delete file;
    }
}

static QString getFileName(const QString &path)
{
    QStringList tmp = path.split(QRegExp("[\\/]"), QString::SkipEmptyParts);
    return tmp.last();
}

void MainWindow::exportToFiles()
{
    QString defaultDir = m_lastFiles;
    if (defaultDir.isEmpty() || !QDir(defaultDir).exists())
    {
        QDir current(qApp->applicationDirPath());
#ifdef Q_OS_MAC
        current.cdUp();
        current.cdUp();
        current.cdUp();
#endif
        defaultDir = current.absolutePath();
    }

    QString saveDir = QFileDialog::getExistingDirectory(this,
                                                        tr("Save"),
                                                        defaultDir);
    if (!saveDir.isEmpty())
    {
        m_lastFiles = saveDir;
        saveSettings();

        QDir dir(saveDir);
        if (!dir.exists("Negative"))
            dir.mkdir("Negative");
        if (!dir.exists("Neutral"))
            dir.mkdir("Neutral");
        if (!dir.exists("Positive"))
            dir.mkdir("Positive");

        if (m_checkUniqueReport->isChecked())
        {
            QStringList stemsList;
            for (int i=0; i<m_listFiles->count(); i++)
            {
                QListWidgetItem *item = m_listFiles->item(i);
                QString report = item->text().replace("\\", "/");

                QStringList stems = m_map.value(report);

                if (m_checkSortWords->isChecked())
                    stems.sort();
                if (m_checkUniqueWords->isChecked())
                    stems.removeDuplicates();

                QString stemsStr = stems.join(" ");
                if (!stemsList.contains(stemsStr))
                {
                    stemsList.append(stemsStr);

                    QString savePath;
                    switch (item->checkState())
                    {
                    case Qt::Unchecked:
                        savePath = dir.absoluteFilePath(QString("Negative/%1").arg(getFileName(report)));
                        break;
                    case Qt::Checked:
                        savePath = dir.absoluteFilePath(QString("Positive/%1").arg(getFileName(report)));
                        break;
                    default:
                        savePath = dir.absoluteFilePath(QString("Neutral/%1").arg(getFileName(report)));
                        break;
                    }

                    QFile *file = new QFile(savePath);
                    file->open(QFile::WriteOnly);
                    file->write(stemsStr.toUtf8());
                    file->close();
                    delete file;
                }
            }
        }
        else
        {
            for (int i=0; i<m_listFiles->count(); i++)
            {
                QListWidgetItem *item = m_listFiles->item(i);
                QString report = item->text().replace("\\", "/");

                QStringList stems = m_map.value(report);

                if (m_checkSortWords->isChecked())
                    stems.sort();
                if (m_checkUniqueWords->isChecked())
                    stems.removeDuplicates();

                QString stemsStr = stems.join(" ");

                QString savePath;
                switch (item->checkState())
                {
                case Qt::Unchecked:
                    savePath = dir.absoluteFilePath(QString("Negative/%1").arg(getFileName(report)));
                    break;
                case Qt::Checked:
                    savePath = dir.absoluteFilePath(QString("Positive/%1").arg(getFileName(report)));
                    break;
                default:
                    savePath = dir.absoluteFilePath(QString("Neutral/%1").arg(getFileName(report)));
                    break;
                }

                QFile *file = new QFile(savePath);
                file->open(QFile::WriteOnly);
                file->write(stemsStr.toUtf8());
                file->close();
                delete file;
            }
        }
    }
}

void MainWindow::exportToSVM()
{
    QString defaultFile = m_lastOpenWords;
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

    QString words = QFileDialog::getOpenFileName(this,
                                                 tr("Open Words"),
                                                 defaultFile,
                                                 "*");

    if (!words.isEmpty())
    {
        m_lastOpenWords = words;
        saveSettings();

        ExportSVMDialog *dlg = new ExportSVMDialog(words, this);
        dlg->exec();
        delete dlg;
    }
}

void MainWindow::exportToLDA()
{
    QString defaultFile = m_lastLDA;
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
                                                    tr("Export to LDA"),
                                                    defaultFile);
    if (!saveFile.isEmpty())
    {
        m_lastLDA = saveFile;
        saveSettings();

        QFile *file = new QFile(saveFile);
        file->open(QFile::WriteOnly);
        if (m_checkUniqueReport->isChecked())
        {
            QStringList stemsList;
            foreach (QString report, m_map.keys())
            {
                QStringList stems = m_map.value(report);

                if (m_checkSortWords->isChecked())
                    stems.sort();
                if (m_checkUniqueWords->isChecked())
                    stems.removeDuplicates();

                QString stemsStr = stems.join(" ");
                if (!stemsList.contains(stemsStr))
                    stemsList.append(stemsStr);
            }

            file->write(QString::number(stemsList.size(), 10).append('\n').toUtf8());
            foreach (QString stemStr, stemsList)
                file->write(stemStr.append('\n').toUtf8());
        }
        else
        {
            file->write(QString::number(m_map.size(), 10).append('\n').toUtf8());

            foreach (QString report, m_map.keys())
            {
                QStringList stems = m_map.value(report);

                if (m_checkSortWords->isChecked())
                    stems.sort();
                if (m_checkUniqueWords->isChecked())
                    stems.removeDuplicates();

                file->write(stems.join(" ").append('\n').toUtf8());
            }
        }
        file->close();
        delete file;
    }
}

void MainWindow::onReporsListed(const QStringList &reports)
{
    if (!reports.isEmpty())
        m_groupFiles->setTitle(tr("Reports (%1)").arg(QString::number(reports.size(), 10)));

    foreach (QString report, reports)
    {
        QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(report));
        item->setToolTip(item->text());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsUserTristate);
        item->setCheckState(Qt::PartiallyChecked);
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
    m_btnImport->setEnabled(true);
    m_btnStem->setEnabled(true);
    m_groupIgnoreWord->setEnabled(true);
    m_btnStem->setText(tr("Stem"));

    if (!m_map.isEmpty())
    {
        m_groupFiles->setEnabled(true);
        m_groupFile->setEnabled(true);
        m_groupStem->setEnabled(true);
        m_btnSave->setEnabled(true);
        m_btnExport->setEnabled(true);

        m_listFiles->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
    }

    if (sender())
        sender()->deleteLater();
}

void MainWindow::importNegative()
{
    importFile(-1);
}

void MainWindow::importNeutral()
{
    importFile(0);
}

void MainWindow::importPositive()
{
    importFile(1);
}

void MainWindow::importFile(int sign)
{
    QString defaultFile = m_lastImport;
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

    QString title;
    switch (sign)
    {
    case -1:
        title = tr("Import as Negative (-1)");
        break;
    case 1:
        title = tr("Import as Positive (+1)");
        break;
    default:
        title = tr("Import as Neutral (0)");
        break;
    }

    QString importFile = QFileDialog::getOpenFileName(this,
                                                      title,
                                                      defaultFile,
                                                      tr("*"));

    if (importFile.isEmpty())
        return;

    m_lastImport = importFile;
    saveSettings();

    QFile *file = new QFile(importFile);
    if (file->open(QFile::ReadOnly))
    {
        QStringList reports;
        while (!file->atEnd())
        {
            QString line = QString::fromUtf8(file->readLine()).remove("\r").remove("\n").trimmed();
            int idxComment = line.indexOf("#");
            if (idxComment > -1)
                line = line.left(idxComment).trimmed();
            if (line.isEmpty())
                continue;

            reports.append(formatPath(line));
        }

        file->close();

        for (int i=0; i<m_listFiles->count(); i++)
        {
            QListWidgetItem *item = m_listFiles->item(i);
            QString text = formatPath(item->text());
            if (reports.contains(text))
            {
                switch (sign)
                {
                case -1:
                    item->setCheckState(Qt::Unchecked);
                    break;
                case 1:
                    item->setCheckState(Qt::Checked);
                    break;
                default:
                    item->setCheckState(Qt::PartiallyChecked);
                    break;
                }
            }
        }
    }
    delete file;
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
    if (settings.contains("SaveReports"))
        m_lastReports = settings.value("SaveReports").toString();
    if (settings.contains("SaveWords"))
        m_lastSaveWords = settings.value("SaveWords").toString();
    if (settings.contains("OpenWords"))
        m_lastOpenWords = settings.value("OpenWords").toString();
    if (settings.contains("Text"))
        m_lastText = settings.value("Text").toString();
    if (settings.contains("Files"))
        m_lastFiles = settings.value("Files").toString();
    if (settings.contains("LDA"))
        m_lastLDA = settings.value("LDA").toString();
    if (settings.contains("SortedStems"))
        m_checkSortWords->setChecked(settings.value("SortedStems").toBool());
    if (settings.contains("UniqueStems"))
        m_checkUniqueWords->setChecked(settings.value("UniqueStems").toBool());
    if (settings.contains("UniqueReportes"))
        m_checkUniqueReport->setChecked(settings.value("UniqueReportes").toBool());
    if (settings.contains("IgnoreWord"))
        m_textIgnoreWord->setText(settings.value("IgnoreWord").toStringList().join(" "));
    if (settings.contains("IgnoreSuffix"))
        m_textIgnoreSuffix->setText(settings.value("IgnoreSuffix").toStringList().join(" "));
    if (settings.contains("Import"))
        m_lastImport = settings.value("Import").toString();
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
    settings.setValue("SaveReports", m_lastReports);
    settings.setValue("SaveWords", m_lastSaveWords);
    settings.setValue("OpenWords", m_lastOpenWords);
    settings.setValue("Text", m_lastText);
    settings.setValue("Files", m_lastFiles);
    settings.setValue("LDA", m_lastLDA);
    settings.setValue("SortedStems", m_checkSortWords->isChecked());
    settings.setValue("UniqueStems", m_checkUniqueWords->isChecked());
    settings.setValue("UniqueReportes", m_checkUniqueReport->isChecked());
    QStringList words = m_textIgnoreWord->toPlainText().toLower().split(QRegExp("[\t \r\n]"), QString::SkipEmptyParts);
    words.removeDuplicates();
    words.sort();
    settings.setValue("IgnoreWord", words);
    QStringList suffixes = m_textIgnoreSuffix->toPlainText().toLower().split(QRegExp("[\t \r\n]"), QString::SkipEmptyParts);
    suffixes.removeDuplicates();
    suffixes.sort();
    settings.setValue("IgnoreSuffix", suffixes);
    settings.setValue("Import", m_lastImport);
}
