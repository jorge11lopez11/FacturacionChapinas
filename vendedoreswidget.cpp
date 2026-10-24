#include "vendedoreswidget.h"
#include "vendedordialog.h"
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

VendedoresWidget::VendedoresWidget(QWidget *parent)
    : QWidget(parent)
{
    // Título de la página
    QLabel *titulo = new QLabel("Vendedores");
    QFont f = titulo->font();
    f.setPointSize(14);
    f.setBold(true);
    titulo->setFont(f);

    // Tabla
    tabla = new QTableWidget(this);
    tabla->setColumnCount(6);
    tabla->setHorizontalHeaderLabels(
        {"Código", "DPI", "Nombre", "Teléfono", "Dirección", "E-mail"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);   // no editar en la tabla
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);  // seleccionar la fila
    tabla->setSelectionMode(QAbstractItemView::SingleSelection);

    // Botones
    QPushButton *btnAgregar = new QPushButton("Agregar");
    QPushButton *btnModificar = new QPushButton("Modificar");
    QPushButton *btnEliminar = new QPushButton("Eliminar");

    QHBoxLayout *filaBotones = new QHBoxLayout;
    filaBotones->addWidget(btnAgregar);
    filaBotones->addWidget(btnModificar);
    filaBotones->addWidget(btnEliminar);
    filaBotones->addStretch();   // empuja los botones a la izquierda

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addWidget(titulo);
    principal->addLayout(filaBotones);
    principal->addWidget(tabla);

    // Conectar cada botón con su función
    connect(btnAgregar,   &QPushButton::clicked, this, &VendedoresWidget::agregar);
    connect(btnModificar, &QPushButton::clicked, this, &VendedoresWidget::modificar);
    connect(btnEliminar,  &QPushButton::clicked, this, &VendedoresWidget::eliminar);

    refrescarTabla();
}

void VendedoresWidget::refrescarTabla()
{
    const std::vector<Vendedor> &lista = Datos::instancia().vendedores;

    tabla->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Vendedor &v = lista[i];
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(v.codigo)));
        tabla->setItem(i, 1, new QTableWidgetItem(v.dpi));
        tabla->setItem(i, 2, new QTableWidgetItem(v.nombre));
        tabla->setItem(i, 3, new QTableWidgetItem(v.telefono));
        tabla->setItem(i, 4, new QTableWidgetItem(v.direccion));
        tabla->setItem(i, 5, new QTableWidgetItem(v.email));
    }
}

void VendedoresWidget::agregar()
{
    VendedorDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Vendedor v = dlg.vendedor();
        v.codigo = Datos::instancia().siguienteCodigoVendedor++;
        Datos::instancia().vendedores.push_back(v);
        refrescarTabla();
    }
}

void VendedoresWidget::modificar()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Modificar",
                                "Seleccione un vendedor de la tabla.");
        return;
    }

    VendedorDialog dlg(this);
    dlg.setVendedor(Datos::instancia().vendedores[fila]);
    if (dlg.exec() == QDialog::Accepted) {
        Datos::instancia().vendedores[fila] = dlg.vendedor();  // conserva el código
        refrescarTabla();
    }
}

void VendedoresWidget::eliminar()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Eliminar",
                                "Seleccione un vendedor de la tabla.");
        return;
    }

    const Vendedor &v = Datos::instancia().vendedores[fila];
    QString msg = QString("¿Eliminar al vendedor \"%1\"?").arg(v.nombre);
    if (QMessageBox::question(this, "Eliminar", msg) == QMessageBox::Yes) {
        auto &vs = Datos::instancia().vendedores;
        vs.erase(vs.begin() + fila);
        refrescarTabla();
    }
}