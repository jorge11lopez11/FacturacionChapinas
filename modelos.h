#ifndef MODELOS_H
#define MODELOS_H

#include <QString>
#include <vector>

struct Vendedor {
    int codigo = 0;
    QString dpi;
    QString nombre;
    QString telefono;
    QString direccion;
    QString email;
};

struct Cliente {
    int codigo = 0;
    QString nombre;
    QString nit;
    QString telefono;
    QString direccion;
    QString contacto;
    QString email;
    QString paginaWeb;
    int codigoVendedor = 0;
};

struct Producto {
    int codigo = 0;
    QString nombre;
    double costo = 0.0;
    double precio = 0.0;
    int existencia = 0;
};

struct LineaFactura {
    int codigoProducto = 0;
    QString nombreProducto;
    int cantidad = 0;
    double precioUnitario = 0.0;
    double subtotal() const { return cantidad * precioUnitario; }
};

struct Pago {
    int correlativo = 0;
    QString fecha;
    double monto = 0.0;
    double saldoDespues = 0.0;
};

struct Factura {
    int correlativo = 0;
    QString serie;
    QString fecha;
    int codigoCliente = 0;
    int codigoVendedor = 0;
    QString condicion;
    int plazoDias = 0;
    QString fechaVencimiento;
    std::vector<LineaFactura> lineas;
    double subtotal = 0.0;
    double iva = 0.0;
    double total = 0.0;
    double saldo = 0.0;
    std::vector<Pago> pagos;
    bool cancelada = false;

    // Número con formato A-001, A-002, ...
    QString numero() const {
        return QString("%1-%2").arg(serie).arg(correlativo, 3, 10, QChar('0'));
    }
};

#endif // MODELOS_H