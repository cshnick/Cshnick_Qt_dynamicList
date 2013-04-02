#ifndef ICOMMONINTERFACE_H
#define ICOMMONINTERFACE_H

#include <QtPlugin>
#include "PluginManager.h"

namespace Plugins {

class ICommonInterface
{
public:
    ICommonInterface()
        : mLoader(0) {}
    PInfoHandler pluginMeta() const {return mHandler;}
    void setPluginMeta(const PInfoHandler &pHandler){mHandler = pHandler;}
    void setLoader(QPluginLoader *loader) {mLoader = loader;}
    QPluginLoader *loader() {return mLoader;}

private:
    PInfoHandler mHandler;
    QPluginLoader *mLoader;
};

} // namespace Plugins

Q_DECLARE_INTERFACE(Plugins::ICommonInterface, "org.opensankore.ICommonInterface")

#endif // ICOMMONINTERFACE_H
