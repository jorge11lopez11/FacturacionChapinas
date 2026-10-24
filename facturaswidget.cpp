#include "facturaswidget.h"
#include "facturadialog.h"
#include "datos.h"

#include <QTableWidget>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QBrush>
#include <QColor>
#include <QShowEvent>

static const char *NARANJA = "#E67E22";

FacturasWidget::FacturasWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel *titulo = new QLabel("Facturación");
    QFont f = titulo->font();
    f.setPointSize(14);
    f.setBold(true);
    titulo->setFont(f);

    tabla = new QTableWidget(this);
    tabla->setColumnCount(7);
    tabla->setHorizontalHeaderLabels(
        {"No. Factura", "Fecha", "Cliente", "Vendedor",
         "Condición", "Total", "Saldo"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabla->setSelectionMode(QAbstractItemView::SingleSelection);

    QPushButton *btnNueva   = new QPushButton("Nueva factura");
    QPushButton *btnDetalle = new QPushButton("Ver detalle");
    QPushButton *btnCancelar = new QPushButton("Cancelar factura");

    QHBoxLayout *filaBotones = new QHBoxLayout;
    filaBotones->addWidget(btnNueva);
    filaBotones->addWidget(btnDetalle);
    filaBotones->addWidget(btnCancelar);
    filaBotones->addStretch();

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addWidget(titulo);
    principal->addLayout(filaBotones);
    principal->addWidget(tabla);

    connect(btnNueva,    &QPushButton::clicked, this, &FacturasWidget::nuevaFactura);
    connect(btnDetalle,  &QPushButton::clicked, this, &FacturasWidget::verDetalle);
    connect(btnCancelar, &QPushButton::clicked, this, &FacturasWidget::cancelarFactura);

    refrescarTabla();
}

void FacturasWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refrescarTabla();
}

QString FacturasWidget::nombreCliente(int codigo) const
{
    for (const Cliente &c : Datos::instancia().clientes)
        if (c.codigo == codigo) return QString("%1 - %2").arg(c.codigo).arg(c.nombre);
    return "(?)";
}

QString FacturasWidget::nombreVendedor(int codigo) const
{
    for (const Vendedor &v : Datos::instancia().vendedores)
        if (v.codigo == codigo) return QString("%1 - %2").arg(v.codigo).arg(v.nombre);
    return "(sin asignar)";
}

void FacturasWidget::refrescarTabla()
{
    const std::vector<Factura> &lista = Datos::instancia().facturas;

    tabla->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Factura &f = lista[i];

        QString numeroTxt = f.numero();
        if (f.cancelada) numeroTxt += " (Cancelada)";

        QStringList datos = {
            numeroTxt,
            f.fecha,
            nombreCliente(f.codigoCliente),
            nombreVendedor(f.codigoVendedor),
            f.condicion,
            "Q " + QString::number(f.total, 'f', 2),
            "Q " + QString::number(f.saldo, 'f', 2)
        };

        for (int col = 0; col < datos.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(datos[col]);
            if (f.cancelada)
                item->setForeground(QBrush(QColor(NARANJA)));   // fila en naranja
            tabla->setItem(i, col, item);
        }
    }
}

void FacturasWidget::nuevaFactura()
{
    if (Datos::instancia().clientes.empty()) {
        QMessageBox::information(this, "Facturación",
            "Primero debe registrar al menos un cliente.");
        return;
    }
    if (Datos::instancia().productos.empty()) {
        QMessageBox::information(this, "Facturación",
            "Primero debe registrar al menos un producto.");
        return;
    }

    FacturaDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    Factura f = dlg.factura();
    f.correlativo = Datos::instancia().siguienteCorrelativoFactura++;

    for (const LineaFactura &l : f.lineas)
        for (Producto &p : Datos::instancia().productos)
            if (p.codigo == l.codigoProducto) { p.existencia -= l.cantidad; break; }

    if (f.condicion == "Contado") {
        Pago pago;
        pago.correlativo = Datos::instancia().siguienteCorrelativoPago++;
        pago.fecha = f.fecha;
        pago.monto = f.total;
        pago.saldoDespues = 0.0;
        f.pagos.push_back(pago);
        f.saldo = 0.0;
    } else {
        f.saldo = f.total;
    }

    Datos::instancia().facturas.push_back(f);
    refrescarTabla();
}

void FacturasWidget::cancelarFactura()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Cancelar factura",
                                "Seleccione una factura de la tabla.");
        return;
    }

    Factura &f = Datos::instancia().facturas[fila];

    if (f.cancelada) {
        QMessageBox::information(this, "Cancelar factura",
                                "Esta factura ya está cancelada.");
        return;
    }

    QString msg = QString("¿Cancelar la factura %1?\n"
                          "Los productos regresarán al inventario.").arg(f.numero());
    if (QMessageBox::question(this, "Cancelar factura", msg) != QMessageBox::Yes)
        return;

    // Devolver los productos al inventario
    for (const LineaFactura &l : f.lineas)
        for (Producto &p : Datos::instancia().productos)
            if (p.codigo == l.codigoProducto) { p.existencia += l.cantidad; break; }

    f.cancelada = true;
    f.saldo = 0.0;

    refrescarTabla();
    QMessageBox::information(this, "Cancelar factura",
        "La factura fue cancelada y los productos regresaron al inventario.");
}

void FacturasWidget::verDetalle()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Ver detalle", "Seleccione una factura de la tabla.");
        return;
    }
    const Factura &f = Datos::instancia().facturas[fila];

    QDialog dlg(this);
    dlg.setWindowTitle(QString("Factura %1%2")
        .arg(f.numero()).arg(f.cancelada ? " (Cancelada)" : ""));
    dlg.setMinimumWidth(560);
    if (f.cancelada)
        dlg.setStyleSheet(QString("QLabel { color: %1; }").arg(NARANJA));  // ventana en naranja

    QVBoxLayout *lay = new QVBoxLayout(&dlg);

    if (f.cancelada) {
        QLabel *aviso = new QLabel("FACTURA CANCELADA", &dlg);
        QFont fa = aviso->font();
        fa.setBold(true);
        fa.setPointSize(fa.pointSize() + 2);
        aviso->setFont(fa);
        aviso->setAlignment(Qt::AlignCenter);
        lay->addWidget(aviso);
    }

    QString enc = QString("No. Factura: %1\nFecha: %2\nCliente: %3\nVendedor: %4\nCondición: %5")
        .arg(f.numero()).arg(f.fecha)
        .arg(nombreCliente(f.codigoCliente)).arg(nombreVendedor(f.codigoVendedor))
        .arg(f.condicion);
    if (f.condicion == "Crédito")
        enc += QString("\nPlazo: %1 días   |   Vencimiento: %2")
                   .arg(f.plazoDias).arg(f.fechaVencimiento);
    lay->addWidget(new QLabel(enc, &dlg));

    QTableWidget *t = new QTableWidget(&dlg);
    t->setColumnCount(4);
    t->setHorizontalHeaderLabels({"Producto", "Cantidad", "Precio unitario", "Subtotal"});
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setRowCount(static_cast<int>(f.lineas.size()));
    for (int i = 0; i < static_cast<int>(f.lineas.size()); ++i) {
        const LineaFactura &l = f.lineas[i];
        t->setItem(i, 0, new QTableWidgetItem(l.nombreProducto));
        t->setItem(i, 1, new QTableWidgetItem(QString::number(l.cantidad)));
        t->setItem(i, 2, new QTableWidgetItem("Q " + QString::number(l.precioUnitario, 'f', 2)));
        t->setItem(i, 3, new QTableWidgetItem("Q " + QString::number(l.subtotal(), 'f', 2)));
    }
    lay->addWidget(t);

    QString tot;
    tot += "Subtotal: Q " + QString::number(f.subtotal, 'f', 2) + "\n";
    tot += "IVA (12%): Q " + QString::number(f.iva, 'f', 2) + "\n";
    tot += "Total: Q " + QString::number(f.total, 'f', 2) + "\n";
    tot += "Saldo: Q " + QString::number(f.saldo, 'f', 2);
    lay->addWidget(new QLabel(tot, &dlg));

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    bb->button(QDialogButtonBox::Close)->setText("Cerrar");
    connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::accept);
    lay->addWidget(bb);

    dlg.exec();
}