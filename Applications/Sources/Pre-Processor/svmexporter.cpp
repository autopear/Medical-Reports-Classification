#include <QCheckBox>
#include <QFile>
#include <QRadioButton>
#include <QSpinBox>
#include "exportsvmdialog.h"
#include "svmexporter.h"

SVMExporter::SVMExporter(ExportSVMDialog *dialog,
                         const QString &file,
                         bool isRank,
                         QObject *parent) :
    QThread(parent),
    m_dialog(dialog),
    m_file(file),
    m_rank(isRank)
{
}

SVMExporter::~SVMExporter()
{
}

static QString realToStr(qreal v, int decimals)
{
    QString str = QString::number(v, 'f', decimals);
    while (str.contains('.') && str.endsWith('0'))
        str.chop(1);
    if (str.endsWith('.'))
        str.chop(1);
    return str;
}

static QString intToStr(int v)
{
    return QString::number(v, 10);
}

void SVMExporter::run()
{
    QFile *file = new QFile(m_file);
    file->open(QFile::WriteOnly);

    QStringList allWords(m_dialog->m_mapWordID.keys());

    QMap<QString, qreal> idfMap;
    foreach (QString word, allWords)
    {
        qreal idf;
        if (m_dialog->m_radioIDF1->isChecked())
            idf = 1;
        else if (m_dialog->m_radioIDF2->isChecked())
            idf = ExportSVMDialog::getIDF(m_dialog->m_mapDocCount.value(word) + 1, m_dialog->m_docCount);
        else if (m_dialog->m_radioIDF3->isChecked())
            idf = ExportSVMDialog::getIDFSmooth(m_dialog->m_mapDocCount.value(word) + 1, m_dialog->m_docCount);
        else
            idf = ExportSVMDialog::getIDFP(m_dialog->m_mapDocCount.value(word) + 1, m_dialog->m_docCount);

        idfMap.insert(word, idf);
    }

    emit progressUpdated();

    foreach (QString report, m_dialog->m_mapDocWords.keys())
    {
        QStringList words = m_dialog->m_mapDocWords.value(report);

        QString target = QString::number(m_dialog->m_mapDocState.value(report, 0), 10);

        QMap<QString, int> tmp = m_dialog->m_mapWordCount.value(report);
        int max = m_dialog->m_radioTF4->isChecked() ? ExportSVMDialog::findMax(&tmp) : 0;

        QMap<int, QString> sort;
        foreach (QString word, allWords)
        {
            int wordID = m_dialog->m_mapWordID.value(word);

            qreal tf;
            if (words.contains(word))
            {
                if (m_dialog->m_radioTF1->isChecked())
                    tf = 1;
                else if (m_dialog->m_radioTF2->isChecked())
                    tf = m_dialog->m_mapWordCount.value(report).value(word);
                else if (m_dialog->m_radioTF3->isChecked())
                    tf = ExportSVMDialog::getTFLogNorm(m_dialog->m_mapWordCount.value(report).value(word));
                else
                    tf = ExportSVMDialog::getTFDoubleNorm(tmp.value(word), max);
            }
            else
            {
                if (m_dialog->m_radioTF4->isChecked())
                    tf = 0.5;
                else
                    tf = 0;
            }

            qreal tf_idf = tf * idfMap.value(word);

            if (tf_idf == 0.0)
            {
                if (m_dialog->m_checkEmpty->isChecked())
                    sort.insert(wordID, QString("%1:%2").arg(intToStr(wordID)).arg(realToStr(tf_idf, m_dialog->m_spinDecimals->value())));
            }
            else
                sort.insert(wordID, QString("%1:%2").arg(intToStr(wordID)).arg(realToStr(tf_idf, m_dialog->m_spinDecimals->value())));
        }
        QStringList features;
        foreach (int index, sort.keys())
            features.append(sort.value(index));

        QString line;
        if (m_rank)
            line = QString("%1 qid:1 %2 # %3\n").arg(target).arg(features.join(" ")).arg(report);
        else
            line = QString("%1 %2 # %3\n").arg(target).arg(features.join(" ")).arg(report);

        file->write(line.toUtf8());

        emit progressUpdated();
    }

    file->close();
    delete file;
}
