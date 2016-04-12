#include <limits>
#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <qmath.h>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QSpinBox>
#include "documentsloader.h"
#include "idfcalculator.h"
#include "mainwindow.h"
#include "svmexporter.h"
#include "tfcalculator.h"
#include "wordsloader.h"
#include "exportsvmdialog.h"

ExportSVMDialog::ExportSVMDialog(const QString &words, MainWindow *parent) :
    QDialog(parent),
    m_window(parent),
    m_tfc(0),
    m_idfc(0)
{
    m_radioTF1 = new QRadioButton(tr("Binary"), this);
    m_radioTF2 = new QRadioButton(tr("Raw frequency"), this);
    m_radioTF3 = new QRadioButton(tr("log e normalization"), this);
    m_radioTF4 = new QRadioButton(tr("Double normalization 0.5"), this);

    m_groupTF = new QGroupBox(tr("Term Frequency"), this);
    QVBoxLayout *vGroupLayout = new QVBoxLayout(m_groupTF);
    vGroupLayout->addWidget(m_radioTF1);
    vGroupLayout->addWidget(m_radioTF2);
    vGroupLayout->addWidget(m_radioTF3);
    vGroupLayout->addWidget(m_radioTF4);
    m_groupTF->setLayout(vGroupLayout);

    m_labelMultiply = new QLabel("×", this);
    m_labelMultiply->setAlignment(Qt::AlignCenter);

    m_radioIDF1 = new QRadioButton(tr("Unary"), this);
    m_radioIDF2 = new QRadioButton(tr("Inverse document frequency"), this);
    m_radioIDF3 = new QRadioButton(tr("Inverse document frequency smooth"), this);
    m_radioIDF4 = new QRadioButton(tr("Probabilistic inverse document frequency"), this);

    m_groupIDF = new QGroupBox(tr("Inverse Document Frequency"), this);
    vGroupLayout = new QVBoxLayout(m_groupIDF);
    vGroupLayout->addWidget(m_radioIDF1);
    vGroupLayout->addWidget(m_radioIDF2);
    vGroupLayout->addWidget(m_radioIDF3);
    vGroupLayout->addWidget(m_radioIDF4);
    m_groupIDF->setLayout(vGroupLayout);

    m_checkWords = new QCheckBox(tr("Words"), this);
    m_checkWords->setTristate(true);
    m_checkWords->setCheckState(Qt::Unchecked);
    m_checkWords->setEnabled(false);

    m_checkDocs = new QCheckBox(tr("Documents"), this);
    m_checkDocs->setTristate(true);
    m_checkDocs->setCheckState(Qt::Unchecked);
    m_checkDocs->setEnabled(false);

    m_checkTF = new QCheckBox(tr("Word frequencies"), this);
    m_checkTF->setTristate(true);
    m_checkTF->setCheckState(Qt::Unchecked);
    m_checkTF->setEnabled(false);

    m_checkDF = new QCheckBox(tr("Document frequencies"), this);
    m_checkDF->setTristate(true);
    m_checkDF->setCheckState(Qt::Unchecked);
    m_checkDF->setEnabled(false);

    m_groupLoad = new QGroupBox(tr("Load"), this);
    vGroupLayout = new QVBoxLayout(m_groupLoad);
    vGroupLayout->addWidget(m_checkWords);
    vGroupLayout->addWidget(m_checkDocs);
    vGroupLayout->addWidget(m_checkTF);
    vGroupLayout->addWidget(m_checkDF);
    m_groupLoad->setLayout(vGroupLayout);

    m_labelDecimals = new QLabel(tr("Decimals:"), this);

    m_spinDecimals = new QSpinBox(this);
    m_spinDecimals->setRange(0, std::numeric_limits<qreal>::digits10);
    m_spinDecimals->setValue(4);

    m_checkEmpty = new QCheckBox(tr("Include 0 feature value"), this);
    m_checkEmpty->setChecked(false);

    m_groupOpt = new QGroupBox(tr("Export Options"), this);
    QGridLayout *gGroupLayout = new QGridLayout(m_groupOpt);
    gGroupLayout->addWidget(m_labelDecimals, 0, 0);
    gGroupLayout->addWidget(m_spinDecimals, 0, 1);
    gGroupLayout->addWidget(m_checkEmpty, 1, 0, 1, 2);
    gGroupLayout->setColumnStretch(0, 0);
    gGroupLayout->setColumnStretch(1, 0);
    gGroupLayout->setColumnStretch(2, 1);
    gGroupLayout->setRowStretch(0, 0);
    gGroupLayout->setRowStretch(1, 0);
    gGroupLayout->setRowStretch(2, 1);
    m_groupOpt->setLayout(gGroupLayout);

    m_bar = new QProgressBar(this);
    m_bar->setTextVisible(true);
    m_bar->setAlignment(Qt::AlignCenter);
    m_bar->setRange(0, 1);
    m_bar->setValue(0);

    m_btnLight = new QPushButton(tr("Export to SVM Light"), this);
    m_btnLight->setAutoDefault(false);
    m_btnLight->setDefault(false);
    connect(m_btnLight, SIGNAL(clicked(bool)),
            this, SLOT(exportLight()));

    m_btnRank = new QPushButton(tr("Export to SVM Rank"), this);
    m_btnRank->setAutoDefault(false);
    m_btnRank->setDefault(false);
    connect(m_btnRank, SIGNAL(clicked(bool)),
            this, SLOT(exportRank()));

    int w = qMax(m_btnLight->sizeHint().width(), m_btnRank->sizeHint().width());
    m_btnLight->setFixedWidth(w);
    m_btnRank->setFixedWidth(w);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setContentsMargins(0, 0, 0, 0);
    layout1->addWidget(m_groupTF);
    layout1->addWidget(m_labelMultiply);
    layout1->addWidget(m_groupIDF);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->addWidget(m_groupLoad);
    layout2->addStretch();
    layout2->addWidget(m_groupOpt);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addStretch();
    layout3->addWidget(m_btnLight);
    layout3->addStretch();
    layout3->addWidget(m_btnRank);
    layout3->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(layout1);
    layout->addLayout(layout2);
    layout->addWidget(m_bar);
    layout->addLayout(layout3);

    m_groupLoad->setFixedWidth(m_groupTF->sizeHint().width());
    m_groupOpt->setFixedWidth(m_groupIDF->sizeHint().width());

    setWindowTitle(tr("Export to SVM"));
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    setFixedSize(sizeHint());

    m_radioIDF1->setChecked(true);
    m_radioTF1->setChecked(true);
    m_btnLight->setEnabled(false);
    m_btnRank->setEnabled(false);

    loadSettings();

    m_wl = new WordsLoader(words, &m_mapWordID, &m_mapIDWord, 0);
    connect(m_wl, SIGNAL(started()),
            this, SLOT(onStarted()));
    connect(m_wl, SIGNAL(finished()),
            this, SLOT(onPreFinished()));
    m_dl = new DocumentsLoader(parent, &m_mapDocWords, &m_mapDocState, 0);
    connect(m_dl, SIGNAL(started()),
            this, SLOT(onStarted()));
    connect(m_dl, SIGNAL(finished()),
            this, SLOT(onPreFinished()));
    m_wl->start();
    m_dl->start();
}

ExportSVMDialog::~ExportSVMDialog()
{
    delete m_radioTF1;
    delete m_radioTF2;
    delete m_radioTF3;
    delete m_radioTF4;
    delete m_groupTF;

    delete m_labelMultiply;

    delete m_radioIDF1;
    delete m_radioIDF2;
    delete m_radioIDF3;
    delete m_radioIDF4;
    delete m_groupIDF;

    delete m_checkWords;
    delete m_checkDocs;
    delete m_checkTF;
    delete m_checkDF;
    delete m_groupLoad;

    delete m_labelDecimals;
    delete m_spinDecimals;
    delete m_checkEmpty;
    delete m_groupOpt;

    delete m_bar;

    delete m_btnLight;
    delete m_btnRank;
}

void ExportSVMDialog::onStarted()
{
    if (!sender())
        return;

    if (sender() == m_wl || sender() == m_dl)
        m_groupLoad->setTitle(tr("Loading..."));

    if (sender() == m_wl)
        m_checkWords->setCheckState(Qt::PartiallyChecked);
    if (sender() == m_dl)
        m_checkDocs->setCheckState(Qt::PartiallyChecked);
    if (sender() == m_tfc)
        m_checkTF->setCheckState(Qt::PartiallyChecked);
    if (sender() == m_idfc)
        m_checkDF->setCheckState(Qt::PartiallyChecked);
}

void ExportSVMDialog::onPreFinished()
{
    if (!sender())
        return;

    if (sender() == m_wl)
    {
        m_wl->deleteLater();
        m_wl = 0;
        m_wordCount = m_mapWordID.size();
        m_checkWords->setCheckState(Qt::Checked);
    }
    if (sender() == m_dl)
    {
        m_dl->deleteLater();
        m_dl = 0;
        m_docCount = m_mapDocWords.size();
        m_checkDocs->setCheckState(Qt::Checked);
        m_bar->setMaximum(m_docCount + 1);
    }

    if (!m_wl && !m_dl)
    {
        m_tfc = new TFCalculator(this, 0);
        connect(m_tfc, SIGNAL(started()),
                this, SLOT(onStarted()));
        connect(m_tfc, SIGNAL(finished()),
                this, SLOT(onPostFinished()));
        m_idfc = new IDFCalculator(this, 0);
        connect(m_idfc, SIGNAL(started()),
                this, SLOT(onStarted()));
        connect(m_idfc, SIGNAL(finished()),
                this, SLOT(onPostFinished()));
        m_tfc->start();
        m_idfc->start();
    }
}

void ExportSVMDialog::onPostFinished()
{
    if (!sender())
        return;

    if (sender() == m_tfc)
    {
        m_tfc->deleteLater();
        m_tfc = 0;
        m_checkTF->setCheckState(Qt::Checked);
    }
    if (sender() == m_idfc)
    {
        m_idfc->deleteLater();
        m_idfc = 0;
        m_checkDF->setCheckState(Qt::Checked);
    }

    if (!m_tfc && !m_idfc)
    {
        m_groupLoad->setTitle(tr("Loaded"));

        m_btnLight->setEnabled(true);
        m_btnRank->setEnabled(true);
        m_groupOpt->setEnabled(true);
    }
}

void ExportSVMDialog::updateProgressBar()
{
    m_bar->setValue(m_bar->value() + 1);
}

void ExportSVMDialog::onExportFinished()
{
    if (sender())
    {
        m_bar->setValue(m_bar->maximum());
        sender()->deleteLater();
        m_btnLight->setEnabled(true);
        m_btnRank->setEnabled(true);
        m_groupIDF->setEnabled(true);
        m_groupTF->setEnabled(true);
        m_groupOpt->setEnabled(true);
    }
}

void ExportSVMDialog::exportLight()
{
    QString defaultFile = m_lastLight;
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
                                                    tr("Export to SVM Light"),
                                                    defaultFile);
    if (!saveFile.isEmpty())
    {
        m_lastLight = saveFile;
        saveSettings();

        m_btnLight->setEnabled(false);
        m_btnRank->setEnabled(false);
        m_groupIDF->setEnabled(false);
        m_groupTF->setEnabled(false);
        m_groupOpt->setEnabled(false);

        m_bar->setValue(0);

        SVMExporter *exporter = new SVMExporter(this, saveFile, false, 0);
        connect(exporter, SIGNAL(progressUpdated()),
                this, SLOT(updateProgressBar()));
        connect(exporter, SIGNAL(finished()),
                this, SLOT(onExportFinished()));
        exporter->start();
    }
}

void ExportSVMDialog::exportRank()
{
    QString defaultFile = m_lastRank;
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
                                                    tr("Export to SVM Rank"),
                                                    defaultFile);
    if (!saveFile.isEmpty())
    {
        m_lastRank = saveFile;
        saveSettings();

        m_btnLight->setEnabled(false);
        m_btnRank->setEnabled(false);
        m_groupIDF->setEnabled(false);
        m_groupTF->setEnabled(false);
        m_groupOpt->setEnabled(false);

        m_bar->setValue(0);

        SVMExporter *exporter = new SVMExporter(this, saveFile, true, 0);
        connect(exporter, SIGNAL(progressUpdated()),
                this, SLOT(updateProgressBar()));
        connect(exporter, SIGNAL(finished()),
                this, SLOT(onExportFinished()));
        exporter->start();
    }
}

void ExportSVMDialog::loadSettings()
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
    if (settings.contains("TFWeight"))
    {
        switch (settings.value("TFWeight").toInt())
        {
        case 1:
            m_radioTF1->setChecked(true);
            break;
        case 2:
            m_radioTF2->setChecked(true);
            break;
        case 3:
            m_radioTF3->setChecked(true);
            break;
        case 4:
            m_radioTF4->setChecked(true);
            break;
        default:
            break;
        }
    }
    if (settings.contains("IDFWeight"))
    {
        switch (settings.value("IDFWeight").toInt())
        {
        case 1:
            m_radioIDF1->setChecked(true);
            break;
        case 2:
            m_radioIDF2->setChecked(true);
            break;
        case 3:
            m_radioIDF3->setChecked(true);
            break;
        case 4:
            m_radioIDF4->setChecked(true);
            break;
        default:
            break;
        }
    }
    if (settings.contains("SVMLight"))
        m_lastLight = settings.value("SVMLight").toString();
    if (settings.contains("SVMRank"))
        m_lastRank = settings.value("SVMRank").toString();
    if (settings.contains("Decimals"))
    {
        bool isInt;
        int v = settings.value("Decimals").toInt(&isInt);
        if (isInt && v >= m_spinDecimals->minimum() && v <= m_spinDecimals->maximum())
            m_spinDecimals->setValue(v);
    }
    if (settings.contains("Empties"))
        m_checkEmpty->setChecked(settings.value("Empties").toBool());
}

void ExportSVMDialog::saveSettings()
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
    if (m_radioTF1->isChecked())
        settings.setValue("TFWeight", 1);
    else if (m_radioTF2->isChecked())
        settings.setValue("TFWeight", 2);
    else if (m_radioTF3->isChecked())
        settings.setValue("TFWeight", 3);
    else if (m_radioTF4->isChecked())
        settings.setValue("TFWeight", 4);
    else
    {
    }
    if (m_radioIDF1->isChecked())
        settings.setValue("IDFWeight", 1);
    else if (m_radioIDF2->isChecked())
        settings.setValue("IDFWeight", 2);
    else if (m_radioIDF3->isChecked())
        settings.setValue("IDFWeight", 3);
    else if (m_radioIDF4->isChecked())
        settings.setValue("IDFWeight", 4);
    else
    {
    }
    settings.setValue("SVMLight", m_lastLight);
    settings.setValue("SVMRank", m_lastRank);
    settings.setValue("Decimals", m_spinDecimals->value());
    settings.setValue("Empties", m_checkEmpty->isChecked());
}

qreal ExportSVMDialog::getTFLogNorm(int count)
{
    return 1 + qLn(count);
}

qreal ExportSVMDialog::getTFDoubleNorm(int count, int max)
{
    return 0.5 + 0.5 * (qreal)count / max;
}

qreal ExportSVMDialog::getIDF(int count, int max)
{
    return qLn((qreal)max / count);
}

qreal ExportSVMDialog::getIDFSmooth(int count, int max)
{
    return qLn(1 + (qreal)max / count);
}

qreal ExportSVMDialog::getIDFP(int count, int max)
{
    return qLn((qreal)max / count - 1);
}

int ExportSVMDialog::findMax(QMap<QString, int> *map)
{
    int max = 0;
    foreach (int v, map->values())
        max = qMax(v, max);
    return max;
}
