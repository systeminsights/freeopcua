/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa client command line options parser.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

#include "server_options.h"

#include <boost/foreach.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <iostream>

namespace
{
  namespace po = boost::program_options;
  using boost::property_tree::ptree;
  using namespace OpcUa;

  const char* OPTION_HELP = "help";
  const char* OPTION_CONFIG = "config";

  std::string GetConfigOptionValue(const po::variables_map& vm)
  {
    if (vm.count(OPTION_CONFIG))
    {
      return vm[OPTION_CONFIG].as<std::string>();
    }
    return "/etc/opcua/server.config";
  }

  struct Configuration
  {
    OpcUa::Server::ModulesConfiguration Modules;
  };


  Common::ParametersGroup GetGroup(const std::string& name, const ptree& groupTree)
  {
    Common::ParametersGroup group(name);

    if (groupTree.empty())
    {
      return group;
    }

    BOOST_FOREACH(const ptree::value_type& child, groupTree)
    {
      if (child.second.empty())
      {
        group.Parameters.push_back(Common::Parameter(child.first, child.second.data()));
        continue;
      }
      group.Groups.push_back(GetGroup(child.first, child.second));
    }

    return group;
  }

  void AddParameter(Common::AddonParameters& params, const std::string& name, const ptree& tree)
  {
    if (tree.empty())
    {
      params.Parameters.push_back(Common::Parameter(name, tree.data()));
      return;
    }
    params.Groups.push_back(GetGroup(name, tree));
  }


  Configuration ParseConfigurationFile(const std::string& configPath)
  {
    ptree pt;
    read_xml(configPath, pt);
    Configuration configuration;
    const boost::optional<ptree&> modules = pt.get_child_optional("config.modules");
    if (modules)
    {
      BOOST_FOREACH(const ptree::value_type& module, modules.get())
      {
        if (module.first != "module")
        {
          continue;
        }

        Server::ModuleConfig moduleConfig;
        moduleConfig.ID = module.second.get<std::string>("id");
        moduleConfig.Path = module.second.get<std::string>("path");
        if (boost::optional<const ptree&> dependsOn = module.second.get_child_optional("depends_on"))
        {
          BOOST_FOREACH(const ptree::value_type& depend, dependsOn.get())
          {
            if (depend.first != "id")
            {
              continue;
            }
            moduleConfig.DependsOn.push_back(depend.second.data());
          }
        }
        
        if (boost::optional<const ptree&> parameters = module.second.get_child_optional("parameters"))
        {
          BOOST_FOREACH(const ptree::value_type& parameter, parameters.get())
          {
            AddParameter(moduleConfig.Parameters, parameter.first, parameter.second);
          }
        }

        configuration.Modules.push_back(moduleConfig);
      }
    }
    return configuration;
  }

}


namespace OpcUa
{
  namespace Server
  {

    CommandLine::CommandLine(int argc, char** argv)
    {
      // Declare the supported options.
      po::options_description desc("Parameters");
      desc.add_options()
        (OPTION_HELP, "produce help message")
        (OPTION_CONFIG, po::value<std::string>(), "Path to config file.")
        ;

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);    

      if (vm.count(OPTION_HELP)) 
      {
        desc.print(std::cout);
        return;
      }

      std::string configFile = GetConfigOptionValue(vm);
      Configuration configuration = ParseConfigurationFile(configFile);
      Modules = configuration.Modules;
    }

  } // namespace Server
} // namespace OpcUa
