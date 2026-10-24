Sistema de Facturación Chapinas:

Proyecto final del curso de **Algoritmos** - Universidad Mariano Gálvez de Guatemala (UMG).
Aplicación de escritorio con interfaz gráfica, desarrollada en **C++ con Qt 6**, para la empresa
ficticia Representaciones "Chapinas", C.A. (venta de artículos de limpieza al mayoreo).

Descripción:

Sistema de facturación que sustituye el proceso manual por una herramienta digital. Permite
gestionar los catálogos, emitir facturas al contado y al crédito, registrar pagos y generar
reportes. La información se maneja **en memoria** durante la ejecución (alcance académico).

Características:

- Gestión de **vendedores**, **clientes** (asignados a un vendedor) y **productos/inventario**.
- **Facturación** al contado y al crédito, con cálculo automático de subtotal, **IVA (12 %)** y total.
- Número de factura correlativo (formato `A-001`), **cancelación** de facturas con devolución al inventario.
- Descuento automático de existencias al vender.
- **Pagos**: abonos a facturas al crédito con control de saldo; pago automático en ventas al contado.
- **Reportes** (clientes, vendedores, productos vendidos, ventas del día, pagos del día e inventario),
  con opción de **exportar a PDF**.
- **Validaciones** con indicadores visuales (DPI, teléfono, e-mail, NIT, precio mayor al costo,
  nombre de producto único, nombres sin números).

Tecnologías:

- Lenguaje: **C++**
- Interfaz gráfica: **Qt 6**
- Construcción: **CMake**
- Editor: **Visual Studio Code**

Requisitos:

- Compilador de C++ (g++ / MinGW) y **Qt 6** instalados.
- **CMake 3.19** o superior.
- En Windows se recomienda **MSYS2 (entorno UCRT64)**.

Cómo compilar y ejecutar:

Desde la carpeta del proyecto (terminal con el entorno de Qt, por ejemplo MSYS2 UCRT64):

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/FacturacionChapinas.exe
```

Autor:

Jorge López — Ingeniería en Sistemas, UMG.
