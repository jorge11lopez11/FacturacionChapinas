#ifndef REPORTESWIDGET_H
#define REPORTESWIDGET_H

#include <QWidget>
#include <QStringList>

class QComboBox;
class QDateEdit;
class QLabel;
class QTableWidget;
class QShowEvent;

class ReportesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportesWidget(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void reporteCambiado();
    void generar();
    void exportarPdf();          // <-- nuevo

private:
    void prepararTabla(const QStringList &headers);
    void reporteClientes();
    void reporteVendedores();
    void reporteProductosVendidos();
    void reporteVentasDia();
    void reportePagosDia();
    void reporteInventario();
    QString nombreCliente(int codigo) const;
    QString nombreVendedor(int codigo) const;

    QComboBox *comboReporte;
    QLabel *lblDesde;
    QDateEdit *dateDesde;
    QLabel *lblHasta;
    QDateEdit *dateHasta;
    QTableWidget *tabla;
    QLabel *lblResumen;
};

#endif // REPORTESWIDGET_H