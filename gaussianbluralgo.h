#ifndef GAUSSIANBLURALGO_H
#define GAUSSIANBLURALGO_H

#include "globaldefinitions.h"
#include <QImage>

namespace gb {
void blurImage(QImage &image, const MatrixType &transformMatrix);
}

#endif // GAUSSIANBLURALGO_H
