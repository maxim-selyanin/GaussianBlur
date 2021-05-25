#include "gaussfunction.h"
#include <QtMath>
#include <qdebug.h>

namespace {
/*
 * по правилу трёх сигм, случайная величина с большой вероятностью лежит в диапазоне от -3sigma до 3sigma,
 * поэтому ограничимся этим диапазоном. тогда, если радиус преобразования равен radius,
 * то 2radius + 1 = 6sigma => sigma = (2radius + 1)/6
 */
qreal getSigma(int radius);
//экспонента из функции Гаусса от х и сигмы
qreal expf(int x, qreal sigma);
//функция Гаусса при известных х, сигме и коэффициенте
qreal g(int x, qreal sigma, qreal coefficient);
//коэффициент функции Гаусса
qreal getCoeff(qreal sigma);
/*
 * в результате применение g по всем x сумма получившихся значений получается меньше 1,
 * что приводит к быстрому затемнению картинки;
 * normalize равномерно раскидывает по всем иксам то, что осталось до 1
 */
void normalize(MatrixType &matrix);



//implementation
qreal getSigma(int radius) {
    return qreal(2*radius + 1)/6.;
}

//
qreal g(int x, qreal sigma, qreal coefficient) {
    return coefficient*expf(x, sigma);
}

//
qreal expf(int x, qreal sigma) {
    return qExp(qreal(-(x*x))
                /
                (2.*sigma*sigma));
}

//
qreal getCoeff(qreal sigma) {
    return 1./(2.*M_PI*sigma*sigma);
}

//
void normalize(MatrixType &matrix) {
    FloatingPointType result = 0;
    for (auto i : matrix) {
        result += i;
    }
    result = 1 - result;
    result /= matrix.size();
    for (auto &i: matrix) {
        i += result;
    }
}
}//namespace anonimous

MatrixType gf::getMatrix(int radius)
{
    MatrixType matrix(radius*2 + 1);
    const qreal sigma = getSigma(radius);
    const qreal coeff = getCoeff(sigma);

    for (int x = -radius; x <= 0; ++x) {
        matrix[x + radius] = g(x, sigma, coeff);
    }

    //лучше лишний раз не трогать qExp и скопировать оставшиеся элементы
    for (int i = 1; i <= radius; ++i) {
        matrix[radius + i] = matrix[radius - i];
    }

    normalize(matrix);

    return matrix;
}
