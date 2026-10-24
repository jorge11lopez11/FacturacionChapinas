#ifndef PAGODIALOG_H
#define PAGODIALOG_H

#include <QDialog>
#include "modelos.h"

class QLineEdit;
class QLabel;
class QDateEdit;

class PagoDialog : public QDialog
{
    Q_OBJECT

public:
    PagoDialog(const Factura &f, const QString &clienteNombre, QWidget *parent = nullptr);

    QString fecha() const;
    double monto() const;

private:
    bool validarMonto(bool forzar);

    double saldo;
    QDateEdit *editFecha;
    QLineEdit *editMonto;
    QLabel *lblMontoIcono;
    QLabel *lblMontoMsg;
};

#endif // PAGODIALOG_H