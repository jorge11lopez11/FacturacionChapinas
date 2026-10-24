#ifndef CLIENTESWIDGET_H
#define CLIENTESWIDGET_H

#include <QWidget>

class QTableWidget;

class ClientesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientesWidget(QWidget *parent = nullptr);

private slots:
    void agregar();
    void modificar();
    void eliminar();

private:
    void refrescarTabla();
    QString nombreVendedor(int codigoVendedor) const;
    QTableWidget *tabla;
};

#endif // CLIENTESWIDGET_H