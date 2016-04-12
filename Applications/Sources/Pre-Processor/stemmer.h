#ifndef STEMMER_H
#define STEMMER_H

#include <QString>
#include <QStringList>

class Stemmer
{
public:
    static QString stemWord(const QString &word);
    static QStringList stemSentence(const QString &sentence);
    static QStringList stemSentence(const QString &sentence,
                                    const QStringList &filterWords,
                                    const QStringList &filterNumerSuffixes);

    static QStringList stopWords();
};

#endif // STEMMER_H
