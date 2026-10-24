#ifndef CLIENTEDIALOG_H
#define CLIENTEDIALOG_H

#include <QDialog>
#include "modelos.h"

class QLineEdit;
class QLabel;
class QComboBox;

class ClienteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClienteDialog(QWidget *parent = nullptr);

    void setCliente(const Cliente &c);
    Cliente cliente() const;

private:
    QWidget* crearFila(QLineEdit *campo, QLabel *&icono, QLabel *&mensaje);
    void pintarEstado(QLineEdit *campo, QLabel *icono, QLabel *mensaje,
                      int estado, const QString &textoError);

    bool validarNombre(bool forzar);
    bool validarNit(bool forzar);
    bool validarTelefono(bool forzar);
    bool validarEmail(bool forzar);   // opcional: válido si está vacío

    QLineEdit *editNombre;
    QLineEdit *editNit;
    QLineEdit *editTelefono;
    QLineEdit *editDireccion;
    QLineEdit *editContacto;
    QLineEdit *editEmail;
    QLineEdit *editWeb;
    QComboBox *comboVendedor;

    QLabel *lblNombreIcono, *lblNombreMsg;
    QLabel *lblNitIcono, *lblNitMsg;
    QLabel *lblTelIcono, *lblTelMsg;
    QLabel *lblEmailIcono, *lblEmailMsg;

    int codigoActual = 0;
};

#endif // CLIENTEDIALOG_H