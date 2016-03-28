#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include "elidiblelabel.h"
#include "batchloaddialog.h"

BatchLoadDialog:: BatchLoadDialog(const QStringList &files, QWidget *parent) :
    QDialog(parent)
{
    QStringList sorted = files;
    sorted.removeDuplicates();
    sorted.sort();

    if (sorted.size() == 1)
    {
        m_labelFile = new QLabel(tr("Import File"), this);
        m_labelFile->setStyleSheet(tr("QLabel { font: bold; }"));

        m_labelSingle = new ElidibleLabel(this);
        m_labelSingle->setText(QDir::toNativeSeparators(files.first()));

        m_listMulti = 0;
    }
    else
    {
        m_labelFile = new QLabel(tr("Import Files"), this);
        m_labelFile->setStyleSheet(tr("QLabel { font: bold; }"));

        m_listMulti = new QListWidget(this);
        m_listMulti->setTextElideMode(Qt::ElideMiddle);
        m_listMulti->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_listMulti->setFixedHeight(m_listMulti->minimumSizeHint().height());

        foreach (QString file, sorted)
        {
            QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(file));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            item->setToolTip(item->text());
            m_listMulti->addItem(item);
        }

        m_labelSingle = 0;
    }

    m_labelReports = new QLabel(tr("Reports"), this);
    m_labelReports->setStyleSheet(tr("QLabel { font: bold; }"));

    m_listReports = new QListWidget(this);
    m_listReports->setTextElideMode(Qt::ElideMiddle);
    m_listReports->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //Scan for reports
    foreach (QString file, sorted)
    {
        QFile *list = new QFile(file, this);
        if (!list->open(QFile::ReadOnly | QFile::Text))
        {
            delete list;
            continue;
        }

        while (!list->atEnd())
        {
            QString line = QString::fromUtf8(list->readLine()).remove("\r").remove("\n").trimmed();
            if (!line.isEmpty() && !line.startsWith("//") && !line.startsWith("#"))
            {
                line = line.replace("\\", "/");
                if (!m_reports.contains(line))
                    m_reports.append(line);
            }
        }

        list->close();
        delete list;
    }

    if (!m_reports.isEmpty())
    {
        m_reports.sort();

        foreach (QString report, m_reports)
        {
            QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(report));
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            item->setToolTip(item->text());
            m_listReports->addItem(item);
        }
    }

    m_labelState = new QLabel(tr("Set Patient State to"), this);
    m_labelState->setStyleSheet(tr("QLabel { font: bold; }"));

    m_radioNone = new QRadioButton(tr("&None"), this);

    m_radioUnknown = new QRadioButton(tr("&Unknown"), this);

    m_radioSeizures = new QRadioButton(tr("&Seizures"), this);

    m_radioUnknown->setChecked(true);

    m_btnImport = new QPushButton(tr("&Import"), this);
    m_btnImport->setShortcut(QKeySequence::New);
    connect(m_btnImport, SIGNAL(clicked(bool)),
            this, SLOT(accept()));

    m_btnCancel = new QPushButton(tr("&Cancel"), this);
    m_btnCancel->setShortcut(QKeySequence::Cancel);
    connect(m_btnCancel, SIGNAL(clicked(bool)),
            this, SLOT(reject()));

    int w = qMax(m_btnCancel->sizeHint().width(), m_btnCancel->sizeHint().width());
    m_btnImport->setFixedWidth(w);
    m_btnCancel->setFixedWidth(w);

    if (m_reports.isEmpty())
    {
        m_btnCancel->setDefault(true);
        m_btnImport->setEnabled(false);
    }
    else
        m_btnImport->setDefault(true);

    QHBoxLayout *radiosLayout = new QHBoxLayout();
    radiosLayout->setContentsMargins(0, 0, 0, 0);
    radiosLayout->addWidget(m_radioNone);
    radiosLayout->addWidget(m_radioUnknown);
    radiosLayout->addWidget(m_radioSeizures);
    radiosLayout->addStretch();

    QHBoxLayout *btnsLayout = new QHBoxLayout();
    btnsLayout->setContentsMargins(0, 0, 0, 0);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnImport);
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_btnCancel);
    btnsLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_labelFile, 0);
    if (sorted.size() == 1)
        layout->addWidget(m_labelSingle, 0);
    else
        layout->addWidget(m_listMulti, 0);
    layout->addWidget(m_labelReports, 0);
    layout->addWidget(m_listReports, 1);
    layout->addWidget(m_labelState, 0);
    layout->addLayout(radiosLayout, 0);
    layout->addLayout(btnsLayout, 0);
    setLayout(layout);

    setWindowTitle(tr(sorted.size() == 1 ? "Import" : "Batch Import"));
    setSizeGripEnabled(true);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
}

BatchLoadDialog::~BatchLoadDialog()
{
    delete m_labelFile;
    if (m_labelSingle)
        delete m_labelSingle;
    if (m_listMulti)
        delete m_listMulti;
    delete m_labelReports;
    delete m_listReports;
    delete m_labelState;
    delete m_radioNone;
    delete m_radioUnknown;
    delete m_radioSeizures;
    delete m_btnImport;
    delete m_btnCancel;

}

Report::State BatchLoadDialog::state() const
{
    if (m_radioNone->isChecked())
        return Report::None;
    else if (m_radioSeizures->isChecked())
        return Report::Seizures;
    else
        return Report::Unknown;
}

QStringList BatchLoadDialog::reports() const
{
    return m_reports;
}
