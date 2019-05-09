#include "machine.h"

#include <QFile>
#include <QtDebug>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtMath>
#include <QtDataVisualization/QSurfaceDataArray>

using namespace QtDataVisualization;

bool Machine::loadParameters()
{
    QFile loadFile(jsonFileName);

    if (!loadFile.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Couldn't open frequency file" << jsonFileName;
        return false;
    }

    QByteArray rawData = loadFile.readAll();
    QJsonObject freqObj {QJsonDocument::fromJson(rawData).object()};

    double maxFreq = 0;
    for (const QJsonValue elem : freqObj)
        maxFreq = elem.toDouble() > maxFreq ? elem.toDouble() : maxFreq;

    // build heights map for simbols
    // where the height is inversely proportional to the frequency
    QJsonObject::const_iterator itr = freqObj.constBegin();
    heightsMap.clear();
    while (itr != freqObj.constEnd())
    {
        heightsMap.insert(itr.key(), maxFreq - itr.value().toDouble());
        ++itr;
    }

    QString str(freqObj.keys().join(""));   // get all counted symbols (it's a keys in json file) into one string

    removeFilterStr.clear();
    removeFilterStr.append("[^");
    removeFilterStr.append(str);
    removeFilterStr.append("]");

    return true;
}

QSurfaceDataArray* Machine::loadFile(QString fn)
{
    QFile fileTxt(fn);

    if (!fileTxt.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Couldn't open file " << fn << " correctly";
        return nullptr;
    }

    QTextStream in(&fileTxt);
    QString textStr;
    textStr.append(in.read(lenReadedText));

    if (textStr.isEmpty())
    {
        qWarning() << "file " << fn << " was read incorrectly or is empty";
        return nullptr;
    }

    QRegExp rx(removeFilterStr);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    textStr.remove(rx);

    if (textStr.length() < minLenText)
    {
        qWarning() << "Length of processed text isn't enough.";
        return nullptr;
    }

    // build QSurfaceDataArray

    int edgeSize = ceil(sqrt(textStr.length()));

    QSurfaceDataArray *data = new QSurfaceDataArray;
    QSurfaceDataRow *dataRow = new QSurfaceDataRow;
    int i = 0;
    int z = 0;
    int x = 0;
    while (i < textStr.length())
    {
        while ((x < edgeSize) &&
               (i < textStr.length()))
        {
            *dataRow << QVector3D(x, heightsMap[textStr.at(i)], z);
            ++x;
            ++i;
        }

        if (i >= textStr.length())
        {
            // we have reached the end of the text but must fulfill
            // the condition: "All rows in newArray must be of same length"
            while (x < edgeSize)
            {
                *dataRow << QVector3D(x, 0, z);
                ++x;
            }
        }

        x = 0;
        ++z;
        *data << dataRow;
        if (i < textStr.length())
            dataRow = new QSurfaceDataRow;
    }

    return data;
}

