/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa binary protocol connection processor.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opcua_server_internal_opc_tcp_processor_h
#define opcua_server_internal_opc_tcp_processor_h

#include <opc/ua/computer.h>
#include <opc/ua/server/server.h>

namespace OpcUa
{
  namespace Internal
  {

    std::unique_ptr<OpcUa::Server::IncomingConnectionProcessor> CreateOpcTcpProcessor(std::shared_ptr<OpcUa::Remote::Computer> computer);

  }
}

#endif // opcua_server_internal_opc_tcp_processor_h
