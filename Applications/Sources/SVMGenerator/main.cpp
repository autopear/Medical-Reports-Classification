#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <qmath.h>
#include <QStringList>

static QStringList readReport(const QString &report)
{
    QFile *file = new QFile(report);
    file->open(QFile::ReadOnly);
    QStringList ret = QString::fromUtf8(file->readAll()).replace(QRegExp("[\r\n]"), " ").split(" ", QString::SkipEmptyParts);
    file->close();
    delete file;
    return ret;
}

static QString realToStr(qreal num)
{
    QString str = QString::number(num, 'f', 4);
    while (str.contains('.') && str.endsWith("0"))
        str.chop(1);
    if (str.endsWith('.'))
        str.chop(1);
    return str;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 4)
    {
        QFileInfo binInfo(QCoreApplication::applicationFilePath());
        printf(QString("Usage: %1 Dir Num_None TF\n").arg(binInfo.baseName()).toUtf8().data());
        return -1;
    }

    bool isInt;
    int numNone = QString(argv[2]).toInt(&isInt, 10);
    if (!isInt || numNone < 1)
    {
        printf("Number of reports in none is invalid.");
        return -1;

    }
    int tfOpt = QString(argv[3]).toInt(&isInt, 10);
    if (!isInt || tfOpt < 2 || tfOpt > 4)
    {
        printf("TF is invalid (2-4).");
        return -1;

    }

    QString dir(argv[1]);
    dir = QFileInfo(dir).absoluteFilePath();
    if (!dir.endsWith("/"))
        dir = dir.append('/');

    QDir dirNone(QString("%1none").arg(dir));
    QDir dirSeizures(QString("%1seizures").arg(dir));

    QMap<QString, QStringList> reportsMap;
    QMap<QString, int> wordsMap;
    QMap<QString, QMap<QString, int> > wordCountMap;
    QMap<QString, int> idfCountMap;
    QMap<QString, qreal> idfMap;
    QStringList allWords;

    foreach (QFileInfo info, dirNone.entryInfoList(QStringList() << "*.txt", QDir::Files | QDir::NoDotAndDotDot))
    {
        QString report = QString("none/%2").arg(info.fileName());
        QStringList words = readReport(info.absoluteFilePath());
        reportsMap.insert(report, words);

        QMap<QString, int> countMap;

        QStringList checkedWords;
        foreach (QString word, words)
        {
            if (!checkedWords.contains(word))
            {
                int count = idfMap.value(word, 0);
                idfMap.insert(word, count + 1);
                checkedWords.append(word);
            }

            if (!allWords.contains(word))
                allWords.append(word);

            int wordCount = countMap.value(word, 0);
            countMap.insert(word, wordCount + 1);
        }

        wordCountMap.insert(report, countMap);
    }

    foreach (QFileInfo info, dirSeizures.entryInfoList(QStringList() << "*.txt", QDir::Files | QDir::NoDotAndDotDot))
    {
        QString report = QString("seizures/%2").arg(info.fileName());
        QStringList words = readReport(info.absoluteFilePath());
        reportsMap.insert(report, words);

        QMap<QString, int> countMap;

        QStringList checkedWords;
        foreach (QString word, words)
        {
            if (!checkedWords.contains(word))
            {
                int count = idfMap.value(word, 0);
                idfMap.insert(word, count + 1);
                checkedWords.append(word);
            }

            if (!allWords.contains(word))
                allWords.append(word);

            int wordCount = countMap.value(word, 0);
            countMap.insert(word, wordCount + 1);
        }

        wordCountMap.insert(report, countMap);
    }

    int numReports = reportsMap.size();

    allWords.sort();
    QFile *wordsFile = new QFile(QString("%1words.list").arg(dir));
    wordsFile->open(QFile::WriteOnly);
    wordsFile->write(allWords.join("\n").toUtf8());
    wordsFile->close();
    delete wordsFile;

    for (int i=0; i<allWords.size(); i++)
    {
        QString word = allWords.at(i);
        wordsMap.insert(word, i + 1);

        qreal idf = qLn((qreal)numReports / (idfCountMap.value(word, 0) + 1));
        idfMap.insert(word, idf);
    }

    QDir svmDir(QString("%1SVMLight").arg(dir));
    if (!svmDir.exists())
        svmDir.mkpath(svmDir.absolutePath());

    QString trainPathNone = QString("%1none.train").arg(dir);
    QString trainPathSeizures = QString("%1seizures.train").arg(dir);
    QString svmPathTrain = QString("%1SVMLight/train_%2x2.svm").arg(dir).arg(QString::number(tfOpt, 10));

    QFile *svmFileTrain = new QFile(svmPathTrain);
    svmFileTrain->open(QFile::WriteOnly);

    //Train none
    QFile *fileTrain = new QFile(trainPathNone);
    fileTrain->open(QFile::ReadOnly);
    while (!fileTrain->atEnd())
    {
        QString report = QString::fromUtf8(fileTrain->readLine()).remove(QRegExp("[\r\n]"));
        if (reportsMap.keys().contains(report))
        {
            QStringList words = reportsMap.value(report);
            QMap<QString, int> countMap = wordCountMap.value(report);

            int maxCount = 0;
            if (tfOpt == 4)
            {
                foreach (int cnt, countMap.values())
                    maxCount = qMax(maxCount, cnt);
            }

            QMap<int, qreal> tmpMap;

            foreach (QString word, words)
            {
                qreal idf = idfMap.value(word);
                qreal tf = countMap.value(word);
                if (tfOpt == 3)
                    tf = 1 + qLn(1 + tf);
                else if (tfOpt == 4)
                    tf = 0.5 + 0.5 * tf / maxCount;
                else
                {}

                qreal tf_idf = tf * idf;
                tmpMap.insert(wordsMap.value(word), tf_idf);
            }

            QStringList values;
            foreach (int idx, tmpMap.keys())
                values.append(QString("%1:%2").arg(QString::number(idx, 10)).arg(realToStr(tmpMap.value(idx))));

            QString line = QString("-1 %1 # %2\n").arg(values.join(' ')).arg(report);
            svmFileTrain->write(line.toUtf8());
        }
    }
    fileTrain->close();
    delete fileTrain;

    //Train seizures
    fileTrain = new QFile(trainPathSeizures);
    fileTrain->open(QFile::ReadOnly);
    while (!fileTrain->atEnd())
    {
        QString report = QString::fromUtf8(fileTrain->readLine()).remove(QRegExp("[\r\n]"));
        if (reportsMap.keys().contains(report))
        {
            QStringList words = reportsMap.value(report);
            QMap<QString, int> countMap = wordCountMap.value(report);

            int maxCount = 0;
            if (tfOpt == 4)
            {
                foreach (int cnt, countMap.values())
                    maxCount = qMax(maxCount, cnt);
            }

            QMap<int, qreal> tmpMap;

            foreach (QString word, words)
            {
                qreal idf = idfMap.value(word);
                qreal tf = countMap.value(word);
                if (tfOpt == 3)
                    tf = 1 + qLn(1 + tf);
                else if (tfOpt == 4)
                    tf = 0.5 + 0.5 * tf / maxCount;
                else
                {}

                qreal tf_idf = tf * idf;
                tmpMap.insert(wordsMap.value(word), tf_idf);
            }

            QStringList values;
            foreach (int idx, tmpMap.keys())
                values.append(QString("%1:%2").arg(QString::number(idx, 10)).arg(realToStr(tmpMap.value(idx))));

            QString line = QString("1 %1 # %2\n").arg(values.join(' ')).arg(report);
            svmFileTrain->write(line.toUtf8());
        }
    }
    fileTrain->close();
    delete fileTrain;

    svmFileTrain->flush();
    svmFileTrain->close();
    delete svmFileTrain;

    QString testPathNone = QString("%1none.test").arg(dir);
    QString testPathSeizures = QString("%1seizures.test").arg(dir);
    QString svmPathTest = QString("%1SVMLight/test_%2x2.svm").arg(dir).arg(QString::number(tfOpt, 10));

    QFile *svmFileTest = new QFile(svmPathTest);
    svmFileTest->open(QFile::WriteOnly);

    //Test none
    QFile *fileTest = new QFile(testPathNone);
    fileTest->open(QFile::ReadOnly);
    while (!fileTest->atEnd())
    {
        QString report = QString::fromUtf8(fileTest->readLine()).remove(QRegExp("[\r\n]"));
        if (reportsMap.keys().contains(report))
        {
            QStringList words = reportsMap.value(report);
            QMap<QString, int> countMap = wordCountMap.value(report);

            int maxCount = 0;
            if (tfOpt == 4)
            {
                foreach (int cnt, countMap.values())
                    maxCount = qMax(maxCount, cnt);
            }

            QMap<int, qreal> tmpMap;

            foreach (QString word, words)
            {
                qreal idf = idfMap.value(word);
                qreal tf = countMap.value(word);
                if (tfOpt == 3)
                    tf = 1 + qLn(1 + tf);
                else if (tfOpt == 4)
                    tf = 0.5 + 0.5 * tf / maxCount;
                else
                {}

                qreal tf_idf = tf * idf;
                tmpMap.insert(wordsMap.value(word), tf_idf);
            }

            QStringList values;
            foreach (int idx, tmpMap.keys())
                values.append(QString("%1:%2").arg(QString::number(idx, 10)).arg(realToStr(tmpMap.value(idx))));

            QString line = QString("-1 %1 # %2\n").arg(values.join(' ')).arg(report);
            svmFileTest->write(line.toUtf8());
        }
    }
    fileTest->close();
    delete fileTest;

    //Test seizures
    fileTest = new QFile(testPathSeizures);
    fileTest->open(QFile::ReadOnly);
    while (!fileTest->atEnd())
    {
        QString report = QString::fromUtf8(fileTest->readLine()).remove(QRegExp("[\r\n]"));
        if (reportsMap.keys().contains(report))
        {
            QStringList words = reportsMap.value(report);
            QMap<QString, int> countMap = wordCountMap.value(report);

            int maxCount = 0;
            if (tfOpt == 4)
            {
                foreach (int cnt, countMap.values())
                    maxCount = qMax(maxCount, cnt);
            }

            QMap<int, qreal> tmpMap;

            foreach (QString word, words)
            {
                qreal idf = idfMap.value(word);
                qreal tf = countMap.value(word);
                if (tfOpt == 3)
                    tf = 1 + qLn(1 + tf);
                else if (tfOpt == 4)
                    tf = 0.5 + 0.5 * tf / maxCount;
                else
                {}

                qreal tf_idf = tf * idf;
                tmpMap.insert(wordsMap.value(word), tf_idf);
            }

            QStringList values;
            foreach (int idx, tmpMap.keys())
                values.append(QString("%1:%2").arg(QString::number(idx, 10)).arg(realToStr(tmpMap.value(idx))));

            QString line = QString("1 %1 # %2\n").arg(values.join(' ')).arg(report);
            svmFileTest->write(line.toUtf8());
        }
    }
    fileTest->close();
    delete fileTest;

    svmFileTest->flush();
    svmFileTest->close();
    delete svmFileTest;

    return 0;
}
