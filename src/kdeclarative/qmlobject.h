/*
 *   Copyright 2013 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef QMLOBJECT_H
#define QMLOBJECT_H

#include <QObject>

#include <QWindow>
#include <QAnimationDriver>
#include <QGuiApplication>
#include <QScreen>

#include <kdeclarative/kdeclarative_export.h>

class QQmlEngine;
class QQmlComponent;
class QQmlContext;

namespace KDeclarative {

class QmlObjectPrivate;

/**
 * @class QmlObject plasma/declarativewidget.h <Plasma/QmlObject>
 *
 * @author Marco Martin <mart@kde.org>
 *
 * @short An object that instantiates an entire QML context, with its own declarative engine
 *
 * Plasma::QmlObject provides a class for conveniently use QML based
 * declarative user interfaces inside Plasma widgets.
 * To one QmlObject corresponds one QML file (that can eventually include others)
 * tere will be its own QQmlEngine with a single root object,
 * described in the QML file.
 */
class KDECLARATIVE_EXPORT QmlObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl source READ source WRITE setSource)
    Q_PROPERTY(bool initializationDelayed READ isInitializationDelayed WRITE setInitializationDelayed)
    Q_PROPERTY(QObject *rootObject READ rootObject)

public:

    /**
     * Constructs a new QmlObject
     *
     * @param parent the parent of this object
     */
    explicit QmlObject(QObject *parent = 0);

    /**
     * Constructs a new QmlObject
     *
     * @param engine a QQmlEngine we want to use
     * @param parent the parent of this object
     */
    explicit QmlObject(QQmlEngine *engine, QObject *parent = 0);
    ~QmlObject();

    /**
     * Sets the path of the QML file to parse and execute
     *
     * @param path the absolute path of a QML file
     */
    void setSource(const QUrl &source);

    /**
     * @return the absolute path of the current QML file
     */
    QUrl source() const;

    /**
     * Sets whether the execution of the QML file has to be delayed later in the event loop. It has to be called before setQmlPath().
     * In this case will be possible to assign new objects in the main engine context
     * before the main component gets initialized.
     * So it will be possible to access it immediately from the QML code.
     * The initialization will either be completed automatically asyncronously
     * or explicitly by calling completeInitialization()
     *
     * @param delay if true the initialization of the QML file will be delayed
     *              at the end of the event loop
     */
    void setInitializationDelayed(const bool delay);

    /**
     * @return true if the initialization of the QML file will be delayed
     *              at the end of the event loop
     */
    bool isInitializationDelayed() const;

    /**
     * @return the declarative engine that runs the qml file assigned to this widget.
     */
    QQmlEngine *engine();

    /**
     * @return the root object of the declarative object tree
     */
    QObject *rootObject() const;

    /**
     * @return the main QQmlComponent of the engine
     */
    QQmlComponent *mainComponent() const;

    /**
     * Creates and returns an object based on the provided url to a Qml file
     * with the same QQmlEngine and the same root context as the amin object,
     * that will be the parent of the newly created object
     * @param source url where the QML file is located
     * @param context The QQmlContext in which we will create the object,
     *             if 0 it will use the engine's root context
     * @param initialProperties optional properties that will be set on
     *             the object when created (and before Component.onCompleted
     *             gets emitted
     */
    QObject *createObjectFromSource(const QUrl &source, QQmlContext *context = 0, const QVariantHash &initialProperties = QVariantHash());

    /**
     * Creates and returns an object based on the provided QQmlComponent
     * with the same QQmlEngine and the same root context as the amin object,
     * that will be the parent of the newly created object
     * @param component the component we want to instantiate
     * @param context The QQmlContext in which we will create the object,
     *             if 0 it will use the engine's root context
     * @param initialProperties optional properties that will be set on
     *             the object when created (and before Component.onCompleted
     *             gets emitted
     */
    QObject *createObjectFromComponent(QQmlComponent *component, QQmlContext *context = 0, const QVariantHash &initialProperties = QVariantHash());

public Q_SLOTS:
    /**
     * Finishes the process of initialization.
     * If isInitializationDelayed() is false, calling this will have no effect.
     * @param initialProperties optional properties that will be set on
     *             the object when created (and before Component.onCompleted
     *             gets emitted
     */
    void completeInitialization(const QVariantHash &initialProperties = QVariantHash());

Q_SIGNALS:
    /**
     * Emitted when the parsing and execution of the QML file is terminated
     */
    void finished();

private:
    friend class QmlObjectPrivate;
    QmlObjectPrivate *const d;

    Q_PRIVATE_SLOT(d, void scheduleExecutionEnd())
};

}

#endif // multiple inclusion guard