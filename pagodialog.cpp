#include "pagodialog.h"

#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDate>
#include <QFont>

PagoDialog::PagoDialog(const Factura &f, const QString &clienteNombre, QWidget *parent)
    : QDialog(parent), saldo(f.saldo)
{
    setWindowTitle("Registrar pago");
    setMinimumWidth(420);

    QLabel *lblNumero = new QLabel(f.numero(), this);
    QFont fb = lblNumero->font(); fb.setBold(true); lblNumero->setFont(fb);

    QLabel *lblCliente = new QLabel(clienteNombre, this);
    QLabel *lblTotal = new QLabel("Q " + QString::number(f.total, 'f', 2), this);
    QLabel *lblSaldo = new QLabel("Q " + QString::number(f.saldo, 'f', 2), this);
    QFont fs = lblSaldo->font(); fs.setBold(true); lblSaldo->setFont(fs);

    editFecha = new QDateEdit(QDate::currentDate(), this);
    editFecha->setCalendarPopup(true);
    editFecha->setDisplayFormat("dd/MM/yyyy");

    editMonto = new QLineEdit(this);
    editMonto->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*\\.?[0-9]*"), this));
    editMonto->setPlaceholderText("Ej. 100.00");

    lblMontoIcono = new QLabel(this);
    lblMontoIcono->setFixedWidth(22);
    lblMontoIcono->setAlignment(Qt::AlignCenter);

    lblMontoMsg = new QLabel(this);
    lblMontoMsg->setWordWrap(true);
    QFont fm = lblMontoMsg->font(); fm.setPointSize(fm.pointSize() - 1); lblMontoMsg->setFont(fm);

    QHBoxLayout *filaMonto = new QHBoxLayout;
    filaMonto->setContentsMargins(0, 0, 0, 0);
    filaMonto->addWidget(editMonto);
    filaMonto->addWidget(lblMontoIcono);

    QFormLayout *form = new QFormLayout;
    form->addRow("No. Factura:", lblNumero);
    form->addRow("Cliente:", lblCliente);
    form->addRow("Total:", lblTotal);
    form->addRow("Saldo pendiente:", lblSaldo);
    form->addRow("Fecha:", editFecha);
    form->addRow("Monto del pago:", filaMonto);
    form->addRow("", lblMontoMsg);

    QDialogButtonBox *botones = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    botones->button(QDialogButtonBox::Ok)->setText("Registrar pago");
    botones->button(QDialogButtonBox::Cancel)->setText("Cancelar");

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addLayout(form);
    principal->addWidget(botones);

    connect(editMonto, &QLineEdit::textChanged, this, [this]{ validarMonto(false); });
    connect(botones, &QDialogButtonBox::accepted, this, [this]() {
        if (validarMonto(true)) accept();
    });
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool PagoDialog::validarMonto(bool forzar)
{
    const QString t = editMonto->text().trimmed();

    auto pintar = [this](int estado, const QString &msg) {
        if (estado == 1) {
            editMonto->setStyleSheet("QLineEdit { border: 2px solid #27AE60; border-radius: 4px; padding: 3px; }");
            lblMontoIcono->setText("\u2714");
            lblMontoIcono->setStyleSheet("color: #27AE60; font-weight: bold;");
            lblMontoMsg->setText("Correcto");
            lblMontoMsg->setStyleSheet("color: #27AE60;");
        } else if (estado == 2) {
            editMonto->setStyleSheet("QLineEdit { border: 2px solid #C0392B; border-radius: 4px; padding: 3px; }");
            lblMontoIcono->setText("\u2716");
            lblMontoIcono->setStyleSheet("color: #C0392B; font-weight: bold;");
            lblMontoMsg->setText(msg);
            lblMontoMsg->setStyleSheet("color: #C0392B;");
        } else {
            editMonto->setStyleSheet("");
            lblMontoIcono->clear();
            lblMontoMsg->clear();
        }
    };

    if (t.isEmpty()) {
        pintar(forzar ? 2 : 0, "Ingrese el monto del pago.");
        return false;
    }
    bool ok = false;
    double v = t.toDouble(&ok);
    if (!ok || v <= 0.0) {
        pintar(2, "El monto debe ser un número mayor que 0.");
        return false;
    }
    if (v > saldo + 0.0001) {   // tolerancia por decimales
        pintar(2, QString("El monto no puede ser mayor al saldo (Q %1).")
                     .arg(QString::number(saldo, 'f', 2)));
        return false;
    }
    pintar(1, "");
    return true;
}

QString PagoDialog::fecha() const
{
    return editFecha->date().toString("dd/MM/yyyy");
}

double PagoDialog::monto() const
{
    return editMonto->text().trimmed().toDouble();
}