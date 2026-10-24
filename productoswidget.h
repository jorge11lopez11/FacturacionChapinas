#ifndef PRODUCTOSWIDGET_H
#define PRODUCTOSWIDGET_H

#include <QWidget>

class QTableWidget;
class QShowEvent;

class ProductosWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProductosWidget(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;   // refresca al mostrarse

private slots:
    void agregar();
    void modificar();
    void eliminar();

private:
    void refrescarTabla();
    QTableWidget *tabla;
};

#endif // PRODUCTOSWIDGET_H