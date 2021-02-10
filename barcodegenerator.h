#ifndef BARCODEGENERATOR_H
#define BARCODEGENERATOR_H

#include <QLabel>
#include <QObject>
#include <QtPrintSupport>

#define CODE128_B_START 104
#define CODE128_STOP 106

class BarcodeGenerator
{
public:
    BarcodeGenerator();
    static void printBarcode(QString barcodeText = "Ninguno", 
                             QLabel *label = nullptr, int pointSize = 50);
    static QString encodeBarcode(QString code);
    static int calculateCheckCharacter(QString code);
    static int codeToChar(int code);
    static int charToCode(int ch);

    void printBarcodeImage(QString barcodeText);
    void configurePage();
    bool configurePrinter(QString printerName);
    QStringList getAvailablePrinters();

private:
    bool printerConfigured = false;
    QPrinter mPrinter;
};

#endif // BARCODEGENERATOR_H
