#ifndef GLOBALDEFINITIONS_H
#define GLOBALDEFINITIONS_H

#include <QVector>
#include <QColor>

constexpr char pic1Path[] = ":/images/defaultpic.jpg";
constexpr char pic2Path[] = ":/images/bigapple.jpg";
constexpr char pic3Path[] = ":/images/smallapple.jpg";
constexpr char pic4Path[] = ":/images/coolangles.jpg";

//представление цвета 8 битами
using ColorType = quint8;
//float быстрее, double точнее
using FloatingPointType = double;
//матрица в одну строку/столбец
using MatrixType = QVector<FloatingPointType>;
//линия исходных данных
using LineType = QVector<QRgb>;

enum class Colors: int {
    blue = 0
    , green
    , red
};

#endif // GLOBALDEFINITIONS_H
