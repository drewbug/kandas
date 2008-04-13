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

#include "window.h"
#include "view.h"

#include <KAction>
#include <KActionCollection>
#include <KActionMenu>
#include <KApplication>
#include <KLocalizedString>
#include <KStatusBar>

Kandas::Client::MainWindow::MainWindow()
    : KXmlGuiWindow()
    , m_view(new Kandas::Client::View(this))
{
    //early GUI initialisation
    setupActions();
    statusBar()->insertPermanentItem(i18n("Connecting to KaNDASd..."), 1, 1);
    statusBar()->setItemAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
    setAutoSaveSettings();
    //central widget
    setCentralWidget(m_view);
    connect(m_view, SIGNAL(initializationComplete(const QString &)), this, SLOT(initializationComplete(const QString &)));
    //late GUI initialisation
    setupGUI();
    setWindowIcon(KIcon("folder-remote"));
}

Kandas::Client::MainWindow::~MainWindow()
{
}

void Kandas::Client::MainWindow::setupActions()
{
    //TODO: States of actions according to selection states.
    //connect device
    KActionMenu *actConnDev = new KActionMenu(KIcon("media-playback-start"), i18n("Connect device"), actionCollection());
    actConnDev->setDelayed(false);
    actConnDev->setStickyMenu(true);
    KAction *actConnDevRead = new KAction(KIcon("media-playback-start"), i18n("Read access"), actConnDev);
    actConnDev->addAction(actConnDevRead);
    KAction *actConnDevWrite = new KAction(KIcon("media-record"), i18n("Write access"), actConnDev);
    actConnDev->addAction(actConnDevWrite);
    actionCollection()->addAction("kandas_device_connect", actConnDev);
    //disconnect device
    KAction *actDiscDev = new KAction(KIcon("media-eject"), i18n("Disconnect device"), actionCollection());
    actionCollection()->addAction("kandas_device_disconnect", actDiscDev);
    //connect slot
    KActionMenu *actConnSlot = new KActionMenu(KIcon("media-playback-start"), i18n("Connect slot"), actionCollection());
    actConnSlot->setDelayed(false);
    actConnSlot->setStickyMenu(true);
    KAction *actConnSlotRead = new KAction(KIcon("media-playback-start"), i18n("Read access"), actConnSlot);
    actConnSlot->addAction(actConnSlotRead);
    KAction *actConnSlotWrite = new KAction(KIcon("media-record"), i18n("Write access"), actConnSlot);
    actConnSlot->addAction(actConnSlotWrite);
    actionCollection()->addAction("kandas_slot_connect", actConnSlot);
    //disconnect slot
    KAction *actDiscSlot = new KAction(KIcon("media-eject"), i18n("Disconnect slot"), actionCollection());
    actionCollection()->addAction("kandas_slot_disconnect", actDiscSlot);
    //meta-object connections
    connect(actConnDevRead, SIGNAL(triggered()), m_view, SLOT(connectDeviceRead()));
    connect(actConnDevWrite, SIGNAL(triggered()), m_view, SLOT(connectDeviceWrite()));
    connect(actDiscDev, SIGNAL(triggered()), m_view, SLOT(disconnectDevice()));
    connect(actConnSlotRead, SIGNAL(triggered()), m_view, SLOT(connectSlotRead()));
    connect(actConnSlotWrite, SIGNAL(triggered()), m_view, SLOT(connectSlotWrite()));
    connect(actDiscSlot, SIGNAL(triggered()), m_view, SLOT(disconnectSlot()));
}

void Kandas::Client::MainWindow::initializationComplete(const QString &daemonVersion)
{
    statusBar()->changeItem(i18n("Connected to KaNDASd %1", daemonVersion), 1);
    show();
}

#include "window.moc"
