#ifndef VENDEDORESWIDGET_H
#define VENDEDORESWIDGET_H

#include <QWidget>

class QTableWidget;

class VendedoresWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VendedoresWidget(QWidget *parent = nullptr);

private slots:
    void agregar();
    void modificar();
    void eliminar();

private:
    void refrescarTabla();
    QTableWidget *tabla;
};

#endif // VENDEDORESWIDGET_H