#ifndef DATOS_H
#define DATOS_H

#include <vector>
#include "modelos.h"

class Datos {
public:
    static Datos& instancia() {
        static Datos unico;
        return unico;
    }

    std::vector<Vendedor> vendedores;
    int siguienteCodigoVendedor = 1;

    std::vector<Cliente> clientes;
    int siguienteCodigoCliente = 1;

    std::vector<Producto> productos;
    int siguienteCodigoProducto = 1;

    std::vector<Factura> facturas;
    int siguienteCorrelativoFactura = 1;
    int siguienteCorrelativoPago = 1;

private:
    Datos() {}
};

#endif // DATOS_H