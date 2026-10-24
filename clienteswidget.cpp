#include "clienteswidget.h"
#include "clientedialog.h"
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

ClientesWidget::ClientesWidget(QWidget *parent)
    : QWidget(parent)
{
    QLabel *titulo = new QLabel("Clientes");
    QFont f = titulo->font();
    f.setPointSize(14);
    f.setBold(true);
    titulo->setFont(f);

    tabla = new QTableWidget(this);
    tabla->setColumnCount(9);
    tabla->setHorizontalHeaderLabels(
        {"Código", "Nombre", "NIT", "Teléfono", "Dirección",
         "Contacto", "E-mail", "Página web", "Vendedor"});
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

    connect(btnAgregar,   &QPushButton::clicked, this, &ClientesWidget::agregar);
    connect(btnModificar, &QPushButton::clicked, this, &ClientesWidget::modificar);
    connect(btnEliminar,  &QPushButton::clicked, this, &ClientesWidget::eliminar);

    refrescarTabla();
}

QString ClientesWidget::nombreVendedor(int codigoVendedor) const
{
    for (const Vendedor &v : Datos::instancia().vendedores) {
        if (v.codigo == codigoVendedor)
            return QString("%1 - %2").arg(v.codigo).arg(v.nombre);
    }
    return "(sin asignar)";
}

void ClientesWidget::refrescarTabla()
{
    const std::vector<Cliente> &lista = Datos::instancia().clientes;

    tabla->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        const Cliente &c = lista[i];
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(c.codigo)));
        tabla->setItem(i, 1, new QTableWidgetItem(c.nombre));
        tabla->setItem(i, 2, new QTableWidgetItem(c.nit));
        tabla->setItem(i, 3, new QTableWidgetItem(c.telefono));
        tabla->setItem(i, 4, new QTableWidgetItem(c.direccion));
        tabla->setItem(i, 5, new QTableWidgetItem(c.contacto));
        tabla->setItem(i, 6, new QTableWidgetItem(c.email));
        tabla->setItem(i, 7, new QTableWidgetItem(c.paginaWeb));
        tabla->setItem(i, 8, new QTableWidgetItem(nombreVendedor(c.codigoVendedor)));
    }
}

void ClientesWidget::agregar()
{
    if (Datos::instancia().vendedores.empty()) {
        QMessageBox::information(this, "Clientes",
            "Primero debe registrar al menos un vendedor, "
            "porque cada cliente se asigna a un vendedor.");
        return;
    }

    ClienteDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Cliente c = dlg.cliente();
        c.codigo = Datos::instancia().siguienteCodigoCliente++;
        Datos::instancia().clientes.push_back(c);
        refrescarTabla();
    }
}

void ClientesWidget::modificar()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Modificar",
                                "Seleccione un cliente de la tabla.");
        return;
    }

    ClienteDialog dlg(this);
    dlg.setCliente(Datos::instancia().clientes[fila]);
    if (dlg.exec() == QDialog::Accepted) {
        Datos::instancia().clientes[fila] = dlg.cliente();
        refrescarTabla();
    }
}

void ClientesWidget::eliminar()
{
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::information(this, "Eliminar",
                                "Seleccione un cliente de la tabla.");
        return;
    }

    const Cliente &c = Datos::instancia().clientes[fila];
    QString msg = QString("¿Eliminar al cliente \"%1\"?").arg(c.nombre);
    if (QMessageBox::question(this, "Eliminar", msg) == QMessageBox::Yes) {
        auto &cs = Datos::instancia().clientes;
        cs.erase(cs.begin() + fila);
        refrescarTabla();
    }
}