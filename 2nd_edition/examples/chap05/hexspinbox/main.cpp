#include <QApplication>

#include "hexspinbox.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    HexSpinBox spinBox;
    spinBox.setWindowTitle(QObject::tr("Hex Spin Box"));
    spinBox.show();
    return app.exec();
}
