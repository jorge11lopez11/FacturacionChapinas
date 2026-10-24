#ifndef FACTURASWIDGET_H
#define FACTURASWIDGET_H

#include <QWidget>

class QTableWidget;
class QShowEvent;

class FacturasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FacturasWidget(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void nuevaFactura();
    void verDetalle();
    void cancelarFactura();

private:
    void refrescarTabla();
    QString nombreCliente(int codigo) const;
    QString nombreVendedor(int codigo) const;
    QTableWidget *tabla;
};

#endif // FACTURASWIDGET_H