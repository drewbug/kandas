/***************************************************************************
 *   Copyright 2009 Stefan Majewsky <majewsky@gmx.net>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "adddialog.h"
#include "manager.h"

#include <QValidator>
#include <KLocalizedString>
#include <KMessageBox>

namespace Kandas
{
    namespace Client
    {

        class DeviceNameValidator : public QValidator
        {
            public:
                DeviceNameValidator(QObject *parent)
                    : QValidator(parent)
                {
                }
                virtual QValidator::State validate(QString &input, int &pos) const
                {
                    Q_UNUSED(pos)
                    input = input.remove('/').remove(' ');
                    return QValidator::Acceptable;
                }
        };

    }
}

Kandas::Client::AddDialog::AddDialog(Kandas::Client::Manager *manager)
    : m_manager(manager)
    , m_widget(new QWidget(this))
{
    //setup dialog
    m_ui.setupUi(m_widget);
    setMainWidget(m_widget);
    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(i18n("Register a drive"));
    setMaximumSize(QSize(500, 250));
    connect(this, SIGNAL(okClicked()), this, SLOT(handleOkClicked()));
    //setup input validation
    const QString keyInputMask = QLatin1String(">NNNNN"); //allow ASCII alphanumeric characters, convert to uppercase automatically
    m_ui.nameWidget->setValidator(new Kandas::Client::DeviceNameValidator(m_ui.nameWidget));
    connect(m_ui.nameWidget, SIGNAL(textChanged(const QString &)), this, SLOT(handleInput()));
    m_ui.readKeyWidget1->setInputMask(keyInputMask);
    connect(m_ui.readKeyWidget1, SIGNAL(textChanged(const QString &)), this, SLOT(handleInput()));
    m_ui.readKeyWidget2->setInputMask(keyInputMask);
    connect(m_ui.readKeyWidget2, SIGNAL(textChanged(const QString &)), this, SLOT(handleInput()));
    m_ui.readKeyWidget3->setInputMask(keyInputMask);
    connect(m_ui.readKeyWidget3, SIGNAL(textChanged(const QString &)), this, SLOT(handleInput()));
    m_ui.readKeyWidget4->setInputMask(keyInputMask);
    connect(m_ui.readKeyWidget4, SIGNAL(textChanged(const QString &)), this, SLOT(handleInput()));
    m_ui.writeKeyWidget->setInputMask(keyInputMask);
    connect(m_ui.writeKeyWidget, SIGNAL(textChanged(const QString &)), this, SLOT(handleInput()));
}

void Kandas::Client::AddDialog::showDialog()
{
    if (isVisible())
        return;
    //clear everything
    m_ui.nameWidget->clear();
    m_ui.readKeyWidget1->clear();
    m_ui.readKeyWidget2->clear();
    m_ui.readKeyWidget3->clear();
    m_ui.readKeyWidget4->clear();
    m_ui.writeKeyWidget->clear();
    //show dialog
    m_ui.nameWidget->setFocus(Qt::OtherFocusReason);
    show();
}

void Kandas::Client::AddDialog::handleInput()
{
    //check input
    bool acceptable = true;
    if (m_ui.nameWidget->text().isEmpty())
        acceptable = false;
    if (m_ui.readKeyWidget1->text().length() != 5)
        acceptable = false;
    if (m_ui.readKeyWidget2->text().length() != 5)
        acceptable = false;
    if (m_ui.readKeyWidget3->text().length() != 5)
        acceptable = false;
    if (m_ui.readKeyWidget4->text().length() != 5)
        acceptable = false;
    int writeKeyLength = m_ui.writeKeyWidget->text().length();
    if (writeKeyLength != 0 && writeKeyLength != 5)
        acceptable = false;
    //enable or disable OK button based on input validity
    enableButtonOk(acceptable);
    //transfer focus to next readKeyWidget if necessary
    if (sender() == m_ui.readKeyWidget1)
        if (m_ui.readKeyWidget1->text().length() == 5)
            m_ui.readKeyWidget2->setFocus(Qt::OtherFocusReason);
    if (sender() == m_ui.readKeyWidget2)
        if (m_ui.readKeyWidget2->text().length() == 5)
            m_ui.readKeyWidget3->setFocus(Qt::OtherFocusReason);
    if (sender() == m_ui.readKeyWidget3)
        if (m_ui.readKeyWidget3->text().length() == 5)
            m_ui.readKeyWidget4->setFocus(Qt::OtherFocusReason);
}

void Kandas::Client::AddDialog::handleOkClicked()
{
    //can assume validity of the input at this point
    const QString name = m_ui.nameWidget->text();
    QStringList readKey;
    readKey << m_ui.readKeyWidget1->text();
    readKey << m_ui.readKeyWidget2->text();
    readKey << m_ui.readKeyWidget3->text();
    readKey << m_ui.readKeyWidget4->text();
    const QString writeKey = m_ui.writeKeyWidget->text();
    //try to add device, and display error message if necessary
    Kandas::AddDeviceResult result = m_manager->addDevice(name, readKey, writeKey);
    QString message;
    switch (result)
    {
        case Kandas::DeviceAdded:
            KMessageBox::information(0, i18n("The NDAS driver is now searching for this drive. It will then appear in the drive list."), QString(), "kandas_adddevice_successinfo");
            return; //nothing to do
        case Kandas::DeviceAdditionFailed:
            message = i18n("The drive registration failed for some undetermined reason.");
            break;
        case Kandas::InvalidDeviceKey:
            message = i18n("The given access keys are invalid.");
            break;
        case Kandas::InvalidDeviceName:
            message = i18n("The given drive name is invalid.");
            break;
        case Kandas::DeviceExistsAlready:
            message = i18n("A drive with this name does already exist.");
            break;
    }
    KMessageBox::error(this, message);
}

#include "adddialog.moc"
