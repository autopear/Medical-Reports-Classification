//https://bitbucket.org/smassung/porter2_stemmer/

#include "Porter2/porter2_stemmer.h"
#include "stemmer.h"

using namespace std;
using namespace Porter2Stemmer;

QString Stemmer::stemWord(const QString &word)
{
    string s = word.toStdString();
    stem(s);
    return QString::fromStdString(s);
}

static bool isLetter(const QChar &c)
{
    if (c >= 'a' && c<= 'z')
        return true;
    else
        return false;
}

static bool isDigit(const QChar &c)
{
    if (c >= '0' && c<= '9')
        return true;
    else
        return false;
}

static bool isLetterOrDigit(const QChar &c)
{
    return (isLetter(c) || isDigit(c));
}

static bool isNum(const QString &word)
{
    for (int i=0; i<word.size(); i++)
    {
        QChar c = word.at(i);

        if (!isDigit(c) && c != '-')
            return false;
    }
    return true;
}

QStringList Stemmer::stemSentence(const QString &sentence)
{
    //Split sentence with ' - ', but keep "A-B" as a single word
    QString copy = sentence.toLower();
    while (copy.contains(" - "))
        copy = copy.replace(" - ", " ");
    while (copy.contains(" -"))
        copy = copy.replace(" -", " ");
    while (copy.contains("- "))
        copy = copy.replace("- ", " ");

    QStringList list;
    QString word;
    for (int i=0; i<copy.size(); i++)
    {
        QChar c = copy.at(i);
        if (isLetterOrDigit(c))
            word = word.append(c);
        else if (c == '-')
            word = word.append(c);
        else if (word.isEmpty())
            continue;
        else if (word.size() < 2 || //Skip single character
                 isNum(word)) //Skip pure number
            word.clear();
        else
        {
            list.append(word);
            word.clear();
        }
    }

    QStringList stemList;
    foreach (QString word, list)
    {
        QString stemmed = stemWord(word);
        while (stemmed.contains("--"))
            stemmed = stemmed.replace("--", "-");
        if (stemmed.size() > 1)
            stemList.append(stemmed);
    }
    return stemList;
}

static bool isNumberSuffix(const QString &input, const QStringList &suffixes)
{
    foreach (QString suffix, suffixes)
    {
        if (input.endsWith(suffix))
        {
            QString copy = input;
            copy.chop(suffix.size());

            if (isNum(copy))
                return true;
        }
    }
    return false;
}

QStringList Stemmer::stemSentence(const QString &sentence,
                                  const QStringList &filterWords,
                                  const QStringList &filterNumerSuffixes)
{
    //Split sentence with ' - ', but keep "A-B" as a single word
    QString copy = sentence.toLower();
    while (copy.contains(" - "))
        copy = copy.replace(" - ", " ");
    while (copy.contains(" -"))
        copy = copy.replace(" -", " ");
    while (copy.contains("- "))
        copy = copy.replace("- ", " ");

    QStringList list;
    QString word;
    for (int i=0; i<copy.size(); i++)
    {
        QChar c = copy.at(i);
        if (isLetterOrDigit(c))
            word = word.append(c);
        else if (c == '-')
            word = word.append(c);
        else if (word.isEmpty())
            continue;
        else if (word.size() < 2 || //Skip single character
                 isNum(word) || //Skip pure number
                 isNumberSuffix(word, filterNumerSuffixes) || //Skip number plus a suffix
                 filterWords.contains(word)) //Skip user defined words
            word.clear();
        else
        {
            list.append(word);
            word.clear();
        }
    }

    QStringList stemList;
    foreach (QString word, list)
    {
        QString stemmed = stemWord(word);
        while (stemmed.contains("--"))
            stemmed = stemmed.replace("--", "-");
        if (stemmed.size() < 2 || //Skip single character
                isNum(stemmed) || //Skip pure number
                isNumberSuffix(stemmed, filterNumerSuffixes) || //Skip number plus a suffix
                filterWords.contains(stemmed)) //Skip user defined words
            continue;

        stemList.append(stemmed);
    }
    return stemList;
}
