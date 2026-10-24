#include "facturadialog.h"
#include "datos.h"

#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QLabel>
#include <QTableWidget>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDate>
#include <QFont>

FacturaDialog::FacturaDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Nueva factura");
    setMinimumWidth(640);

    // --- Número de factura (solo lectura): A-001, A-002, ... ---
    int prox = Datos::instancia().siguienteCorrelativoFactura;
    lblNumero = new QLabel(QString("A-%1").arg(prox, 3, 10, QChar('0')), this);
    QFont fnum = lblNumero->font();
    fnum.setBold(true);
    lblNumero->setFont(fnum);

    // --- Encabezado ---
    editFecha = new QDateEdit(QDate::currentDate(), this);
    editFecha->setCalendarPopup(true);
    editFecha->setDisplayFormat("dd/MM/yyyy");

    comboCliente = new QComboBox(this);
    for (const Cliente &c : Datos::instancia().clientes)
        comboCliente->addItem(QString("%1 - %2").arg(c.codigo).arg(c.nombre), c.codigo);

    lblVendedor = new QLabel(this);

    comboCondicion = new QComboBox(this);
    comboCondicion->addItems({"Contado", "Crédito"});

    spinPlazo = new QSpinBox(this);
    spinPlazo->setRange(1, 365);
    spinPlazo->setValue(30);
    spinPlazo->setSuffix(" días");
    spinPlazo->setEnabled(false);

    lblVencimiento = new QLabel("-", this);

    QFormLayout *formEnc = new QFormLayout;
    formEnc->addRow("No. Factura:", lblNumero);
    formEnc->addRow("Fecha:", editFecha);
    formEnc->addRow("Cliente:", comboCliente);
    formEnc->addRow("Vendedor:", lblVendedor);
    formEnc->addRow("Condición:", comboCondicion);
    formEnc->addRow("Plazo:", spinPlazo);
    formEnc->addRow("Vencimiento:", lblVencimiento);

    // --- Agregar productos ---
    comboProducto = new QComboBox(this);
    for (const Producto &p : Datos::instancia().productos)
        comboProducto->addItem(
            QString("%1 - %2 (Q %3, exist. %4)")
                .arg(p.codigo).arg(p.nombre)
                .arg(QString::number(p.precio, 'f', 2)).arg(p.existencia),
            p.codigo);

    spinCantidad = new QSpinBox(this);
    spinCantidad->setRange(1, 1000000);
    spinCantidad->setValue(1);

    QPushButton *btnAgregarLinea = new QPushButton("Agregar producto", this);
    QPushButton *btnQuitarLinea  = new QPushButton("Quitar seleccionado", this);

    QHBoxLayout *filaProd = new QHBoxLayout;
    filaProd->addWidget(new QLabel("Producto:", this));
    filaProd->addWidget(comboProducto, 1);
    filaProd->addWidget(new QLabel("Cantidad:", this));
    filaProd->addWidget(spinCantidad);
    filaProd->addWidget(btnAgregarLinea);

    tablaLineas = new QTableWidget(this);
    tablaLineas->setColumnCount(4);
    tablaLineas->setHorizontalHeaderLabels(
        {"Producto", "Cantidad", "Precio unitario", "Subtotal"});
    tablaLineas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaLineas->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaLineas->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaLineas->setSelectionMode(QAbstractItemView::SingleSelection);

    // --- Totales ---
    lblSubtotal = new QLabel("Q 0.00", this);
    lblIva = new QLabel("Q 0.00", this);
    lblTotal = new QLabel("Q 0.00", this);
    QFont ft = lblTotal->font();
    ft.setBold(true);
    ft.setPointSize(ft.pointSize() + 1);
    lblTotal->setFont(ft);

    QFormLayout *formTot = new QFormLayout;
    formTot->addRow("Subtotal:", lblSubtotal);
    formTot->addRow("IVA (12%):", lblIva);
    formTot->addRow("Total:", lblTotal);

    // --- Botones ---
    QDialogButtonBox *botones = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    botones->button(QDialogButtonBox::Ok)->setText("Registrar");
    botones->button(QDialogButtonBox::Cancel)->setText("Cancelar");

    // --- Armado general ---
    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addLayout(formEnc);
    principal->addLayout(filaProd);
    principal->addWidget(btnQuitarLinea, 0, Qt::AlignLeft);
    principal->addWidget(tablaLineas);
    principal->addLayout(formTot);
    principal->addWidget(botones);

    // --- Conexiones ---
    connect(btnAgregarLinea, &QPushButton::clicked, this, &FacturaDialog::agregarLinea);
    connect(btnQuitarLinea,  &QPushButton::clicked, this, &FacturaDialog::quitarLinea);
    connect(comboCondicion,  &QComboBox::currentIndexChanged, this, &FacturaDialog::condicionCambiada);
    connect(comboCliente,    &QComboBox::currentIndexChanged, this, &FacturaDialog::clienteCambiado);
    connect(editFecha,       &QDateEdit::dateChanged, this, &FacturaDialog::recalcularVencimiento);
    connect(spinPlazo,       &QSpinBox::valueChanged, this, &FacturaDialog::recalcularVencimiento);

    connect(botones, &QDialogButtonBox::accepted, this, [this]() {
        if (lineas.empty()) {
            QMessageBox::warning(this, "Factura",
                                 "Agregue al menos un producto a la factura.");
            return;
        }
        accept();
    });
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);

    clienteCambiado();
    condicionCambiada();
    recalcular();
}

void FacturaDialog::clienteCambiado()
{
    int codCliente = comboCliente->currentData().toInt();

    codigoVendedorActual = 0;
    for (const Cliente &c : Datos::instancia().clientes)
        if (c.codigo == codCliente) { codigoVendedorActual = c.codigoVendedor; break; }

    QString nom = "(sin asignar)";
    for (const Vendedor &v : Datos::instancia().vendedores)
        if (v.codigo == codigoVendedorActual) {
            nom = QString("%1 - %2").arg(v.codigo).arg(v.nombre);
            break;
        }
    lblVendedor->setText(nom);
}

void FacturaDialog::condicionCambiada()
{
    bool credito = (comboCondicion->currentText() == "Crédito");
    spinPlazo->setEnabled(credito);
    recalcularVencimiento();
}

void FacturaDialog::recalcularVencimiento()
{
    if (comboCondicion->currentText() == "Crédito") {
        QDate v = editFecha->date().addDays(spinPlazo->value());
        lblVencimiento->setText(v.toString("dd/MM/yyyy"));
    } else {
        lblVencimiento->setText("-");
    }
}

void FacturaDialog::agregarLinea()
{
    if (comboProducto->count() == 0) return;
    int codProd = comboProducto->currentData().toInt();

    for (const LineaFactura &l : lineas)
        if (l.codigoProducto == codProd) {
            QMessageBox::information(this, "Producto repetido",
                "Ese producto ya está en la factura. Quítelo si desea cambiar la cantidad.");
            return;
        }

    const Producto *prod = nullptr;
    for (const Producto &p : Datos::instancia().productos)
        if (p.codigo == codProd) { prod = &p; break; }
    if (!prod) return;

    int cant = spinCantidad->value();
    if (cant > prod->existencia) {
        QMessageBox::warning(this, "Sin existencia",
            QString("No hay suficiente existencia de \"%1\". Disponible: %2.")
                .arg(prod->nombre).arg(prod->existencia));
        return;
    }

    LineaFactura l;
    l.codigoProducto = prod->codigo;
    l.nombreProducto = prod->nombre;
    l.cantidad = cant;
    l.precioUnitario = prod->precio;
    lineas.push_back(l);

    refrescarTablaLineas();
    recalcular();
}

void FacturaDialog::quitarLinea()
{
    int fila = tablaLineas->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Quitar", "Seleccione una línea de la tabla.");
        return;
    }
    lineas.erase(lineas.begin() + fila);
    refrescarTablaLineas();
    recalcular();
}

void FacturaDialog::refrescarTablaLineas()
{
    tablaLineas->setRowCount(static_cast<int>(lineas.size()));
    for (int i = 0; i < static_cast<int>(lineas.size()); ++i) {
        const LineaFactura &l = lineas[i];
        tablaLineas->setItem(i, 0, new QTableWidgetItem(l.nombreProducto));
        tablaLineas->setItem(i, 1, new QTableWidgetItem(QString::number(l.cantidad)));
        tablaLineas->setItem(i, 2, new QTableWidgetItem("Q " + QString::number(l.precioUnitario, 'f', 2)));
        tablaLineas->setItem(i, 3, new QTableWidgetItem("Q " + QString::number(l.subtotal(), 'f', 2)));
    }
}

void FacturaDialog::recalcular()
{
    double subtotal = 0.0;
    for (const LineaFactura &l : lineas) subtotal += l.subtotal();
    double iva = subtotal * 0.12;
    double total = subtotal + iva;

    lblSubtotal->setText("Q " + QString::number(subtotal, 'f', 2));
    lblIva->setText("Q " + QString::number(iva, 'f', 2));
    lblTotal->setText("Q " + QString::number(total, 'f', 2));
}

Factura FacturaDialog::factura() const
{
    Factura f;
    f.serie = "A";   // serie fija todas inician con A
    f.fecha = editFecha->date().toString("dd/MM/yyyy");
    f.codigoCliente = comboCliente->currentData().toInt();
    f.codigoVendedor = codigoVendedorActual;
    f.condicion = comboCondicion->currentText();
    f.lineas = lineas;

    double subtotal = 0.0;
    for (const LineaFactura &l : lineas) subtotal += l.subtotal();
    f.subtotal = subtotal;
    f.iva = subtotal * 0.12;
    f.total = f.subtotal + f.iva;

    if (f.condicion == "Crédito") {
        f.plazoDias = spinPlazo->value();
        f.fechaVencimiento = editFecha->date().addDays(spinPlazo->value()).toString("dd/MM/yyyy");
    } else {
        f.plazoDias = 0;
        f.fechaVencimiento = "-";
    }
    f.saldo = f.total;
    return f;
}