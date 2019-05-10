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
    loadSettings();

    QFile loadFile(JSON_FREQ_FILE_NAME);

    if (!loadFile.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Couldn't open frequency file" << JSON_FREQ_FILE_NAME;
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


void Machine::loadSettings()
{
    QFile loadFile(JSON_SET_FILE_NAME);

    if (!loadFile.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Couldn't open settings file" << JSON_SET_FILE_NAME << ".\r Default values will be used.";
        return;
    }

    QByteArray rawData = loadFile.readAll();
    QJsonObject settingsObj {QJsonDocument::fromJson(rawData).object()};

    if (settingsObj.contains("minBufLength") && settingsObj["minBufLength"].isDouble())
        minBufLength = settingsObj["minBufLength"].toInt();

    if (settingsObj.contains("bufferLength") && settingsObj["bufferLength"].isDouble())
        bufferLength = settingsObj["bufferLength"].toInt();
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
    textStr.append(in.read(bufferLength));

    if (textStr.isEmpty())
    {
        qWarning() << "file " << fn << " was read incorrectly or is empty";
        return nullptr;
    }

    QRegExp rx(removeFilterStr);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    textStr.remove(rx);

    if (textStr.length() < minBufLength)
    {
        qWarning() << "Length of processed text isn't enough.";
        return nullptr;
    }

    // build QSurfaceDataArray from textStr

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

