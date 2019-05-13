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

    for (const QJsonValue elem : freqObj)
        maxHeigt = elem.toDouble() > maxHeigt ? elem.toDouble() : maxHeigt;

    // build heights map for simbols
    // where the height is inversely proportional to the frequency
    QJsonObject::const_iterator itr = freqObj.constBegin();
    heightsMap.clear();
    while (itr != freqObj.constEnd())
    {
        heightsMap.insert(itr.key(), maxHeigt - itr.value().toDouble());
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
        qWarning() << "Couldn't open settings file" << JSON_SET_FILE_NAME << ".\n Default values will be used.";
        return;
    }

    QByteArray rawData = loadFile.readAll();
    QJsonObject settingsObj {QJsonDocument::fromJson(rawData).object()};

    if (settingsObj.contains("minBufLength") && settingsObj["minBufLength"].isDouble())
        minBufLength = settingsObj["minBufLength"].toInt();

    if (settingsObj.contains("bufferLength") && settingsObj["bufferLength"].isDouble())
        bufferLength = settingsObj["bufferLength"].toInt();
}


bool Machine::loadFile(QString fn)
{
    QFile fileTxt(fn);

    if (!fileTxt.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Couldn't open file " << fn << " correctly";
        return false;
    }

    QTextStream in(&fileTxt);
    textBuffer.clear();
    textBuffer.append(in.read(bufferLength));

    if (textBuffer.trimmed().isEmpty())
    {
        qWarning() << "File " << fn << " was read incorrectly or is empty";
        return false;
    }

    QRegExp rx(removeFilterStr);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    textBuffer.remove(rx);

    if (textBuffer.length() < minBufLength)
    {
        qWarning() << "Length of processed text isn't enough.";
        return false;
    }

    return true;
}

QVector<QPointF> Machine::getChartSeriesData()
{
    QVector<QPointF> buffer;

    for (int i=0; i < textBuffer.length(); ++i)
        buffer << QPointF(i, heightsMap[textBuffer.at(i)]);

    return buffer;
}

QSurfaceDataArray* Machine::getSurfaceDataArray()
{
    if (textBuffer.trimmed().isEmpty())
        return nullptr;

    int edgeSize = ceil(sqrt(textBuffer.length()));

    QSurfaceDataArray *data = new QSurfaceDataArray;
    QSurfaceDataRow *dataRow = new QSurfaceDataRow;
    int i = 0;
    int z = 0;
    int x = 0;
    while (i < textBuffer.length())
    {
        while ((x < edgeSize) &&
               (i < textBuffer.length()))
        {
            *dataRow << QVector3D(x, heightsMap[textBuffer.at(i)], z);
            ++x;
            ++i;
        }

        if (i >= textBuffer.length())
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
        if (i < textBuffer.length())
            dataRow = new QSurfaceDataRow;
    }

    return data;
}

