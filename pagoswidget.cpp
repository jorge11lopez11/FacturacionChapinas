#include "pagoswidget.h"
#include "pagodialog.h"
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

PagosWidget::PagosWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel *titulo = new QLabel("Pagos");
    QFont f = titulo->font();
    f.setPointSize(14);
    f.setBold(true);
    titulo->setFont(f);

    tabla = new QTableWidget(this);
    tabla->setColumnCount(7);
    tabla->setHorizontalHeaderLabels(
        {"No. Factura", "Fecha", "Cliente", "Condición", "Total", "Saldo", "Estado"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabla->setSelectionMode(QAbstractItemView::SingleSelection);

    QPushButton *btnPagar = new QPushButton("Registrar pago");
    QPushButton *btnVer   = new QPushButton("Ver pagos");

    QHBoxLayout *filaBotones = new QHBoxLayout;
    filaBotones->addWidget(btnPagar);
    filaBotones->addWidget(btnVer);
    filaBotones->addStretch();

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addWidget(titulo);
    principal->addLayout(filaBotones);
    principal->addWidget(tabla);

    connect(btnPagar, &QPushButton::clicked, this, &PagosWidget::registrarPago);
    connect(btnVer,   &QPushButton::clicked, this, &PagosWidget::verPagos);

    refrescarTabla();
}

void PagosWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refrescarTabla();
}

QString PagosWidget::nombreCliente(int codigo) const
{
    for (const Cliente &c : Datos::instancia().clientes)
        if (c.codigo == codigo) return QString("%1 - %2").arg(c.codigo).arg(c.nombre);
    return "(?)";
}

void PagosWidget::refrescarTabla()
{
    const std::vector<Factura> &lista = Datos::instancia().facturas;

    tabla->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Factura &f = lista[i];

        QString estado;
        if (f.cancelada)            estado = "Cancelada";
        else if (f.saldo <= 0.0001) estado = "Pagada";
        else                        estado = "Pendiente";

        QStringList datos = {
            f.numero(),
            f.fecha,
            nombreCliente(f.codigoCliente),
            f.condicion,
            "Q " + QString::number(f.total, 'f', 2),
            "Q " + QString::number(f.saldo, 'f', 2),
            estado
        };

        for (int col = 0; col < datos.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(datos[col]);
            if (f.cancelada) item->setForeground(QBrush(QColor(NARANJA)));
            tabla->setItem(i, col, item);
        }
    }
}

void PagosWidget::registrarPago()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Registrar pago", "Seleccione una factura de la tabla.");
        return;
    }
    Factura &f = Datos::instancia().facturas[fila];

    if (f.cancelada) {
        QMessageBox::information(this, "Registrar pago",
            "No se pueden registrar pagos en una factura cancelada.");
        return;
    }
    if (f.saldo <= 0.0001) {
        QMessageBox::information(this, "Registrar pago",
            "Esta factura ya está pagada (saldo Q 0.00).");
        return;
    }

    PagoDialog dlg(f, nombreCliente(f.codigoCliente), this);
    if (dlg.exec() != QDialog::Accepted) return;

    Pago pago;
    pago.correlativo = Datos::instancia().siguienteCorrelativoPago++;
    pago.fecha = dlg.fecha();
    pago.monto = dlg.monto();

    f.saldo -= pago.monto;
    if (f.saldo < 0.0001) f.saldo = 0.0;   // evita -0.00 por decimales
    pago.saldoDespues = f.saldo;
    f.pagos.push_back(pago);

    refrescarTabla();

    QString msg = QString("Pago registrado: Q %1\nNuevo saldo de %2: Q %3")
        .arg(QString::number(pago.monto, 'f', 2))
        .arg(f.numero())
        .arg(QString::number(f.saldo, 'f', 2));
    QMessageBox::information(this, "Pago registrado", msg);
}

void PagosWidget::verPagos()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Ver pagos", "Seleccione una factura de la tabla.");
        return;
    }
    const Factura &f = Datos::instancia().facturas[fila];

    QDialog dlg(this);
    dlg.setWindowTitle(QString("Pagos de la factura %1").arg(f.numero()));
    dlg.setMinimumWidth(480);
    QVBoxLayout *lay = new QVBoxLayout(&dlg);

    QString enc = QString("No. Factura: %1\nCliente: %2\nTotal: Q %3\nSaldo actual: Q %4")
        .arg(f.numero())
        .arg(nombreCliente(f.codigoCliente))
        .arg(QString::number(f.total, 'f', 2))
        .arg(QString::number(f.saldo, 'f', 2));
    lay->addWidget(new QLabel(enc, &dlg));

    QTableWidget *t = new QTableWidget(&dlg);
    t->setColumnCount(4);
    t->setHorizontalHeaderLabels({"No. Pago", "Fecha", "Monto", "Saldo después"});
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setRowCount(static_cast<int>(f.pagos.size()));
    for (int i = 0; i < static_cast<int>(f.pagos.size()); ++i) {
        const Pago &p = f.pagos[i];
        t->setItem(i, 0, new QTableWidgetItem(QString::number(p.correlativo)));
        t->setItem(i, 1, new QTableWidgetItem(p.fecha));
        t->setItem(i, 2, new QTableWidgetItem("Q " + QString::number(p.monto, 'f', 2)));
        t->setItem(i, 3, new QTableWidgetItem("Q " + QString::number(p.saldoDespues, 'f', 2)));
    }
    lay->addWidget(t);

    if (f.pagos.empty())
        lay->addWidget(new QLabel("Esta factura aún no tiene pagos registrados.", &dlg));

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    bb->button(QDialogButtonBox::Close)->setText("Cerrar");
    connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::accept);
    lay->addWidget(bb);

    dlg.exec();
}