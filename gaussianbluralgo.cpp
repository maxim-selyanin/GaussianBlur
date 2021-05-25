#include "gaussianbluralgo.h"
#include <limits>
#include <functional>
#include <QDebug>
#include <cstring>

static constexpr ColorType ColorTypeMax = std::numeric_limits<ColorType>::max();
static constexpr int bitsInByte = 8;

namespace {
//установить в rgb ref значение colorVal для цвета colorType
void setColor(QRgb &ref, ColorType colorVal, Colors colorType);
//извлечь цвет color из rgb pixel
ColorType getColor(QRgb pixel, Colors color);

//увеличить result на addedValue, если их сумма не превысит максимальное значение для result
void checkAndIncreaseResult(ColorType &result, FloatingPointType addedValue);
//заблюрить цвет colorType пикселя resultRef с окрестностью vicinity и матрицей преобразования transformMatrix
void blurPlotSignleColor(QRgb &resultRef, const QRgb *vicinity, const MatrixType &transformMatrix, Colors colorType);
//заблюрить все цвета пикселя resultRef исходя из окрестности vicinity и матрицы transformMatrix
void blurPlotAllColors(QRgb &resultRef, const QRgb *vicinity, const MatrixType &transformMatrix);

//"размазать" первые smudgedAmount значения вектора source по его остальным значениям и записать результат в target
template <typename ConstIterator, typename Iterator>
void smudgeFirstValues(ConstIterator sourceBegin, ConstIterator sourceEnd
                       , Iterator targetBegin
                       , int smudgedAmount);
//получаем "обрезанную" матрицу для преобразования угловых точек, у которых нет левой/правой/верхней/нижней части окрестности
MatrixType getSlicedMatrix(const MatrixType &transformMatrix, int slicedAmount);
//блюрим угловой пиксель resultRef с неполной окрестностью vicinity и "обрезанной" матрицей transformMatrix
void blurAnglePlot(QRgb &resultRef, const QRgb *vicinity, const MatrixType &transformMatrix, int slicedAmount);


//блюрим некую последовательность пикселей с доступом из функции getter, длиной lineSize и матрицей transformMatrix
//геттер сделан, потому что к пикселям столбцов нет прямого доступа, для унифицированной работы со строками и столбцами
void blurBlock(std::function<QRgb&(int i)> getter, int lineSize
               , const MatrixType &transformMatrix);
//блюрим линию
void blurLine(QImage &image, int number, const MatrixType &transformMatrix);
//блюрим стоблец
void blurColumn(QImage &image, int number, const MatrixType &transformMatrix);







//implementations

//
void setColor(QRgb &ref, ColorType colorVal, Colors colorType) {
    //на сколько сдвигать ref
    int factor = static_cast<int>(colorType)*bitsInByte;

    //зануляем интересущий нас фрагмент рефа
    QRgb nullVal = ~(0x000000ffu << factor);
    ref &= nullVal;

    //пишем туда значение colorVal
    QRgb setVal = (QRgb(colorVal) << factor);
    ref |= setVal;
}

//
ColorType getColor(QRgb pixel, Colors color) {
    //смотрим, что лежит в pixel под интересущим нас отступом
    int factor = static_cast<int>(color)*bitsInByte;
    return ((pixel >> factor) & 0xff);
}

//
void checkAndIncreaseResult(ColorType &result, FloatingPointType addedValue) {
    ColorType tmp = static_cast<ColorType>(addedValue);
    //result + addedValue > RgbMax
    while (result > ColorTypeMax - tmp) {//проверяем на переполнение и прибавляем
        --result;
    }
    result += tmp;
}

//
MatrixType getSlicedMatrix(const MatrixType &transformMatrix, int slicedAmount) {
    //смотрим, элементов не хватает в начале или в конце
    bool slicedFromStart = (slicedAmount < 0);
    slicedAmount = qAbs(slicedAmount);

    MatrixType resultingMatrix(transformMatrix.length() - slicedAmount);

    //"обрезаем" элементы с начала
    if (slicedFromStart) {
        smudgeFirstValues(transformMatrix.cbegin(), transformMatrix.cend()
                          , resultingMatrix.begin(), slicedAmount);
    } else {//обрезаем элементы с конца
        smudgeFirstValues(transformMatrix.crbegin(), transformMatrix.crend()
                          , resultingMatrix.rbegin(), slicedAmount);
    }
    return resultingMatrix;
}

//шаблонная функция, чтобы можно было использовать reverse и обычные итератора
template <typename ConstIterator, typename Iterator>
void smudgeFirstValues(ConstIterator sourceBegin, ConstIterator sourceEnd
                       , Iterator targetBegin
                       , int slicedAmount) {
    FloatingPointType smudgedValue = 0;
    int remainingAmount = int(sourceEnd - sourceBegin) - slicedAmount;

    for (auto i = sourceBegin; i < sourceBegin + slicedAmount; ++i) {
        smudgedValue += *i;
    }
    //значения "отрезанных" элементов равномерно разлетаются по оставшимся элементам
    smudgedValue /= remainingAmount;

    for (int i = 0; i < remainingAmount; ++i) {
        *(targetBegin + i) = smudgedValue + *(sourceBegin + slicedAmount + i);
    }
}

//блюрим пиксель одним цветом
void blurPlotSignleColor(QRgb &resultRef, const QRgb *vicinity, const MatrixType &transformMatrix, Colors colorType) {
    ColorType result = 0;
    for (int i = 0; i < transformMatrix.length(); ++i) {
        FloatingPointType part =
                static_cast<FloatingPointType>(getColor(vicinity[i], colorType))
                * transformMatrix[i];
        checkAndIncreaseResult(result, part);
    }
    setColor(resultRef, result, colorType);
}

//блюрим пиксель тремя цветами
void blurPlotAllColors(QRgb &resultRef, const QRgb *vicinity, const MatrixType &transformMatrix) {
    blurPlotSignleColor(resultRef, vicinity, transformMatrix, Colors::red);
    blurPlotSignleColor(resultRef, vicinity, transformMatrix, Colors::green);
    blurPlotSignleColor(resultRef, vicinity, transformMatrix, Colors::blue);
}

//блюрим угловой пиксель
void blurAnglePlot(QRgb &resultRef, const QRgb *vicinity
                   , const MatrixType &transformMatrix, int slicedAmount) {
    blurPlotAllColors(resultRef, vicinity,
             getSlicedMatrix(transformMatrix, slicedAmount));
}

//
void blurBlock(std::function<QRgb&(int i)> getter, int lineSize, const MatrixType &transformMatrix) {
    //изначальные данные
    LineType blockState(lineSize);
    for (int i = 0; i < lineSize; ++i) {
        blockState[i] = getter(i);
    }

    //blur angle points
    int anglePointsAmount = transformMatrix.length() / 2;

    //blur begin angle points
    for (int i = 0; i < anglePointsAmount; ++i) {
        blurAnglePlot(getter(i), blockState.constData(), transformMatrix, i - anglePointsAmount);
    }

    //blur end angle points
    for (int i = 0; i < anglePointsAmount; ++i) {
        int vicinityOffset = lineSize - transformMatrix.length() + i + 1;
        int anglePointOffset = lineSize - anglePointsAmount + i;
        blurAnglePlot(getter(anglePointOffset), blockState.constData() + vicinityOffset, transformMatrix, i + 1);
    }

    //blur normal points
    for (int i = anglePointsAmount; i < lineSize - anglePointsAmount; ++i)
    {
        blurPlotAllColors(getter(i), blockState.constData() + i - anglePointsAmount, transformMatrix);
    }
}

//
void blurLine(QImage &image, int number, const MatrixType &transformMatrix) {
    auto getter = [&image, number](int position) -> QRgb& {
        //примитивный геттер для строки
        QRgb *rawData = reinterpret_cast<QRgb*>(image.scanLine(number));
        return rawData[position];
    };

    //здесь можно было написать сохранение строки в векторе через memcpy, но при этом всё падает

    blurBlock(getter, image.width(), transformMatrix);
}

//
void blurColumn(QImage &image, int number, const MatrixType &transformMatrix) {
    auto getter = [&image, number](int position) -> QRgb& {
        //менее примитивный геттер для столбца
        QRgb *rawData = reinterpret_cast<QRgb*>(image.scanLine(position));
        return rawData[number];
    };

    blurBlock(getter, image.height(), transformMatrix);
}
}

void gb::blurImage(QImage &image, const MatrixType &transformMatrix)
{
    //blur all lines
    for (int line = 0; line < image.height(); ++line) {
        blurLine(image, line, transformMatrix);
    }
    //blur all columns
    for (int column = 0; column < image.width(); ++column) {
        blurColumn(image, column, transformMatrix);
    }
}
