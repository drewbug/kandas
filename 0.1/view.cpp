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

#include "view.h"
#include "devicemodel.h"
#include "manager.h"
#include "slotmodel.h"

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QListView>
#include <QPainter>
#include <QTextDocument>
#include <QUrl>
#include <QVBoxLayout>

namespace Kandas
{
    namespace Client
    {

        class ViewDelegate : public QAbstractItemDelegate
        {
            public:
                ViewDelegate(QObject *parent = 0) : QAbstractItemDelegate(parent) {}

                virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
                virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
            private:
                QTextDocument *document(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        };

        class ViewPrivate
        {
            public:
                QVBoxLayout m_layout;
                QListView m_deviceList, m_slotList;
                Manager m_manager;
                ViewDelegate m_deviceDelegate, m_slotDelegate;
        };

    }
}

Kandas::Client::View::View(QWidget *parent)
    : QWidget(parent)
    , p(new Kandas::Client::ViewPrivate())
{
    p->m_deviceList.setItemDelegate(&p->m_deviceDelegate);
    p->m_deviceList.setModel(p->m_manager.deviceModel());
    p->m_deviceList.setSelectionMode(QAbstractItemView::SingleSelection);
    p->m_slotList.setItemDelegate(&p->m_slotDelegate);
    p->m_slotList.setModel(p->m_manager.slotModel());
    p->m_slotList.setSelectionMode(QAbstractItemView::SingleSelection);

    p->m_layout.addWidget(&p->m_deviceList);
    p->m_layout.addWidget(&p->m_slotList);
    setLayout(&p->m_layout);

    connect(p->m_deviceList.selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), &p->m_manager, SLOT(selectedDeviceChanged(const QModelIndex &)));
    connect(p->m_manager.deviceModel(), SIGNAL(modelReset()), this, SLOT(resetDeviceSelection()));

    connect(&p->m_manager, SIGNAL(initializationComplete(const QString &)), this, SIGNAL(initializationComplete(const QString &)));
}

Kandas::Client::View::~View()
{
    delete p;
}

void Kandas::Client::View::resetDeviceSelection()
{
    QModelIndex selection = p->m_manager.deviceModel()->index(0, 0);
    p->m_deviceList.selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
    p->m_manager.selectedDeviceChanged(selection);
}

void Kandas::Client::View::connectDeviceRead()
{
    if (!p->m_deviceList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_deviceList.selectionModel()->currentIndex();
    QString device = p->m_manager.deviceModel()->data(selected, Kandas::Client::ItemIdentifierRole).toString();
    p->m_manager.connectDevice(device, true);
}

void Kandas::Client::View::connectDeviceWrite()
{
    if (!p->m_deviceList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_deviceList.selectionModel()->currentIndex();
    QString device = p->m_manager.deviceModel()->data(selected, Kandas::Client::ItemIdentifierRole).toString();
    p->m_manager.connectDevice(device, false);
}

void Kandas::Client::View::connectSlotRead()
{
    if (!p->m_slotList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_slotList.selectionModel()->currentIndex();
    int slot = p->m_manager.slotModel()->data(selected, Kandas::Client::ItemIdentifierRole).toInt();
    p->m_manager.connectSlot(slot, true);
}

void Kandas::Client::View::connectSlotWrite()
{
    if (!p->m_slotList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_slotList.selectionModel()->currentIndex();
    int slot = p->m_manager.slotModel()->data(selected, Kandas::Client::ItemIdentifierRole).toInt();
    p->m_manager.connectSlot(slot, false);
}

void Kandas::Client::View::disconnectDevice()
{
    if (!p->m_deviceList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_deviceList.selectionModel()->currentIndex();
    QString device = p->m_manager.deviceModel()->data(selected, Kandas::Client::ItemIdentifierRole).toString();
    p->m_manager.disconnectDevice(device);
}

void Kandas::Client::View::disconnectSlot()
{
    if (!p->m_slotList.selectionModel()->hasSelection())
        return;
    QModelIndex selected = p->m_slotList.selectionModel()->currentIndex();
    int slot = p->m_manager.slotModel()->data(selected, Kandas::Client::ItemIdentifierRole).toInt();
    p->m_manager.disconnectSlot(slot);
}

void Kandas::Client::ViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    QTextDocument *d = document(option, index);
    if (!d)
        return;

    //find color group for this style
    QPalette::ColorGroup group = (option.state & QStyle::State_Enabled) ? ((option.state & QStyle::State_Active) ? QPalette::Normal : QPalette::Inactive) : QPalette::Disabled;
    //save pen (has to be restored later)
    QPen oldPen = painter->pen();
    //find new pen
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.brush(group, QPalette::Highlight));
        painter->setPen(option.palette.color(group, QPalette::HighlightedText));
    }
    else
        painter->setPen(option.palette.color(group, QPalette::Text));
    //render document
    painter->setRenderHint(QPainter::Antialiasing);
    painter->save();
    painter->translate(option.rect.topLeft());
    d->drawContents(painter);
    painter->restore();
    painter->setPen(oldPen);
    //draw focus
    if (option.state & QStyle::State_HasFocus)
    {
        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(option);
        o.rect = option.rect;
        o.state |= QStyle::State_KeyboardFocusChange;
        o.backgroundColor = option.palette.color(group, (option.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Background);
        QApplication::style()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter);
    }
    delete d;
}

QSize Kandas::Client::ViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return QSize();
    QTextDocument *d = document(option, index);
    if (!d)
        return QSize();
    const QSize ret = d->documentLayout()->documentSize().toSize();
    delete d;
    return ret;
}

QTextDocument *Kandas::Client::ViewDelegate::document(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //gather information
    if (!index.isValid())
        return 0;
    const QString displayText = index.model()->data(index, Qt::DisplayRole).toString();
    const QVariant decoration = index.model()->data(index, Qt::DecorationRole);
    const QString statusText = index.model()->data(index, Kandas::Client::ConnectionStatusRole).toString();
    //create document
    QTextDocument *document = new QTextDocument(0);
    static const QSize decorationSize(48, 48);
    if (decoration.isValid() && decoration.type() == QVariant::Icon)
        document->addResource(QTextDocument::ImageResource, QUrl(QLatin1String("deco_icon")), qvariant_cast<QIcon>(decoration).pixmap(decorationSize));
    //get text color
    QPalette::ColorGroup group = (option.state & QStyle::State_Enabled) ? ((option.state & QStyle::State_Active) ? QPalette::Normal : QPalette::Inactive) : QPalette::Disabled;
    QColor textColor = option.palette.color(group, (option.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text);
    //build content
    static const QString content = QString::fromLatin1(
        "<html style=\"color:%1\"><body>"
        "<table><tr><td><img src=\"deco_icon\"></td><td style=\"vertical-align:middle\">"
        "<table><tr><td>&nbsp;<b>%2</b></td></tr><tr><td>&nbsp;%3</td></tr></table>"
        "</td></tr></table></body></html>"
    );
    document->setHtml(content.arg(textColor.name().toUpper()).arg(displayText).arg(statusText));
    return document;
}

#include "view.moc"
