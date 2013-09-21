/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "server.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>

#include <stdexcept>

namespace
{

  using namespace OpcUa;

  class OpcUaServer : public OpcUa::Application
  {
  public:
    void Start(const std::vector<Common::AddonConfiguration>& addonConfigurations)
    {
      if (Addons.get())
      {
        throw std::logic_error("Application already started.");
      }

      Addons = Common::CreateAddonsManager();
      for (const Common::AddonConfiguration& config : addonConfigurations)
      {
        Addons->Register(config);
      }
      Addons->Start();
    }

    virtual Common::AddonsManager& GetAddonsManager()
    {
      if (!Addons.get())
      {
        throw std::logic_error("Cannot return addons manager. Application wasn't started.");
      }
      return *Addons;
    }

    virtual void Stop()
    {
      Addons->Stop();
      Addons.reset();
    }

  private:
    Common::AddonsManager::UniquePtr Addons;
  };

}

OpcUa::Application::UniquePtr OpcUa::CreateApplication()
{
  return OpcUa::Application::UniquePtr(new OpcUaServer());
}