/*
   Copyright (c) 2015 Marco Martin <mart@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as Controls
import QtQuick.Templates 2.2 as T2
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.2 as Kirigami

/**
 * Base delegate for KControlmodules based on Grid views of thumbnails
 * Use the onClicked signal handler for managing the main action when
 * the user clicks on the thumbnail
 * @inherits QtQuick.Templates.ItemDelegate
 */
T2.ItemDelegate {
    id: delegate

    /**
     * toolTip: string
     * string for a tooltip for the whole delegate
     */
    property string toolTip

    /**
     * thumbnail: Item
     * the item actually implementing the thumbnail: the visualization is up to the implementation
     */
    property alias thumbnail: thumbnailArea.data

    /**
     * thumbnailAvailable: bool
     * Set it to true when a thumbnail is actually available: when false,
     * a default icon will be shown instead of the actual thumbnail.
     */
    property bool thumbnailAvailable: false

    /**
     * actions: list<Action>
     * A list of extra actions for the thumbnails. They will be shown as
     * icons on the bottom-right corner of the thumbnail on mouse over
     */
    property list<QtObject> actions

    width: GridView.view.cellWidth
    height: GridView.view.cellHeight
    hoverEnabled: true

    Rectangle {
        id: thumbnail
        anchors {
           centerIn: parent
           verticalCenterOffset: -label.height/2
        }
        width: Math.min(delegate.GridView.view.implicitCellWidth, delegate.width - Kirigami.Units.gridUnit)
        height: width / 1.6
        radius: Kirigami.Units.smallSpacing
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.View

        color: {
            if (delegate.GridView.isCurrentItem) {
                return Kirigami.Theme.highlightColor;
            } else if (parent.hovered) {
                return Qt.tint(Kirigami.Theme.backgroundColor, Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.3))
            } else {
                return Kirigami.Theme.backgroundColor
            }
        }
        Behavior on color {
            ColorAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutQuad
            }
        }

        Rectangle {
            id: thumbnailArea
            radius: Kirigami.Units.smallSpacing/2
            anchors {
                fill: parent
                margins: Kirigami.Units.smallSpacing
            }

            color: Kirigami.Theme.backgroundColor
            Kirigami.Icon {
                visible: !delegate.thumbnailAvailable
                anchors.centerIn: parent
                width: Kirigami.Units.iconSizes.large
                height: width
                source: "view-preview"
            }
        }

        Rectangle {
            anchors.fill: parent
            visible: actionsRow.children.length > 0
            opacity: delegate.hovered || (actionsScope.focus) ? 1 : 0
            radius: Kirigami.Units.smallSpacing
            color: Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.4)

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

            Behavior on opacity {
                PropertyAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutQuad
                }
            }

            FocusScope {
                id: actionsScope
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: Kirigami.Units.smallSpacing
                }
                width: actionsRow.width
                height: actionsRow.height
                RowLayout {
                    id: actionsRow
                    
                    Repeater {
                        model: delegate.actions
                        delegate: Controls.ToolButton {
                            Kirigami.Icon {
                                Kirigami.Theme.inherit: false
                                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                                anchors.centerIn: parent
                                width: Kirigami.Units.iconSizes.smallMedium
                                height: width
                                source: modelData.iconName
                            }
                            activeFocusOnTab: focus || delegate.focus
                            onClicked: modelData.trigger()
                            enabled: modelData.enabled
                            visible: modelData.visible
                            //NOTE: there aren't any global settings where to take "official" tooltip timeouts
                            Controls.ToolTip.delay: 1000
                            Controls.ToolTip.timeout: 5000
                            Controls.ToolTip.visible: hovered && modelData.tooltip.length > 0
                            Controls.ToolTip.text: modelData.tooltip
                        }
                    }
                }
            }
        }
        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 2
            radius: 10
            samples: 32
            color: Qt.rgba(0, 0, 0, 0.3)
        }
    }

    Controls.Label {
        id: label
        anchors {
            left: parent.left
            right: parent.right
            top: thumbnail.bottom
            topMargin: Kirigami.Units.smallSpacing
        }
        text: delegate.text
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }
    Controls.ToolTip.delay: 1000
    Controls.ToolTip.timeout: 5000
    Controls.ToolTip.visible: hovered && delegate.toolTip.length > 0
    Controls.ToolTip.text: toolTip
}
