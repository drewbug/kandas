/***************************************************************************
 *   Copyright (C) 2008 Stefan Majewsky <majewsky@gmx.net>
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

#include "slotmanager.h"
#include <kandasd/definitions.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <KLed>
#include <KLocalizedString>

namespace Kandas
{
    namespace Client
    {

        class SlotManagerPrivate
        {
            public:
                SlotManagerPrivate(int slot, Kandas::Client::SlotManager *widget);

                int m_slot, m_state;
                QPushButton m_statusChanger;
                QLabel m_statusText;
                KLed m_statusLight;
                QHBoxLayout m_layout;
        };

    }
}

Kandas::Client::SlotManagerPrivate::SlotManagerPrivate(int slot, Kandas::Client::SlotManager *widget)
    : m_slot(slot)
    , m_state(Kandas::Undetermined)
    , m_statusChanger(widget)
    , m_statusText(widget)
    , m_statusLight(widget)
{
    m_layout.addWidget(&m_statusLight);
    m_layout.setStretchFactor(&m_statusLight, 0);
    m_layout.setAlignment(&m_statusLight, Qt::AlignCenter);
    m_layout.addWidget(&m_statusText);
    m_layout.setStretchFactor(&m_statusText, 1);
    m_layout.addWidget(&m_statusChanger);
    m_layout.setStretchFactor(&m_statusChanger, 0);
    widget->setLayout(&m_layout);
    QObject::connect(&m_statusChanger, SIGNAL(clicked()), widget, SLOT(connectionChangeRequested()));
}

Kandas::Client::SlotManager::SlotManager(int slot, int state, QWidget *parent)
    : QWidget(parent)
    , p(new Kandas::Client::SlotManagerPrivate(slot, this))
{
    update(state);
}

Kandas::Client::SlotManager::~SlotManager()
{
    delete p;
}

void Kandas::Client::SlotManager::connectionChangeRequested()
{
    if (p->m_state == Kandas::Disconnected)
        emit connectRequested(p->m_slot);
    else if (p->m_state == Kandas::Connected)
        emit disconnectRequested(p->m_slot);
}

void Kandas::Client::SlotManager::update(int state)
{
    p->m_state = state;
    switch (state)
    {
        case Kandas::Connected:
            p->m_statusChanger.setVisible(true);
            p->m_statusChanger.setText(i18n("Disconnect"));
            p->m_statusLight.setColor(QColor(0, 255, 0));
            p->m_statusLight.setState(KLed::On);
            p->m_statusText.setText(i18n("Connected"));
            break;
        case Kandas::Connecting:
            p->m_statusChanger.setVisible(false);
            p->m_statusLight.setColor(QColor(255, 255, 0));
            p->m_statusLight.setState(KLed::On);
            p->m_statusText.setText(i18n("Connecting"));
            break;
        case Kandas::Disconnected:
            p->m_statusChanger.setVisible(true);
            p->m_statusChanger.setText(i18n("Connect"));
            p->m_statusLight.setColor(QColor(0, 255, 0));
            p->m_statusLight.setState(KLed::Off);
            p->m_statusText.setText(i18n("Disconnected"));
            break;
        case Kandas::Disconnecting:
            p->m_statusChanger.setVisible(false);
            p->m_statusLight.setColor(QColor(255, 255, 0));
            p->m_statusLight.setState(KLed::On);
            p->m_statusText.setText(i18n("Disconnecting"));
            break;
        default: //case Kandas::Undetermined:
            p->m_statusChanger.setVisible(false);
            p->m_statusLight.setColor(QColor(0, 255, 0));
            p->m_statusLight.setState(KLed::Off);
            p->m_statusText.setText(i18n("Unavailable"));
            break;
    }
}

#include "slotmanager.moc"
