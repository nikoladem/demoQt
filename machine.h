#ifndef MACHINA_H
#define MACHINA_H

#include <QVector>
#include <QHash>
#include <QChar>
#include <QJsonDocument>
#include <QtDataVisualization/QSurfaceDataArray>

using namespace QtDataVisualization;

class Machine
{

public:
//    explicit Machina();
//    ~Machina();
    bool loadParameters();
    QSurfaceDataArray* loadFile(QString fn);

private:
    const QString jsonFileName {"simbol_frequency.json"};
    const qint64 lenReadedText = 10000;
    const qint64 minLenText = 100;


    QString removeFilterStr;            // regExp for remove all non-counted symbols
    QHash<QString, double> heightsMap;
};

#endif // MACHINA_H
