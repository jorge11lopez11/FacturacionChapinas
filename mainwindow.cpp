#include "mainwindow.h"
#include "vendedoreswidget.h"
#include "clienteswidget.h"
#include "productoswidget.h"
#include "facturaswidget.h"
#include "pagoswidget.h"
#include "reporteswidget.h"

#include <QMenuBar>
#include <QLabel>
#include <QStackedWidget>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Sistema de Facturación Chapinas");
    resize(1000, 640);

    paginas = new QStackedWidget(this);
    setCentralWidget(paginas);

    QLabel *bienvenida = new QLabel(
        "Sistema de Facturación Chapinas\n"
        "Representaciones \"Chapinas\", C.A.");
    bienvenida->setAlignment(Qt::AlignCenter);
    paginaInicio = bienvenida;
    paginas->addWidget(paginaInicio);

    paginaVendedores = new VendedoresWidget(this);
    paginas->addWidget(paginaVendedores);

    paginaClientes = new ClientesWidget(this);
    paginas->addWidget(paginaClientes);

    paginaProductos = new ProductosWidget(this);
    paginas->addWidget(paginaProductos);

    paginaFacturacion = new FacturasWidget(this);
    paginas->addWidget(paginaFacturacion);

    paginaPagos = new PagosWidget(this);
    paginas->addWidget(paginaPagos);

    paginaReportes = new ReportesWidget(this);
    paginas->addWidget(paginaReportes);

    QAction *accVendedores  = new QAction("Vendedores", this);
    QAction *accClientes    = new QAction("Clientes", this);
    QAction *accProductos   = new QAction("Productos", this);
    QAction *accFacturacion = new QAction("Facturación", this);
    QAction *accPagos       = new QAction("Pagos", this);
    QAction *accReportes    = new QAction("Reportes", this);

    menuBar()->addAction(accVendedores);
    menuBar()->addAction(accClientes);
    menuBar()->addAction(accProductos);
    menuBar()->addAction(accFacturacion);
    menuBar()->addAction(accPagos);
    menuBar()->addAction(accReportes);

    connect(accVendedores,  &QAction::triggered, this, &MainWindow::mostrarVendedores);
    connect(accClientes,    &QAction::triggered, this, &MainWindow::mostrarClientes);
    connect(accProductos,   &QAction::triggered, this, &MainWindow::mostrarProductos);
    connect(accFacturacion, &QAction::triggered, this, &MainWindow::mostrarFacturacion);
    connect(accPagos,       &QAction::triggered, this, &MainWindow::mostrarPagos);
    connect(accReportes,    &QAction::triggered, this, &MainWindow::mostrarReportes);

    paginas->setCurrentWidget(paginaInicio);
}

MainWindow::~MainWindow()
{
}

void MainWindow::mostrarVendedores()   { paginas->setCurrentWidget(paginaVendedores); }
void MainWindow::mostrarClientes()     { paginas->setCurrentWidget(paginaClientes); }
void MainWindow::mostrarProductos()    { paginas->setCurrentWidget(paginaProductos); }
void MainWindow::mostrarFacturacion()  { paginas->setCurrentWidget(paginaFacturacion); }
void MainWindow::mostrarPagos()        { paginas->setCurrentWidget(paginaPagos); }
void MainWindow::mostrarReportes()     { paginas->setCurrentWidget(paginaReportes); }