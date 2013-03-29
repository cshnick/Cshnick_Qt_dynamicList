#ifndef ICOMMONINTERFACE_H
#define ICOMMONINTERFACE_H

#include "QtPlugin"
#include "PluginManager.h"

namespace Plugins {

class ICommonInterface
{
public:
    virtual PInfoHandler pluginMeta() const = 0;
};

} // namespace Plugins

Q_DECLARE_INTERFACE(Plugins::ICommonInterface, "org.opensankore.ICommonInterface")

#endif // ICOMMONINTERFACE_H
