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

#include "removedialog.h"
#include "manager.h"
#include "ndasdevice.h"
#include "ndasmodel.h"

#include <QListView>
#include <KLocalizedString>
#include <KStandardGuiItem>

Kandas::Client::RemoveDialog::RemoveDialog(Kandas::Client::Manager *manager)
    : m_manager(manager)
    , m_view(new QListView(this))
{
    //setup dialog
    setMainWidget(m_view);
    setButtons(KDialog::User1 | KDialog::Cancel);
    setButtonGuiItem(KDialog::User1, KStandardGuiItem::remove());
    setCaption(i18n("Remove a drive"));
    connect(this, SIGNAL(user1Clicked()), this, SLOT(handleOkClicked()));
    //setup view
    m_view->setModel(m_manager->model());
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setViewMode(QListView::IconMode);
    m_view->setSpacing(6);
    connect(m_view->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(handleInput()));
}

void Kandas::Client::RemoveDialog::showDialog()
{
    if (isVisible())
        return;
    m_view->selectionModel()->clear();
    m_view->setFocus(Qt::OtherFocusReason);
    enableButton(KDialog::User1, false);
    show();
}

void Kandas::Client::RemoveDialog::handleInput()
{
    enableButton(KDialog::User1, !m_view->selectionModel()->selectedIndexes().isEmpty());
}

void Kandas::Client::RemoveDialog::handleOkClicked()
{
    hide();
    QModelIndexList selectedIndexes = m_view->selectionModel()->selectedIndexes();
    foreach (const QModelIndex &index, selectedIndexes)
    {
        Kandas::Client::NdasDevice *device = ndasdata_cast<Kandas::Client::NdasDevice *>(index.internalPointer());
        if (!device)
            continue;
        m_manager->removeDevice(device->name());
    }
}

#include "removedialog.moc"
