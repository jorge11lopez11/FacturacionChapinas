#include "productodialog.h"
#include "datos.h"   // para verificar nombres de producto repetidos

#include <QLineEdit>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QFont>
#include <QWidget>

ProductoDialog::ProductoDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Producto");
    setMinimumWidth(460);

    editNombre = new QLineEdit(this);
    editCosto = new QLineEdit(this);
    editPrecio = new QLineEdit(this);
    editExistencia = new QLineEdit(this);

    // Costo y precio: dígitos con un punto decimal opcional
    editCosto->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*\\.?[0-9]*"), this));
    editPrecio->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*\\.?[0-9]*"), this));
    editCosto->setPlaceholderText("Ej. 12.50");
    editPrecio->setPlaceholderText("Ej. 15.00");

    // Existencia: solo dígitos (entero)
    editExistencia->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*"), this));
    editExistencia->setPlaceholderText("Ej. 100");

    QFormLayout *form = new QFormLayout;
    form->addRow("Nombre:", crearFila(editNombre, lblNombreIcono, lblNombreMsg));
    form->addRow("Costo (Q):", crearFila(editCosto, lblCostoIcono, lblCostoMsg));
    form->addRow("Precio (Q):", crearFila(editPrecio, lblPrecioIcono, lblPrecioMsg));
    form->addRow("Existencia:", crearFila(editExistencia, lblExistIcono, lblExistMsg));

    QDialogButtonBox *botones = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    botones->button(QDialogButtonBox::Ok)->setText("Aceptar");
    botones->button(QDialogButtonBox::Cancel)->setText("Cancelar");

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addLayout(form);
    principal->addWidget(botones);

    connect(editNombre,     &QLineEdit::textChanged, this, [this]{ validarNombre(false); });
    connect(editCosto,      &QLineEdit::textChanged, this, [this]{
        validarCosto(false);
        // si el precio ya tiene algo, se revisa de nuevo que siga siendo mayor que el costo
        if (!editPrecio->text().trimmed().isEmpty()) validarPrecio(false);
    });
    connect(editPrecio,     &QLineEdit::textChanged, this, [this]{ validarPrecio(false); });
    connect(editExistencia, &QLineEdit::textChanged, this, [this]{ validarExistencia(false); });

    connect(botones, &QDialogButtonBox::accepted, this, [this]() {
        bool okNombre = validarNombre(true);
        bool okCosto  = validarCosto(true);
        bool okPrecio = validarPrecio(true);
        bool okExist  = validarExistencia(true);
        if (okNombre && okCosto && okPrecio && okExist)
            accept();
    });
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QWidget* ProductoDialog::crearFila(QLineEdit *campo, QLabel *&icono, QLabel *&mensaje)
{
    icono = new QLabel(this);
    icono->setFixedWidth(22);
    icono->setAlignment(Qt::AlignCenter);

    mensaje = new QLabel(this);
    mensaje->setWordWrap(true);
    QFont f = mensaje->font();
    f.setPointSize(f.pointSize() - 1);
    mensaje->setFont(f);

    QHBoxLayout *fila = new QHBoxLayout;
    fila->setContentsMargins(0, 0, 0, 0);
    fila->addWidget(campo);
    fila->addWidget(icono);

    QVBoxLayout *cont = new QVBoxLayout;
    cont->setContentsMargins(0, 0, 0, 0);
    cont->setSpacing(2);
    cont->addLayout(fila);
    cont->addWidget(mensaje);

    QWidget *w = new QWidget(this);
    w->setLayout(cont);
    return w;
}

void ProductoDialog::pintarEstado(QLineEdit *campo, QLabel *icono, QLabel *mensaje,
                                  int estado, const QString &textoError)
{
    if (estado == 1) {
        campo->setStyleSheet("QLineEdit { border: 2px solid #27AE60; border-radius: 4px; padding: 3px; }");
        icono->setText("\u2714");
        icono->setStyleSheet("color: #27AE60; font-weight: bold;");
        mensaje->setText("Correcto");
        mensaje->setStyleSheet("color: #27AE60;");
    } else if (estado == 2) {
        campo->setStyleSheet("QLineEdit { border: 2px solid #C0392B; border-radius: 4px; padding: 3px; }");
        icono->setText("\u2716");
        icono->setStyleSheet("color: #C0392B; font-weight: bold;");
        mensaje->setText(textoError);
        mensaje->setStyleSheet("color: #C0392B;");
    } else {
        campo->setStyleSheet("");
        icono->clear();
        mensaje->clear();
    }
}

bool ProductoDialog::validarNombre(bool forzar)
{
    const QString t = editNombre->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editNombre, lblNombreIcono, lblNombreMsg, forzar ? 2 : 0,
                     "El nombre es obligatorio.");
        return false;
    }
    // No se permiten dos productos con el mismo nombre
    for (const Producto &p : Datos::instancia().productos) {
        if (p.codigo != codigoActual &&
            p.nombre.compare(t, Qt::CaseInsensitive) == 0) {
            pintarEstado(editNombre, lblNombreIcono, lblNombreMsg, 2,
                         "Ya existe un producto con ese nombre.");
            return false;
        }
    }
    pintarEstado(editNombre, lblNombreIcono, lblNombreMsg, 1, "");
    return true;
}

bool ProductoDialog::validarCosto(bool forzar)
{
    const QString t = editCosto->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editCosto, lblCostoIcono, lblCostoMsg, forzar ? 2 : 0,
                     "El costo es obligatorio.");
        return false;
    }
    bool ok = false;
    double v = t.toDouble(&ok);
    bool valido = ok && v >= 0.0;
    pintarEstado(editCosto, lblCostoIcono, lblCostoMsg, valido ? 1 : 2,
                 "El costo debe ser un número no negativo (ej. 12.50).");
    return valido;
}

bool ProductoDialog::validarPrecio(bool forzar)
{
    const QString t = editPrecio->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editPrecio, lblPrecioIcono, lblPrecioMsg, forzar ? 2 : 0,
                     "El precio es obligatorio.");
        return false;
    }
    bool ok = false;
    double v = t.toDouble(&ok);
    if (!ok || v <= 0.0) {
        pintarEstado(editPrecio, lblPrecioIcono, lblPrecioMsg, 2,
                     "El precio debe ser un número mayor que 0 (ej. 15.00).");
        return false;
    }
    // El precio de venta debe ser MAYOR que el costo
    bool okCosto = false;
    double costo = editCosto->text().trimmed().toDouble(&okCosto);
    if (okCosto && v <= costo) {
        pintarEstado(editPrecio, lblPrecioIcono, lblPrecioMsg, 2,
                     QString("El precio debe ser mayor que el costo (Q %1).")
                         .arg(QString::number(costo, 'f', 2)));
        return false;
    }
    pintarEstado(editPrecio, lblPrecioIcono, lblPrecioMsg, 1, "");
    return true;
}

bool ProductoDialog::validarExistencia(bool forzar)
{
    const QString t = editExistencia->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editExistencia, lblExistIcono, lblExistMsg, forzar ? 2 : 0,
                     "La existencia es obligatoria.");
        return false;
    }
    bool ok = false;
    int v = t.toInt(&ok);
    bool valido = ok && v >= 0;
    pintarEstado(editExistencia, lblExistIcono, lblExistMsg, valido ? 1 : 2,
                 "La existencia debe ser un número entero no negativo.");
    return valido;
}

void ProductoDialog::setProducto(const Producto &p)
{
    codigoActual = p.codigo;
    editNombre->setText(p.nombre);
    editCosto->setText(QString::number(p.costo, 'f', 2));
    editPrecio->setText(QString::number(p.precio, 'f', 2));
    editExistencia->setText(QString::number(p.existencia));
}

Producto ProductoDialog::producto() const
{
    Producto p;
    p.codigo = codigoActual;
    p.nombre = editNombre->text().trimmed();
    p.costo = editCosto->text().trimmed().toDouble();
    p.precio = editPrecio->text().trimmed().toDouble();
    p.existencia = editExistencia->text().trimmed().toInt();
    return p;
}