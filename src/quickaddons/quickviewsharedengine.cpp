/*
   This file is part of the KDE libraries

   Copyright (C) 2015 Marco Martin <mart@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

*/

#include "quickviewsharedengine.h"

#include <QTimer>
#include <QDebug>
#include <QUrl>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlExpression>
#include <QQmlProperty>

#include <klocalizedstring.h>
#include <kdeclarative/qmlobjectsharedengine.h>

#include <KPackage/Package>
#include <KPackage/PackageLoader>

namespace KQuickAddons {

class QuickViewSharedEnginePrivate
{
public:
    QuickViewSharedEnginePrivate(QuickViewSharedEngine *module)
        : q(module)
    {
        qmlObject = new KDeclarative::QmlObjectSharedEngine(q);
        QObject::connect(qmlObject, &KDeclarative::QmlObject::statusChanged,
                         q, &QuickViewSharedEngine::statusChanged);
        QObject::connect(qmlObject, SIGNAL(finished()),
                         q, SLOT(executionFinished()));
    }

    void executionFinished();
    void syncWidth();
    void syncHeight();


    QuickViewSharedEngine *q;
    KDeclarative::QmlObjectSharedEngine *qmlObject;
    QuickViewSharedEngine::ResizeMode resizeMode;
    QSize initialSize;
};

void QuickViewSharedEnginePrivate::executionFinished()
{
    if (!qmlObject->rootObject()) {
        return;
    }

    QQuickItem *item = qobject_cast<QQuickItem *>(qmlObject->rootObject());

    if (!item) {
        return;
    }

    item->setParentItem(q->contentItem());

    if (resizeMode == QuickViewSharedEngine::SizeRootObjectToView) {
        QQmlExpression expr(QtQml::qmlContext(item), item, "parent");
        QQmlProperty prop(item, "anchors.fill");
        prop.write(expr.evaluate());

        QObject::disconnect(item, SIGNAL(widthChanged()),
                            q, SLOT(syncWidth()));
        QObject::disconnect(item, SIGNAL(heightChanged()),
                            q, SLOT(syncHeight()));

    } else {
        QQmlExpression expr(QtQml::qmlContext(item), item, "undefined");
        QQmlProperty prop(item, "anchors.fill");
        prop.write(expr.evaluate());

        QObject::connect(item, SIGNAL(widthChanged()),
                         q, SLOT(syncWidth()));
        QObject::connect(item, SIGNAL(heightChanged()),
                         q, SLOT(syncHeight()));
    }
}

void QuickViewSharedEnginePrivate::syncWidth()
{
    QQuickItem *item = qobject_cast<QQuickItem *>(qmlObject->rootObject());

    if (!item) {
        return;
    }

    q->setWidth(item->width());
}

void QuickViewSharedEnginePrivate::syncHeight()
{
    QQuickItem *item = qobject_cast<QQuickItem *>(qmlObject->rootObject());

    if (!item) {
        return;
    }

    q->setHeight(item->height());
}



QuickViewSharedEngine::QuickViewSharedEngine(QWindow *parent)
    : QQuickWindow(parent),
      d(new QuickViewSharedEnginePrivate(this))
{
}

QuickViewSharedEngine::~QuickViewSharedEngine()
{
}


QQmlEngine *QuickViewSharedEngine::engine() const
{
    return d->qmlObject->engine();
}

QList<QQmlError> QuickViewSharedEngine::errors() const
{
    QList<QQmlError> errs;

    if (d->qmlObject->mainComponent()) {
        errs = d->qmlObject->mainComponent()->errors();
    }

    return errs;
}

QSize QuickViewSharedEngine::sizeHint() const
{
    return initialSize();
}

QSize QuickViewSharedEngine::initialSize() const
{
    return QSize();
}

QuickViewSharedEngine::ResizeMode QuickViewSharedEngine::resizeMode() const
{
    return d->resizeMode;
}

QQmlContext *QuickViewSharedEngine::rootContext() const
{
    return d->qmlObject->rootContext();
}

QQuickItem *QuickViewSharedEngine::rootObject() const
{
     return qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
}

void QuickViewSharedEngine::setResizeMode(ResizeMode mode)
{
    if (d->resizeMode == mode) {
        return;
    }

    d->resizeMode = mode;

    emit resizeModeChanged(mode);

    QQuickItem *item = qobject_cast<QQuickItem *>(d->qmlObject->rootObject());
    if (!item) {
        return;
    }

    if (d->resizeMode == QuickViewSharedEngine::SizeRootObjectToView) {
        QQmlExpression expr(QtQml::qmlContext(item), item, "parent");
        QQmlProperty prop(item, "anchors.fill");
        prop.write(expr.evaluate());

        QObject::disconnect(item, SIGNAL(widthChanged()),
                            this, SLOT(syncWidth()));
        QObject::disconnect(item, SIGNAL(heightChanged()),
                            this, SLOT(syncHeight()));

    } else {
        QQmlExpression expr(QtQml::qmlContext(item), item, "undefined");
        QQmlProperty prop(item, "anchors.fill");
        prop.write(expr.evaluate());

        QObject::connect(item, SIGNAL(widthChanged()),
                         this, SLOT(syncWidth()));
        QObject::connect(item, SIGNAL(heightChanged()),
                         this, SLOT(syncHeight()));
    }
}

void QuickViewSharedEngine::setSource(const QUrl &url)
{
    if (d->qmlObject->source() == url) {
        return;
    }

    d->qmlObject->setSource(url);
    emit sourceChanged(url);
}

QUrl QuickViewSharedEngine::source() const
{
    return d->qmlObject->source();
}

QQmlComponent::Status QuickViewSharedEngine::status() const
{
    if (!d->qmlObject->mainComponent()) {
        return QQmlComponent::Null;
    }

    return QQmlComponent::Status(d->qmlObject->status());
}

}

#include "moc_quickviewsharedengine.cpp"

