#include "productoswidget.h"
#include "productodialog.h"
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
#include <QShowEvent>

ProductosWidget::ProductosWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel *titulo = new QLabel("Productos");
    QFont f = titulo->font();
    f.setPointSize(14);
    f.setBold(true);
    titulo->setFont(f);

    tabla = new QTableWidget(this);
    tabla->setColumnCount(5);
    tabla->setHorizontalHeaderLabels(
        {"Código", "Nombre", "Costo", "Precio", "Existencia"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabla->setSelectionMode(QAbstractItemView::SingleSelection);

    QPushButton *btnAgregar = new QPushButton("Agregar");
    QPushButton *btnModificar = new QPushButton("Modificar");
    QPushButton *btnEliminar = new QPushButton("Eliminar");

    QHBoxLayout *filaBotones = new QHBoxLayout;
    filaBotones->addWidget(btnAgregar);
    filaBotones->addWidget(btnModificar);
    filaBotones->addWidget(btnEliminar);
    filaBotones->addStretch();

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addWidget(titulo);
    principal->addLayout(filaBotones);
    principal->addWidget(tabla);

    connect(btnAgregar,   &QPushButton::clicked, this, &ProductosWidget::agregar);
    connect(btnModificar, &QPushButton::clicked, this, &ProductosWidget::modificar);
    connect(btnEliminar,  &QPushButton::clicked, this, &ProductosWidget::eliminar);

    refrescarTabla();
}

void ProductosWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refrescarTabla();   // así refleja la existencia actualizada por las ventas
}

void ProductosWidget::refrescarTabla()
{
    const std::vector<Producto> &lista = Datos::instancia().productos;

    tabla->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Producto &p = lista[i];
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(p.codigo)));
        tabla->setItem(i, 1, new QTableWidgetItem(p.nombre));
        tabla->setItem(i, 2, new QTableWidgetItem("Q " + QString::number(p.costo, 'f', 2)));
        tabla->setItem(i, 3, new QTableWidgetItem("Q " + QString::number(p.precio, 'f', 2)));
        tabla->setItem(i, 4, new QTableWidgetItem(QString::number(p.existencia)));
    }
}

void ProductosWidget::agregar()
{
    ProductoDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Producto p = dlg.producto();
        p.codigo = Datos::instancia().siguienteCodigoProducto++;
        Datos::instancia().productos.push_back(p);
        refrescarTabla();
    }
}

void ProductosWidget::modificar()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Modificar",
                                "Seleccione un producto de la tabla.");
        return;
    }

    ProductoDialog dlg(this);
    dlg.setProducto(Datos::instancia().productos[fila]);
    if (dlg.exec() == QDialog::Accepted) {
        Datos::instancia().productos[fila] = dlg.producto();
        refrescarTabla();
    }
}

void ProductosWidget::eliminar()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Eliminar",
                                "Seleccione un producto de la tabla.");
        return;
    }

    const Producto &p = Datos::instancia().productos[fila];
    QString msg = QString("¿Eliminar el producto \"%1\"?").arg(p.nombre);
    if (QMessageBox::question(this, "Eliminar", msg) == QMessageBox::Yes) {
        auto &ps = Datos::instancia().productos;
        ps.erase(ps.begin() + fila);
        refrescarTabla();
    }
}