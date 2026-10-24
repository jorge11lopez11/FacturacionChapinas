#ifndef PAGOSWIDGET_H
#define PAGOSWIDGET_H

#include <QWidget>

class QTableWidget;
class QShowEvent;

class PagosWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PagosWidget(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void registrarPago();
    void verPagos();

private:
    void refrescarTabla();
    QString nombreCliente(int codigo) const;
    QTableWidget *tabla;
};

#endif // PAGOSWIDGET_H