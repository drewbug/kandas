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

#include "viewdelegate.h"
#include "iconwidget.h"
#include "ndasmodel.h"

#include <QAbstractItemView>
#include <QGridLayout>
#include <QTreeView> //DEBUG

Kandas::Client::ViewDelegate::ViewDelegate(QAbstractItemView *view, QObject *parent)
    : KWidgetItemDelegate(view, parent)
{
}

QList<QWidget *> Kandas::Client::ViewDelegate::createItemWidgets() const
{
    //container widget
    QWidget *container = new QWidget;
    QGridLayout *layout = new QGridLayout;
    container->setLayout(layout);
    //contained widgets
    static const QSize bigIconSize(48, 48);
    static const QSize smallIconSize(32, 32);
    Kandas::Client::IconWidget* iconWidget = new Kandas::Client::IconWidget(bigIconSize);
    iconWidget->setEnabled(false);
    Kandas::Client::IconWidget* buttonWidget = new Kandas::Client::IconWidget(smallIconSize);
    QLabel* headlineWidget = new QLabel;
    QLabel* sublineWidget = new QLabel;
    QFont headlineFont = headlineWidget->font();
    headlineFont.setBold(true);
    headlineWidget->setFont(headlineFont);
    //build layout
    layout->addWidget(iconWidget, 0, 0, 2, 1);
    layout->addWidget(headlineWidget, 0, 1, Qt::AlignLeft | Qt::AlignBottom);
    layout->addWidget(sublineWidget, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(buttonWidget, 0, 2, 2, 1);
    return QList<QWidget *>() << container;
}

void Kandas::Client::ViewDelegate::updateItemWidgets(const QList<QWidget *> widgets, const QStyleOptionViewItem &option, const QPersistentModelIndex &index) const
{
    //retrieve data
    const QString text = index.data(Qt::DisplayRole).toString();
    const QVariant decoration = index.data(Qt::DecorationRole);
    const QString secondText = index.data(Kandas::Client::SecondDisplayRole).toString();
    const QVariant secondDecoration = index.data(Kandas::Client::SecondDecorationRole);
    const QString action = index.data(Kandas::Client::ActionRole).toString();
    const QString actionDescription = index.data(Kandas::Client::ActionDescriptionRole).toString();
    //retrieve widgets
    QWidget* container = widgets[0];
    QGridLayout* layout = qobject_cast<QGridLayout*>(container->layout());
    QLabel* headlineWidget = qobject_cast<QLabel*>(layout->itemAtPosition(0, 1)->widget());
    QLabel* sublineWidget = qobject_cast<QLabel*>(layout->itemAtPosition(1, 1)->widget());
    Kandas::Client::IconWidget* iconWidget = qobject_cast<Kandas::Client::IconWidget*>(layout->itemAtPosition(0, 0)->widget());
    Kandas::Client::IconWidget* buttonWidget = qobject_cast<Kandas::Client::IconWidget*>(layout->itemAtPosition(0, 2)->widget());
    //update icon
    headlineWidget->setText(text);
    sublineWidget->setText(secondText);
    iconWidget->setIcon(qvariant_cast<QIcon>(decoration));
    buttonWidget->setIcon(qvariant_cast<QIcon>(secondDecoration));
    buttonWidget->setAction(action);
    buttonWidget->setToolTip(actionDescription);
    if (!buttonWidget->hasIndex())
    {
        buttonWidget->setIndex(index);
        connect(buttonWidget, SIGNAL(triggered(const QPersistentModelIndex &, const QString &)), this, SIGNAL(actionTriggered(const QPersistentModelIndex &, const QString &)));
    }
    //update size of layout and text color
    container->setGeometry(QRect(QPoint(0, 0), option.rect.size()));
    //update text color in palette
    QPalette basePalette = iconWidget->palette();
    if (option.state & QStyle::State_Selected)
        basePalette.setBrush(QPalette::Text, option.palette.highlightedText());
    headlineWidget->setPalette(basePalette);
    sublineWidget->setPalette(basePalette);
    //DEBUG
    QTreeView* tv = qobject_cast<QTreeView*>(itemView());
    if (tv)
        tv->expand(index);
}

void Kandas::Client::ViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //paint background for selected or hovered item
    QStyleOptionViewItemV4 opt = option;
    itemView()->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, 0);
    //paint widgets - the below method call is deprecated in KDE 4.2, but necessary in KDE 4.1 which we also want to support
    KWidgetItemDelegate::paintWidgets(painter, option, index);
}

QSize Kandas::Client::ViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    //only the height of the size hint is interesting
    static const int topMargin = itemView()->style()->pixelMetric(QStyle::PM_LayoutTopMargin);
    static const int bottomMargin = itemView()->style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
    static const int iconSize = 48;
    static const int verticalSizeHint = iconSize + topMargin + bottomMargin;
    return QSize(itemView()->viewport()->width(), verticalSizeHint);
}
