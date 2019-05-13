#ifndef MACHINA_H
#define MACHINA_H

//#include <QPointF>
#include <QHash>
#include <QtDataVisualization/QSurfaceDataArray>

using namespace QtDataVisualization;

class Machine
{

public:
//    explicit Machina();
//    ~Machina();
    bool loadParameters();
    void loadSettings();
    bool loadFile(QString fn);
    QSurfaceDataArray* getSurfaceDataArray();
    QVector<QPointF> getChartSeriesData();

private:
    const QString JSON_FREQ_FILE_NAME   {"simbol_frequency.json"};
    const QString JSON_SET_FILE_NAME    {"settings.json"};

    qint64 bufferLength = 2000;
    qint64 minBufLength = 100;

    QString textStr;

    QString removeFilterStr;            // regExp for remove all non-counted symbols
    QHash<QString, double> heightsMap;
    float maxHeigt = 0;

public:
    float getMaxHeight() { return maxHeigt; }
};

#endif // MACHINA_H
