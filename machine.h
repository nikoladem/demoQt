#ifndef MACHINA_H
#define MACHINA_H

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
    QSurfaceDataArray* loadFile(QString fn);

private:
    const QString JSON_FREQ_FILE_NAME   {"simbol_frequency.json"};
    const QString JSON_SET_FILE_NAME    {"settings.json"};

    qint64 bufferLength = 10000;
    qint64 minBufLength = 100;

    QString removeFilterStr;            // regExp for remove all non-counted symbols
    QHash<QString, double> heightsMap;
};

#endif // MACHINA_H
