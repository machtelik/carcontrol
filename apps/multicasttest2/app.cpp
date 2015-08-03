#include "app.h"

#include <libccm/message.h>

#include <iostream>

App::App ( int argc, char** argv ) : 
    Component ( argc, argv ) 
{
}

bool App::begin()
{
    return true;
}

bool App::loop()
{
  
  std::cout << "Sending" << std::endl;
    
  ccm:: Message *message = getMessage();
  std::string str ("Message 2");
  std::size_t length = str.copy(message->getData(), message->getMaxDataSize(), 0);
  message->getData()[length]='\0';
  message->setDataSize(length);
  sendMessage(message);
  
  return true;
}

bool App::messageReceived ( const ccm::Message* message )
{  
      std::cout << message->getData() << std::endl;
      return true;
}
