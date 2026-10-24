#ifndef VENDEDORDIALOG_H
#define VENDEDORDIALOG_H

#include <QDialog>
#include "modelos.h"

class QLineEdit;
class QLabel;

class VendedorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VendedorDialog(QWidget *parent = nullptr);

    void setVendedor(const Vendedor &v);   // cargar datos (al modificar)
    Vendedor vendedor() const;             // leer lo que se escribió

private:
    // Crea la fila: [campo][ícono] + mensaje debajo (guarda las etiquetas)
    QWidget* crearFila(QLineEdit *campo, QLabel *&icono, QLabel *&mensaje);
    // Pinta el estado: 0 = neutral, 1 = válido (verde), 2 = inválido (rojo)
    void pintarEstado(QLineEdit *campo, QLabel *icono, QLabel *mensaje,
                      int estado, const QString &textoError);

    // Validadores por campo (devuelven true si es válido)
    bool validarNombre(bool forzar);
    bool validarDpi(bool forzar);
    bool validarTelefono(bool forzar);
    bool validarEmail(bool forzar);

    QLineEdit *editDpi;
    QLineEdit *editNombre;
    QLineEdit *editTelefono;
    QLineEdit *editDireccion;
    QLineEdit *editEmail;

    QLabel *lblDpiIcono, *lblDpiMsg;
    QLabel *lblNombreIcono, *lblNombreMsg;
    QLabel *lblTelIcono, *lblTelMsg;
    QLabel *lblEmailIcono, *lblEmailMsg;

    int codigoActual = 0;                  // recuerda el código al modificar
};

#endif // VENDEDORDIALOG_H