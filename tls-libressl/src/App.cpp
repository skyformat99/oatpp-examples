//
//  main.cpp
//  web-starter-project
//
//  Created by Leonid on 2/12/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

//#define OATPP_USE_TARGET
//#define OATPP_TARGET_TEST

//////////////////////////////////
// App

#include "./controller/MyController.hpp"
#include "./AppComponent.hpp"
#include "./Logger.hpp"

//////////////////////////////////
// Test

#ifdef OATPP_TARGET_TEST
#endif

//////////////////////////////////
// std

#include <iostream>
///

#include "oatpp-libressl/client/ConnectionProvider.hpp"
#include "oatpp-libressl/Config.hpp"

#include "oatpp/web/client/HttpRequestExecutor.hpp"
#include "oatpp/network/client/SimpleTCPConnectionProvider.hpp"


void tryLibressl() {
  
  auto config = oatpp::libressl::Config::createShared();
  
  tls_config_insecure_noverifycert(config->getTLSConfig());
  tls_config_insecure_noverifyname(config->getTLSConfig());
  
  auto httpsCP = oatpp::libressl::client::ConnectionProvider::createShared("httpbin.org", 443, config);
  auto httpCP = oatpp::network::client::SimpleTCPConnectionProvider::createShared("httpbin.org", 80);
  auto re = oatpp::web::client::HttpRequestExecutor::createShared(httpsCP);
  
  auto response = re->execute("GET", "/get", nullptr, nullptr);
  
  auto result = response->readBodyToString();
  
  OATPP_LOGD("TLS", "result='%s'", result->c_str());
  
}

/**
 *  run() method.
 *  1) set Environment components.
 *  2) add ApiController's endpoints to router
 *  3) run server
 */
void run() {
  
  AppComponent components; // Create scope Environment components
  
  /* create ApiControllers and add endpoints to router */
  
  auto router = components.httpRouter.getObject();
  
  auto MyController = MyController::createShared();
  MyController->addEndpointsToRouter(router);
  
  /* create server */
  
  oatpp::network::server::Server server(components.serverConnectionProvider.getObject(),
                                        components.serverConnectionHandler.getObject());
  
  OATPP_LOGD("Server", "Running on port %u...", components.serverConnectionProvider.getObject()->getPort());
  
  server.run();
  
}

/**
 *  main
 */
int main(int argc, const char * argv[]) {
  
  oatpp::base::Environment::setLogger(new Logger());
  oatpp::base::Environment::init();
  
#if !defined(OATPP_USE_TARGET) | defined(OATPP_TARGET_APP)
  run();
  //tryLibressl();
#endif
  
#ifdef OATPP_TARGET_TEST
#endif
  
  oatpp::base::Environment::setLogger(nullptr); ///< free Logger
  
  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";
  
  oatpp::base::Environment::destroy();
  
  return 0;
}
