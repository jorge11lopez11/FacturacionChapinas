#ifndef PRODUCTODIALOG_H
#define PRODUCTODIALOG_H

#include <QDialog>
#include "modelos.h"

class QLineEdit;
class QLabel;

class ProductoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductoDialog(QWidget *parent = nullptr);

    void setProducto(const Producto &p);
    Producto producto() const;

private:
    QWidget* crearFila(QLineEdit *campo, QLabel *&icono, QLabel *&mensaje);
    void pintarEstado(QLineEdit *campo, QLabel *icono, QLabel *mensaje,
                      int estado, const QString &textoError);

    bool validarNombre(bool forzar);
    bool validarCosto(bool forzar);
    bool validarPrecio(bool forzar);
    bool validarExistencia(bool forzar);

    QLineEdit *editNombre;
    QLineEdit *editCosto;
    QLineEdit *editPrecio;
    QLineEdit *editExistencia;

    QLabel *lblNombreIcono, *lblNombreMsg;
    QLabel *lblCostoIcono, *lblCostoMsg;
    QLabel *lblPrecioIcono, *lblPrecioMsg;
    QLabel *lblExistIcono, *lblExistMsg;

    int codigoActual = 0;
};

#endif // PRODUCTODIALOG_H