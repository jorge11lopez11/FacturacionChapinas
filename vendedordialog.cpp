#include "vendedordialog.h"

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

VendedorDialog::VendedorDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Vendedor");
    setMinimumWidth(400);

    editDpi = new QLineEdit(this);
    editNombre = new QLineEdit(this);
    editNombre->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-zÁÉÍÓÚÜÑáéíóúüñ .'-]*"), this));
    editTelefono = new QLineEdit(this);
    editDireccion = new QLineEdit(this);
    editEmail = new QLineEdit(this);

    // Ayudas mientras se escribe
    editDpi->setMaxLength(13);
    editDpi->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    editDpi->setPlaceholderText("13 dígitos");

    editTelefono->setMaxLength(8);
    editTelefono->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    editTelefono->setPlaceholderText("8 dígitos");

    editEmail->setPlaceholderText("nombre@dominio.ext");

    // Formulario con indicadores (ícono + mensaje) por campo
    QFormLayout *form = new QFormLayout;
    form->addRow("DPI:", crearFila(editDpi, lblDpiIcono, lblDpiMsg));
    form->addRow("Nombre:", crearFila(editNombre, lblNombreIcono, lblNombreMsg));
    form->addRow("Teléfono:", crearFila(editTelefono, lblTelIcono, lblTelMsg));
    form->addRow("Dirección:", editDireccion);   // campo libre, sin validación
    form->addRow("E-mail:", crearFila(editEmail, lblEmailIcono, lblEmailMsg));

    // Botones Aceptar / Cancelar
    QDialogButtonBox *botones = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    botones->button(QDialogButtonBox::Ok)->setText("Aceptar");
    botones->button(QDialogButtonBox::Cancel)->setText("Cancelar");

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addLayout(form);
    principal->addWidget(botones);

    // Validación EN VIVO (mientras se escribe): vacío = neutral
    connect(editDpi,      &QLineEdit::textChanged, this, [this]{ validarDpi(false); });
    connect(editNombre,   &QLineEdit::textChanged, this, [this]{ validarNombre(false); });
    connect(editTelefono, &QLineEdit::textChanged, this, [this]{ validarTelefono(false); });
    connect(editEmail,    &QLineEdit::textChanged, this, [this]{ validarEmail(false); });

    // Al pulsar Aceptar: valida todo (vacío = error) y solo cierra si todo va bien
    connect(botones, &QDialogButtonBox::accepted, this, [this]() {
        bool okNombre = validarNombre(true);
        bool okDpi    = validarDpi(true);
        bool okTel    = validarTelefono(true);
        bool okEmail  = validarEmail(true);
        if (okNombre && okDpi && okTel && okEmail)
            accept();
    });
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QWidget* VendedorDialog::crearFila(QLineEdit *campo, QLabel *&icono, QLabel *&mensaje)
{
    icono = new QLabel(this);
    icono->setFixedWidth(60);
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

void VendedorDialog::pintarEstado(QLineEdit *campo, QLabel *icono, QLabel *mensaje,
                                  int estado, const QString &textoError)
{
    if (estado == 1) {            // válido (verde)
        campo->setStyleSheet("QLineEdit { border: 2px solid #27AE60; border-radius: 4px; padding: 3px; }");
        icono->setText("\u2714");                              // ✔
        icono->setStyleSheet("color: #27AE60; font-weight: bold;");
        mensaje->setText("Correcto");
        mensaje->setStyleSheet("color: #27AE60;");
    } else if (estado == 2) {     // inválido (rojo)
        campo->setStyleSheet("QLineEdit { border: 2px solid #C0392B; border-radius: 4px; padding: 3px; }");
        icono->setText("\u2716");                              // ✖
        icono->setStyleSheet("color: #C0392B; font-weight: bold;");
        mensaje->setText(textoError);
        mensaje->setStyleSheet("color: #C0392B;");
    } else {                      // neutral (campo vacío)
        campo->setStyleSheet("");
        icono->clear();
        mensaje->clear();
    }
}

bool VendedorDialog::validarNombre(bool forzar)
{
    const QString t = editNombre->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editNombre, lblNombreIcono, lblNombreMsg, forzar ? 2 : 0,
                     "El nombre es obligatorio.");
        return false;
    }
    pintarEstado(editNombre, lblNombreIcono, lblNombreMsg, 1, "");
    return true;
}

bool VendedorDialog::validarDpi(bool forzar)
{
    const QString t = editDpi->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editDpi, lblDpiIcono, lblDpiMsg, forzar ? 2 : 0,
                     "El DPI es obligatorio (13 dígitos).");
        return false;
    }
    static const QRegularExpression re("^[0-9]{13}$");
    bool ok = re.match(t).hasMatch();
    pintarEstado(editDpi, lblDpiIcono, lblDpiMsg, ok ? 1 : 2,
                 "El DPI debe tener exactamente 13 dígitos.");
    return ok;
}

bool VendedorDialog::validarTelefono(bool forzar)
{
    const QString t = editTelefono->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editTelefono, lblTelIcono, lblTelMsg, forzar ? 2 : 0,
                     "El teléfono es obligatorio (8 dígitos).");
        return false;
    }
    static const QRegularExpression re("^[0-9]{8}$");
    bool ok = re.match(t).hasMatch();
    pintarEstado(editTelefono, lblTelIcono, lblTelMsg, ok ? 1 : 2,
                 "El teléfono debe tener exactamente 8 dígitos.");
    return ok;
}

bool VendedorDialog::validarEmail(bool forzar)
{
    const QString t = editEmail->text().trimmed();
    if (t.isEmpty()) {
        pintarEstado(editEmail, lblEmailIcono, lblEmailMsg, forzar ? 2 : 0,
                     "El e-mail es obligatorio.");
        return false;
    }
    // Ahora SÍ permite números. Solo letras, números, _ y -,
    // con una sola @ y un solo punto: nombre@dominio.ext
    static const QRegularExpression re("^(?!(?:.*\\.){3})[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?(?:\\.[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?)*@[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?(?:\\.[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?)+$");
    bool ok = re.match(t).hasMatch();
    pintarEstado(editEmail, lblEmailIcono, lblEmailMsg, ok ? 1 : 2,
                 "E-mail no válido. Use nombre@dominio.ext (letras, números, _ o -).");
    return ok;
}

void VendedorDialog::setVendedor(const Vendedor &v)
{
    codigoActual = v.codigo;
    editDpi->setText(v.dpi);
    editNombre->setText(v.nombre);
    editTelefono->setText(v.telefono);
    editDireccion->setText(v.direccion);
    editEmail->setText(v.email);
}

Vendedor VendedorDialog::vendedor() const
{
    Vendedor v;
    v.codigo = codigoActual;
    v.dpi = editDpi->text().trimmed();
    v.nombre = editNombre->text().trimmed();
    v.telefono = editTelefono->text().trimmed();
    v.direccion = editDireccion->text().trimmed();
    v.email = editEmail->text().trimmed();
    return v;
}