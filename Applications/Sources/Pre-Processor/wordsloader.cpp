#include <QFile>
#include "wordsloader.h"

WordsLoader::WordsLoader(const QString &words,
                         QMap<QString, int> *map1,
                         QMap<int, QString> *map2,
                         QObject *parent) :
    QThread(parent),
    m_words(words),
    m_map1(map1),
    m_map2(map2)
{
}

WordsLoader::~WordsLoader()
{
}

void WordsLoader::run()
{
    QFile *file = new QFile(m_words);
    file->open(QFile::ReadOnly);

    int index = 1;
    while (!file->atEnd())
    {
        QString word = QString::fromUtf8(file->readLine()).remove("\r").remove("\n");
        if (!word.isEmpty())
        {
            m_map1->insert(word, index);
            m_map2->insert(index, word);
            index++;
        }
    }
    file->close();
    delete file;
}
