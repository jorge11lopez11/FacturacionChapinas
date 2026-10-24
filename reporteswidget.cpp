#include "reporteswidget.h"
#include "datos.h"

#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QTableWidget>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <QDate>
#include <QBrush>
#include <QColor>
#include <QShowEvent>
#include <QMap>
#include <vector>
// --- necesarios para exportar a PDF ---
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QPageSize>
#include <QPageLayout>

static const char *NARANJA = "#E67E22";

ReportesWidget::ReportesWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel *titulo = new QLabel("Reportes");
    QFont f = titulo->font();
    f.setPointSize(14);
    f.setBold(true);
    titulo->setFont(f);

    comboReporte = new QComboBox(this);
    comboReporte->addItems({
        "Clientes",
        "Vendedores",
        "Productos vendidos (por rango de fechas)",
        "Ventas del día (facturas)",
        "Pagos del día",
        "Inventario existente"
    });

    lblDesde = new QLabel("Desde:", this);
    dateDesde = new QDateEdit(QDate::currentDate(), this);
    dateDesde->setCalendarPopup(true);
    dateDesde->setDisplayFormat("dd/MM/yyyy");

    lblHasta = new QLabel("Hasta:", this);
    dateHasta = new QDateEdit(QDate::currentDate(), this);
    dateHasta->setCalendarPopup(true);
    dateHasta->setDisplayFormat("dd/MM/yyyy");

    QPushButton *btnGenerar = new QPushButton("Generar", this);
    QPushButton *btnPdf = new QPushButton("Exportar a PDF", this);   // <-- nuevo

    QHBoxLayout *fila = new QHBoxLayout;
    fila->addWidget(new QLabel("Reporte:", this));
    fila->addWidget(comboReporte);
    fila->addWidget(lblDesde);
    fila->addWidget(dateDesde);
    fila->addWidget(lblHasta);
    fila->addWidget(dateHasta);
    fila->addWidget(btnGenerar);
    fila->addWidget(btnPdf);           // <-- nuevo
    fila->addStretch();

    tabla = new QTableWidget(this);
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabla->setSelectionMode(QAbstractItemView::SingleSelection);

    lblResumen = new QLabel(this);
    QFont fr = lblResumen->font();
    fr.setBold(true);
    lblResumen->setFont(fr);

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addWidget(titulo);
    principal->addLayout(fila);
    principal->addWidget(tabla);
    principal->addWidget(lblResumen);

    connect(comboReporte, &QComboBox::currentIndexChanged, this, &ReportesWidget::reporteCambiado);
    connect(btnGenerar,   &QPushButton::clicked,           this, &ReportesWidget::generar);
    connect(btnPdf,       &QPushButton::clicked,           this, &ReportesWidget::exportarPdf);   // <-- nuevo

    reporteCambiado();
}

void ReportesWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    generar();
}

void ReportesWidget::reporteCambiado()
{
    int idx = comboReporte->currentIndex();
    bool rango = (idx == 2);
    bool unaFecha = (idx == 3 || idx == 4);

    lblDesde->setVisible(rango || unaFecha);
    dateDesde->setVisible(rango || unaFecha);
    lblHasta->setVisible(rango);
    dateHasta->setVisible(rango);
    lblDesde->setText(rango ? "Desde:" : "Fecha:");
}

void ReportesWidget::prepararTabla(const QStringList &headers)
{
    tabla->clear();
    tabla->setColumnCount(headers.size());
    tabla->setHorizontalHeaderLabels(headers);
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setRowCount(0);
}

QString ReportesWidget::nombreCliente(int codigo) const
{
    for (const Cliente &c : Datos::instancia().clientes)
        if (c.codigo == codigo) return QString("%1 - %2").arg(c.codigo).arg(c.nombre);
    return "(?)";
}

QString ReportesWidget::nombreVendedor(int codigo) const
{
    for (const Vendedor &v : Datos::instancia().vendedores)
        if (v.codigo == codigo) return QString("%1 - %2").arg(v.codigo).arg(v.nombre);
    return "(sin asignar)";
}

void ReportesWidget::generar()
{
    switch (comboReporte->currentIndex()) {
        case 0: reporteClientes();          break;
        case 1: reporteVendedores();        break;
        case 2: reporteProductosVendidos(); break;
        case 3: reporteVentasDia();         break;
        case 4: reportePagosDia();          break;
        case 5: reporteInventario();        break;
    }
}

void ReportesWidget::reporteClientes()
{
    prepararTabla({"Código", "Nombre", "NIT", "Teléfono", "E-mail", "Vendedor"});
    const std::vector<Cliente> &lista = Datos::instancia().clientes;
    tabla->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Cliente &c = lista[i];
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(c.codigo)));
        tabla->setItem(i, 1, new QTableWidgetItem(c.nombre));
        tabla->setItem(i, 2, new QTableWidgetItem(c.nit));
        tabla->setItem(i, 3, new QTableWidgetItem(c.telefono));
        tabla->setItem(i, 4, new QTableWidgetItem(c.email));
        tabla->setItem(i, 5, new QTableWidgetItem(nombreVendedor(c.codigoVendedor)));
    }
    lblResumen->setText(QString("Total de clientes: %1").arg(lista.size()));
}

void ReportesWidget::reporteVendedores()
{
    prepararTabla({"Código", "DPI", "Nombre", "Teléfono", "E-mail"});
    const std::vector<Vendedor> &lista = Datos::instancia().vendedores;
    tabla->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Vendedor &v = lista[i];
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(v.codigo)));
        tabla->setItem(i, 1, new QTableWidgetItem(v.dpi));
        tabla->setItem(i, 2, new QTableWidgetItem(v.nombre));
        tabla->setItem(i, 3, new QTableWidgetItem(v.telefono));
        tabla->setItem(i, 4, new QTableWidgetItem(v.email));
    }
    lblResumen->setText(QString("Total de vendedores: %1").arg(lista.size()));
}

void ReportesWidget::reporteProductosVendidos()
{
    QDate desde = dateDesde->date();
    QDate hasta = dateHasta->date();
    if (desde > hasta) {
        QMessageBox::warning(this, "Rango de fechas",
                             "La fecha 'Desde' no puede ser mayor que 'Hasta'.");
        return;
    }

    struct Agg { QString nombre; int cant = 0; double total = 0.0; };
    QMap<int, Agg> acum;

    for (const Factura &f : Datos::instancia().facturas) {
        if (f.cancelada) continue;
        QDate ff = QDate::fromString(f.fecha, "dd/MM/yyyy");
        if (!ff.isValid() || ff < desde || ff > hasta) continue;
        for (const LineaFactura &l : f.lineas) {
            Agg &a = acum[l.codigoProducto];
            a.nombre = l.nombreProducto;
            a.cant += l.cantidad;
            a.total += l.subtotal();
        }
    }

    prepararTabla({"Código", "Producto", "Cantidad vendida", "Total (Q)"});
    tabla->setRowCount(static_cast<int>(acum.size()));
    int row = 0;
    double totalSinIva = 0.0;
    for (auto it = acum.constBegin(); it != acum.constEnd(); ++it, ++row) {
        const Agg &a = it.value();
        tabla->setItem(row, 0, new QTableWidgetItem(QString::number(it.key())));
        tabla->setItem(row, 1, new QTableWidgetItem(a.nombre));
        tabla->setItem(row, 2, new QTableWidgetItem(QString::number(a.cant)));
        tabla->setItem(row, 3, new QTableWidgetItem("Q " + QString::number(a.total, 'f', 2)));
        totalSinIva += a.total;
    }

    double iva = totalSinIva * 0.12;
    double totalConIva = totalSinIva + iva;

    lblResumen->setText(QString("Del %1 al %2   —   Total sin IVA: Q %3   |   IVA (12%): Q %4   |   Total con IVA: Q %5")
        .arg(desde.toString("dd/MM/yyyy"))
        .arg(hasta.toString("dd/MM/yyyy"))
        .arg(QString::number(totalSinIva, 'f', 2))
        .arg(QString::number(iva, 'f', 2))
        .arg(QString::number(totalConIva, 'f', 2)));
}

void ReportesWidget::reporteVentasDia()
{
    QDate fecha = dateDesde->date();
    prepararTabla({"No. Factura", "Cliente", "Vendedor", "Condición",
                   "Total", "Saldo", "Estado"});

    const std::vector<Factura> &fs = Datos::instancia().facturas;
    std::vector<int> idxs;
    for (int i = 0; i < static_cast<int>(fs.size()); ++i) {
        QDate ff = QDate::fromString(fs[i].fecha, "dd/MM/yyyy");
        if (ff == fecha) idxs.push_back(i);
    }

    tabla->setRowCount(static_cast<int>(idxs.size()));
    double totalSinIva = 0.0;
    double totalConIva = 0.0;
    int validas = 0;
    for (int r = 0; r < static_cast<int>(idxs.size()); ++r) {
        const Factura &f = fs[idxs[r]];
        QString estado = f.cancelada ? "Cancelada"
                         : (f.saldo <= 0.0001 ? "Pagada" : "Pendiente");
        QStringList datos = {
            f.numero(),
            nombreCliente(f.codigoCliente),
            nombreVendedor(f.codigoVendedor),
            f.condicion,
            "Q " + QString::number(f.total, 'f', 2),
            "Q " + QString::number(f.saldo, 'f', 2),
            estado
        };
        for (int col = 0; col < datos.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(datos[col]);
            if (f.cancelada) item->setForeground(QBrush(QColor(NARANJA)));
            tabla->setItem(r, col, item);
        }
        if (!f.cancelada) {
            totalSinIva += f.subtotal;
            totalConIva += f.total;
            ++validas;
        }
    }

    double iva = totalConIva - totalSinIva;

    lblResumen->setText(QString("Ventas del %1: %2 factura(s) válida(s)   —   Total sin IVA: Q %3   |   IVA (12%): Q %4   |   Total con IVA: Q %5   (no incluye canceladas)")
        .arg(fecha.toString("dd/MM/yyyy"))
        .arg(validas)
        .arg(QString::number(totalSinIva, 'f', 2))
        .arg(QString::number(iva, 'f', 2))
        .arg(QString::number(totalConIva, 'f', 2)));
}

void ReportesWidget::reportePagosDia()
{
    QDate fecha = dateDesde->date();
    prepararTabla({"No. Pago", "No. Factura", "Cliente", "Fecha", "Monto"});

    struct Fila { int noPago; QString factura; QString cliente; QString fecha; double monto; };
    std::vector<Fila> filas;
    double total = 0.0;

    for (const Factura &f : Datos::instancia().facturas) {
        if (f.cancelada) continue;
        for (const Pago &p : f.pagos) {
            QDate pf = QDate::fromString(p.fecha, "dd/MM/yyyy");
            if (pf == fecha) {
                filas.push_back({p.correlativo, f.numero(),
                                 nombreCliente(f.codigoCliente), p.fecha, p.monto});
                total += p.monto;
            }
        }
    }

    tabla->setRowCount(static_cast<int>(filas.size()));
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
        const Fila &x = filas[i];
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(x.noPago)));
        tabla->setItem(i, 1, new QTableWidgetItem(x.factura));
        tabla->setItem(i, 2, new QTableWidgetItem(x.cliente));
        tabla->setItem(i, 3, new QTableWidgetItem(x.fecha));
        tabla->setItem(i, 4, new QTableWidgetItem("Q " + QString::number(x.monto, 'f', 2)));
    }

    lblResumen->setText(QString("Pagos del %1: %2   —   Total recibido: Q %3")
        .arg(fecha.toString("dd/MM/yyyy"))
        .arg(filas.size())
        .arg(QString::number(total, 'f', 2)));
}

void ReportesWidget::reporteInventario()
{
    prepararTabla({"Código", "Nombre", "Costo", "Precio", "Existencia", "Valor (al costo)"});
    const std::vector<Producto> &lista = Datos::instancia().productos;
    tabla->setRowCount(static_cast<int>(lista.size()));
    double valorTotal = 0.0;
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Producto &p = lista[i];
        double valor = p.existencia * p.costo;
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(p.codigo)));
        tabla->setItem(i, 1, new QTableWidgetItem(p.nombre));
        tabla->setItem(i, 2, new QTableWidgetItem("Q " + QString::number(p.costo, 'f', 2)));
        tabla->setItem(i, 3, new QTableWidgetItem("Q " + QString::number(p.precio, 'f', 2)));
        tabla->setItem(i, 4, new QTableWidgetItem(QString::number(p.existencia)));
        tabla->setItem(i, 5, new QTableWidgetItem("Q " + QString::number(valor, 'f', 2)));
        valorTotal += valor;
    }
    lblResumen->setText(QString("Productos: %1   —   Valor del inventario (al costo): Q %2")
        .arg(lista.size())
        .arg(QString::number(valorTotal, 'f', 2)));
}

void ReportesWidget::exportarPdf()
{
    if (tabla->rowCount() == 0) {
        QMessageBox::information(this, "Exportar a PDF",
            "Primero genere un reporte con datos para poder exportarlo.");
        return;
    }

    // Nombre sugerido para el archivo (se limpia de caracteres no válidos)
    QString nombreReporte = comboReporte->currentText();
    QString sugerido = "Reporte - " + nombreReporte + ".pdf";
    sugerido.replace("/", "-").replace("\\", "-").replace(":", "-");

    // Diálogo para que el usuario elija la CARPETA y el nombre del archivo
    QString ruta = QFileDialog::getSaveFileName(this,
        "Guardar reporte como PDF", sugerido, "Archivos PDF (*.pdf)");
    if (ruta.isEmpty()) return;   // el usuario canceló
    if (!ruta.endsWith(".pdf", Qt::CaseInsensitive)) ruta += ".pdf";

    // Se arma el reporte en HTML leyendo lo que tiene la tabla
    QString html;
    html += "<h2 style='color:#1F3864; margin-bottom:0;'>Sistema de Facturación Chapinas</h2>";
    html += "<p style='margin-top:2px;'>Representaciones &quot;Chapinas&quot;, C.A.</p>";
    html += "<h3 style='color:#2E5496;'>" + nombreReporte.toHtmlEscaped() + "</h3>";
    html += "<p>Fecha de generación: " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>";

    html += "<table border='1' cellspacing='0' cellpadding='4' width='100%'>";
    html += "<tr style='background-color:#2E5496; color:#FFFFFF;'>";
    for (int c = 0; c < tabla->columnCount(); ++c) {
        QString h = tabla->horizontalHeaderItem(c) ? tabla->horizontalHeaderItem(c)->text() : "";
        html += "<th>" + h.toHtmlEscaped() + "</th>";
    }
    html += "</tr>";

    for (int r = 0; r < tabla->rowCount(); ++r) {
        html += "<tr>";
        for (int c = 0; c < tabla->columnCount(); ++c) {
            QTableWidgetItem *it = tabla->item(r, c);
            QString v = it ? it->text() : "";
            html += "<td>" + v.toHtmlEscaped() + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    if (!lblResumen->text().isEmpty())
        html += "<p style='margin-top:10px;'><b>" + lblResumen->text().toHtmlEscaped() + "</b></p>";

    // Se genera el PDF con QPrinter
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(ruta);
    printer.setPageSize(QPageSize(QPageSize::Letter));
    printer.setPageOrientation(QPageLayout::Landscape);   // horizontal, útil para tablas anchas

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    QMessageBox::information(this, "Exportar a PDF",
        "El reporte se guardó correctamente en:\n" + ruta);
}