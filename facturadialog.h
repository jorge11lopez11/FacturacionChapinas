#ifndef FACTURADIALOG_H
#define FACTURADIALOG_H

#include <QDialog>
#include <vector>
#include "modelos.h"

class QComboBox;
class QDateEdit;
class QSpinBox;
class QLabel;
class QTableWidget;

class FacturaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FacturaDialog(QWidget *parent = nullptr);
    Factura factura() const;

private slots:
    void agregarLinea();
    void quitarLinea();
    void condicionCambiada();
    void clienteCambiado();
    void recalcularVencimiento();

private:
    void recalcular();
    void refrescarTablaLineas();

    QLabel *lblNumero;         // No. Factura (solo lectura)
    QDateEdit *editFecha;
    QComboBox *comboCliente;
    QLabel *lblVendedor;
    QComboBox *comboCondicion;
    QSpinBox *spinPlazo;
    QLabel *lblVencimiento;

    QComboBox *comboProducto;
    QSpinBox *spinCantidad;
    QTableWidget *tablaLineas;

    QLabel *lblSubtotal;
    QLabel *lblIva;
    QLabel *lblTotal;

    std::vector<LineaFactura> lineas;
    int codigoVendedorActual = 0;
};

#endif // FACTURADIALOG_H