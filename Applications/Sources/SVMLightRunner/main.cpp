#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStringList>
#include <QMap>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 2)
    {
        QFileInfo binInfo(QCoreApplication::applicationFilePath());
        printf(QString("Usage: %1 Dir\n").arg(binInfo.baseName()).toUtf8().data());
        return -1;
    }

    QString svmDir(argv[1]);
    svmDir = QFileInfo(svmDir).absoluteFilePath();
    if (!svmDir.endsWith('/'))
        svmDir = svmDir.append('/');

    if (!QDir(svmDir).exists())
    {
        printf(QString("Cannot find %1.").arg(QDir::toNativeSeparators(svmDir)).toUtf8().data());
        return -1;
    }

    QStringList pathComs = svmDir.split(QRegExp("[\\/]"), QString::SkipEmptyParts);
    int pathSize = pathComs.size();
    QString category = QString("%1/%2/%3")
            .arg(pathComs.at(pathSize - 4))
            .arg(pathComs.at(pathSize - 3))
            .arg(pathComs.at(pathSize - 2));

    QString appDir(QCoreApplication::applicationDirPath());
    if (!appDir.endsWith('/'))
        appDir = appDir.append('/');

#ifdef Q_OS_WIN32
    QString svmLearnPath = QString("%1svm_learn.exe").arg(appDir);
    QString svmClassifyPath = QString("%1svm_classify.exe").arg(appDir);
#else
    QString svmLearnPath = QString("%1svm_learn").arg(appDir);
    QString svmClassifyPath = QString("%1svm_classify").arg(appDir);
#endif

    if (!QFile::exists(svmLearnPath))
    {
        printf(QString("Cannot find %1.").arg(QDir::toNativeSeparators(svmLearnPath)).toUtf8().data());
        return -1;
    }
    if (!QFile::exists(svmClassifyPath))
    {
        printf(QString("Cannot find %1.").arg(QDir::toNativeSeparators(svmClassifyPath)).toUtf8().data());
        return -1;
    }

    QStringList results;
    results.append(category);

    for (int tf=2; tf<5; tf++)
    {
        QString trainPath = QString("%1train_%2").arg(svmDir).arg(QString::number(tf, 10));
        trainPath = trainPath.append("x2.svm");
        QString testPath = QString("%1test_%2").arg(svmDir).arg(QString::number(tf, 10));
        testPath = testPath.append("x2.svm");
        QString svmModelPath = QString("%1model_%2").arg(svmDir).arg(QString::number(tf, 10));
        svmModelPath = svmModelPath.append("x2.svm");
        QString svmOutPath = QString("%1output_%2").arg(svmDir).arg(QString::number(tf, 10));
        svmOutPath = svmOutPath.append("x2.svm");

        if (QFile::exists(svmModelPath))
            QFile::remove(svmModelPath);
        if (QFile::exists(svmOutPath))
            QFile::remove(svmOutPath);

        QProcess::execute(svmLearnPath, QStringList() << trainPath << svmModelPath);

        if (!QFile::exists(svmModelPath))
        {
            printf(QString("SVM Light learn failed for %1.").arg(QDir::toNativeSeparators(trainPath)).toUtf8().data());
            continue;
        }

        QProcess::execute(svmClassifyPath, QStringList() << testPath << svmModelPath << svmOutPath);

        if (!QFile::exists(svmOutPath))
        {
            printf(QString("SVM Light classify failed for %1.").arg(QDir::toNativeSeparators(testPath)).toUtf8().data());
            continue;
        }

        QStringList testSet;
        QMap<QString, bool> testSign, classifiedSign;

        QFile *testFile = new QFile(testPath);
        testFile->open(QFile::ReadOnly);
        while (!testFile->atEnd())
        {
            QString line = QString::fromUtf8(testFile->readLine()).remove(QRegExp("[\r\n]"));
            QString filename = line.mid(line.lastIndexOf('#') + 1).trimmed();
            testSet.append(filename);
            if (line.startsWith("-"))
                testSign.insert(filename, false);
            else
                testSign.insert(filename, true);
        }
        testFile->close();
        delete testFile;

        QFile *svmOutFile = new QFile(svmOutPath);
        svmOutFile->open(QFile::ReadOnly);
        int lineNum = 0;
        while (!svmOutFile->atEnd())
        {
            QString line = QString::fromUtf8(svmOutFile->readLine()).remove(QRegExp("[\r\n]"));
            if (line.isEmpty())
                break;

            if (lineNum >= testSet.size())
            {
                printf("Incorrect number of files classified.");
                break;
            }

            QString filename = testSet.at(lineNum);
            if (line.startsWith('-'))
                classifiedSign.insert(filename, false);
            else
                classifiedSign.insert(filename, true);

            lineNum++;
        }
        svmOutFile->close();
        delete svmOutFile;

        if (testSign.size() != classifiedSign.size())
        {
            printf("Incorrect number of files classified.");
            continue;
        }

        //Compute precision
        int matchedNone = 0, matchedSeizures = 0;
        QStringList mismatchedNone, mismatchedSeizures;
        foreach (QString filename, testSet)
        {
            bool sign1 = testSign.value(filename);
            bool sign2 = classifiedSign.value(filename);
            if (sign1 == sign2)
            {
                if (sign1)
                    matchedSeizures++;
                else
                    matchedNone++;
            }
            else if (sign1)
                mismatchedSeizures.append(filename);
            else
                mismatchedNone.append(filename);
        }

        qreal precesion = (qreal)(matchedNone + matchedSeizures) * 100 / testSet.size();
        qreal precisionNone = (qreal)matchedNone * 100 / (testSet.size() - 40);
        qreal precisionSeizures = (qreal)matchedSeizures * 100 / 40;

        QStringList tmp;
        tmp.append(QString("TF: %1").arg(QString::number(tf, 10)));
        tmp.append("IDF: 2");
        tmp.append(QString("Test Set Size: %1").arg(QString::number(testSet.size(), 10)));
        tmp.append(QString("Total Matched: %1").arg(QString::number(matchedNone + matchedSeizures, 10)));
        tmp.append(QString("\tNone Matched: %1").arg(QString::number(matchedNone, 10)));
        tmp.append(QString("\tSeizures Matched: %1").arg(QString::number(matchedSeizures, 10)));
        tmp.append(QString("Precision: %1%").arg(QString::number(precesion, 'f', 2)));
        tmp.append(QString("\tNone Precision: %1%").arg(QString::number(precisionNone, 'f', 2)));
        tmp.append(QString("\tSeizures Precision: %1%").arg(QString::number(precisionSeizures, 'f', 2)));
        tmp.append(QString("Total Mismatched: %1").arg(QString::number(mismatchedNone.size() + mismatchedSeizures.size(), 10)));
        tmp.append(QString("\tNone Mismatched: %1").arg(QString::number(mismatchedNone.size(), 10)));
        if (!mismatchedNone.isEmpty())
            tmp.append(QString("\t\t%1").arg(mismatchedNone.join("\n\t\t")));
        tmp.append(QString("\tSeizures Mismatched: %1").arg(QString::number(mismatchedSeizures.size(), 10)));
        if (!mismatchedSeizures.isEmpty())
            tmp.append(QString("\t\t%1").arg(mismatchedSeizures.join("\n\t\t")));

        results.append(tmp.join("\n"));
    }

    QFile *resFile = new QFile(QString("%1result.svm").arg(svmDir));
    resFile->open(QFile::WriteOnly);
    resFile->write(results.join("\n\n").toUtf8());
    resFile->flush();
    resFile->close();
    delete resFile;

    return 0;
}
