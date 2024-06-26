// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dsglobal.h"
#include "constants.h"
#include "dockpanel.h"
#include "dockdbusproxy.h"

#include "private/dsqmlglobal_p.h"

#include <QObject>
DS_BEGIN_NAMESPACE
    namespace dock {
    DockDBusProxy::DockDBusProxy(DockPanel* parent)
        : QObject(parent)
        , m_oldDockApplet(nullptr)
        , m_clipboardApplet(nullptr)
        , m_searchApplet(nullptr)
    {
        registerPluginInfoMetaType();

        connect(parent, &DockPanel::geometryChanged, this, &DockDBusProxy::FrontendWindowRectChanged);
        connect(parent, &DockPanel::positionChanged, this, &DockDBusProxy::PositionChanged);
        connect(parent, &DockPanel::hideModeChanged, this, &DockDBusProxy::hideModeChanged);

        // Communicate with the other module
        auto getOtherApplet = [ = ] {
            QList<DApplet *> list = DQmlGlobal::instance()->appletList("org.deepin.ds.dock.tray");
            if (!list.isEmpty()) m_oldDockApplet = list.first();

            list = DQmlGlobal::instance()->appletList("org.deepin.ds.dock.clipboarditem");
            if (!list.isEmpty()) m_clipboardApplet = list.first();

            list = DQmlGlobal::instance()->appletList("org.deepin.ds.dock.searchitem");
            if (!list.isEmpty()) m_searchApplet = list.first();

            return m_oldDockApplet && m_clipboardApplet && m_searchApplet;
        };

        // TODO: DQmlGlobal maybe missing a  signal which named `appletListChanged`?
        QTimer *timer = new QTimer;
        timer->setInterval(1000);
        connect(timer, &QTimer::timeout, this, [ = ] {
            if (getOtherApplet()) {
                timer->stop();
                timer->deleteLater();
            }
        });
        timer->start();
    }

    DockPanel* DockDBusProxy::parent() const
    {
        return static_cast<DockPanel*>(QObject::parent());
    }

    QRect DockDBusProxy::geometry()
    {
        return parent()->window() ? parent()->window()->geometry() : QRect();
    }

    QRect DockDBusProxy::frontendWindowRect()
    {
        return parent()->frontendWindowRect();
    }

    Position DockDBusProxy::position()
    {
        return parent()->position();
    }

    void DockDBusProxy::setPosition(Position position)
    {
        parent()->setPosition(position);
    }

    HideMode DockDBusProxy::hideMode()
    {
        return parent()->hideMode();
    }

    void DockDBusProxy::setHideMode(HideMode mode)
    {
        parent()->setHideMode(mode);
    }

    HideState DockDBusProxy::hideState()
    {
        return parent()->hideState();
    }

    QStringList DockDBusProxy::GetLoadedPlugins()
    {
        // TODO: implement this function
        return QStringList();
    }

    DockItemInfos DockDBusProxy::plugins()
    {
        if (m_oldDockApplet) {
            DockItemInfos iteminfos;
            QMetaObject::invokeMethod(m_oldDockApplet, "plugins", Qt::DirectConnection, qReturnArg(iteminfos));

            if (m_clipboardApplet) {
                DockItemInfo info;
                if (QMetaObject::invokeMethod(m_clipboardApplet, "dockItemInfo", Qt::DirectConnection, qReturnArg(info))) {
                    iteminfos.append(info);
                }
            }

            if (m_searchApplet) {
                DockItemInfo info;
                if (QMetaObject::invokeMethod(m_searchApplet, "dockItemInfo", Qt::DirectConnection, qReturnArg(info)))
                    iteminfos.append(info);
            }

            return iteminfos;
        }
        return DockItemInfos();
    }

    void DockDBusProxy::ReloadPlugins()
    {
        parent()->ReloadPlugins();
    }

    void DockDBusProxy::callShow()
    {
        parent()->callShow();
    }

    void DockDBusProxy::setItemOnDock(const QString &settingKey, const QString &itemKey, bool visible)
    {
        if (itemKey == "clipboard" && m_clipboardApplet) {
            QMetaObject::invokeMethod(m_clipboardApplet, "setVisible", Qt::QueuedConnection, visible);
        } else if(itemKey == "search" && m_searchApplet) {
            QMetaObject::invokeMethod(m_searchApplet, "setVisible", Qt::QueuedConnection, visible);
        } else if (m_oldDockApplet) {
            QMetaObject::invokeMethod(m_oldDockApplet, "setItemOnDock", Qt::QueuedConnection, settingKey, itemKey, visible);
        }
    }

    void DockDBusProxy::setPluginVisible(const QString &pluginName, bool visible)
    {
        // TODO: implement this function
        Q_UNUSED(pluginName)
        Q_UNUSED(visible)
    }

    bool DockDBusProxy::getPluginVisible(const QString &pluginName)
    {
        // TODO: implement this function
        Q_UNUSED(pluginName)
        return true;
    }

    QString DockDBusProxy::getPluginKey(const QString &pluginName)
    {
        // TODO: implement this function
        Q_UNUSED(pluginName)
        return QString();
    }

    void DockDBusProxy::resizeDock(int offset, bool dragging)
    {
        // TODO: implement this function
        Q_UNUSED(offset)
        Q_UNUSED(dragging)
    }

}

DS_END_NAMESPACE
