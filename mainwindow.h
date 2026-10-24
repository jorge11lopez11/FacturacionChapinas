#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void mostrarVendedores();
    void mostrarClientes();
    void mostrarProductos();
    void mostrarFacturacion();
    void mostrarPagos();
    void mostrarReportes();

private:
    QStackedWidget *paginas;
    QWidget *paginaInicio;
    QWidget *paginaVendedores;
    QWidget *paginaClientes;
    QWidget *paginaProductos;
    QWidget *paginaFacturacion;
    QWidget *paginaPagos;
    QWidget *paginaReportes;
};

#endif // MAINWINDOW_H