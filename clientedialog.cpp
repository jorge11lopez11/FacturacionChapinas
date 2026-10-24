#include "clientedialog.h"
#include "datos.h"

#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QFont>
#include <QWidget>

ClienteDialog::ClienteDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Cliente");
    setMinimumWidth(480);

    editNombre = new QLineEdit(this);
    editNombre->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-zÁÉÍÓÚÜÑáéíóúüñ .'-]*"), this));
    editNit = new QLineEdit(this);
    editTelefono = new QLineEdit(this);
    editDireccion = new QLineEdit(this);
    editContacto = new QLineEdit(this);
    editContacto->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-zÁÉÍÓÚÜÑáéíóúüñ .'-]*"), this));
    editEmail = new QLineEdit(this);
    editWeb = new QLineEdit(this);
    comboVendedor = new QComboBox(this);

    // Cargar los vendedores existentes en el desplegable
    for (const Vendedor &v : Datos::instancia().vendedores) {
        comboVendedor->addItem(QString("%1 - %2").arg(v.codigo).arg(v.nombre), v.codigo);
    }

    // Ayudas mientras se escribe
    editTelefono->setMaxLength(8);
    editTelefono->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    editTelefono->setPlaceholderText("8 dígitos");
    editNit->setPlaceholderText("8 a 13 dígitos (ej. 1234567-8)");
        editNit->setMaxLength(14);   // 13 dígitos + un guion opcional
        editNit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9KkCFcf/-]*"), this));
    editEmail->setPlaceholderText("nombre@dominio.ext (opcional)");
    editWeb->setPlaceholderText("opcional");
    editContacto->setPlaceholderText("opcional");

    QFormLayout *form = new QFormLayout;
    form->addRow("Nombre:", crearFila(editNombre, lblNombreIcono, lblNombreMsg));
    form->addRow("NIT:", crearFila(editNit, lblNitIcono, lblNitMsg));
    form->addRow("Teléfono:", crearFila(editTelefono, lblTelIcono, lblTelMsg));
    form->addRow("Dirección:", editDireccion);
    form->addRow("Persona de contacto:", editContacto);
    form->addRow("E-mail:", crearFila(editEmail, lblEmailIcono, lblEmailMsg));
    form->addRow("Página web:", editWeb);
    form->addRow("Vendedor:", comboVendedor);

    QDialogButtonBox *botones = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    botones->button(QDialogButtonBox::Ok)->setText("Aceptar");
    botones->button(QDialogButtonBox::Cancel)->setText("Cancelar");

    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addLayout(form);
    principal->addWidget(botones);

    connect(editNombre,   &QLineEdit::textChanged, this, [this]{ validarNombre(false); });
    connect(editNit,      &QLineEdit::textChanged, this, [this]{ validarNit(false); });
    connect(editTelefono, &QLineEdit::textChanged, this, [this]{ validarTelefono(false); });
    connect(editEmail,    &QLineEdit::textChanged, this, [this]{ validarEmail(false); });

    connect(botones, &QDialogButtonBox::accepted, this, [this]() {
        bool okNombre = validarNombre(true);
        bool okNit    = validarNit(true);
        bool okTel    = validarTelefono(true);
        bool okEmail  = validarEmail(true);
        if (okNombre && okNit && okTel && okEmail)
            accept();
    });
    connect(botones, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QWidget* ClienteDialog::crearFila(QLineEdit *campo, QLabel *&icono, QLabel *&mensaje)
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

void ClienteDialog::pintarEstado(QLineEdit *campo, QLabel *icono, QLabel *mensaje,
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

bool ClienteDialog::validarNombre(bool forzar)
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

bool ClienteDialog::validarNit(bool forzar)
{
    const QString original = editNit->text().trimmed();
    if (original.isEmpty()) {
        pintarEstado(editNit, lblNitIcono, lblNitMsg, forzar ? 2 : 0,
                     "El NIT es obligatorio.");
        return false;
    }

    QString t = original.toUpper();

    // "CF" = Consumidor Final (se acepta como NIT válido)
    if (t == "CF" || t == "C/F") {
        pintarEstado(editNit, lblNitIcono, lblNitMsg, 1, "");
        return true;
    }

    // Se permite un guion opcional antes del dígito verificador
    QString limpio = t;
    limpio.remove('-');

    // 8 a 13 caracteres: dígitos y, como verificador final, un dígito o la letra K.
    // ({7,12} dígitos + 1 verificador = 8 a 13 en total)
    static const QRegularExpression re("^[0-9]{7,12}[0-9K]$");
    bool ok = re.match(limpio).hasMatch();
    pintarEstado(editNit, lblNitIcono, lblNitMsg, ok ? 1 : 2,
                 "NIT no válido. De 8 a 13 dígitos (el último puede ser K); "
                 "guion opcional o \"CF\".");
    return ok;
}

bool ClienteDialog::validarTelefono(bool forzar)
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

bool ClienteDialog::validarEmail(bool forzar)
{
    Q_UNUSED(forzar);
    const QString t = editEmail->text().trimmed();
    if (t.isEmpty()) {
        // El e-mail es opcional: vacío se considera válido
        pintarEstado(editEmail, lblEmailIcono, lblEmailMsg, 0, "");
        return true;
    }
    static const QRegularExpression re("^(?!(?:.*\\.){3})[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?(?:\\.[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?)*@[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?(?:\\.[A-Za-z0-9](?:[A-Za-z0-9_-]*[A-Za-z0-9])?)+$");
    bool ok = re.match(t).hasMatch();
    pintarEstado(editEmail, lblEmailIcono, lblEmailMsg, ok ? 1 : 2,
                 "E-mail no válido. Use nombre@dominio.ext (máx. 2 puntos).");
    return ok;
}

void ClienteDialog::setCliente(const Cliente &c)
{
    codigoActual = c.codigo;
    editNombre->setText(c.nombre);
    editNit->setText(c.nit);
    editTelefono->setText(c.telefono);
    editDireccion->setText(c.direccion);
    editContacto->setText(c.contacto);
    editEmail->setText(c.email);
    editWeb->setText(c.paginaWeb);

    int idx = comboVendedor->findData(c.codigoVendedor);
    if (idx >= 0)
        comboVendedor->setCurrentIndex(idx);
}

Cliente ClienteDialog::cliente() const
{
    Cliente c;
    c.codigo = codigoActual;
    c.nombre = editNombre->text().trimmed();
    c.nit = editNit->text().trimmed();
    c.telefono = editTelefono->text().trimmed();
    c.direccion = editDireccion->text().trimmed();
    c.contacto = editContacto->text().trimmed();
    c.email = editEmail->text().trimmed();
    c.paginaWeb = editWeb->text().trimmed();
    c.codigoVendedor = comboVendedor->currentData().toInt();
    return c;
}