#include <QCoreApplication>

#include <thread>
#include <iostream>

#include "Worker.h"
//-------------------------------------------------------------------------------------------------
int main()
{
  std::cout << "Application has started, for viewing command's list use 'help'" << std::endl;

  Worker worker;
  std::thread thread(&Worker::vWork, &worker);
  thread.detach();

  std::string command = "";
  forever {
    std::cin >> command;
    auto itCommand = cmapCommands.find(command);
    if (itCommand != cmapCommands.end()) {
      worker.vAddCommandToQueue(itCommand->second);
      if (itCommand->second == enSCExit) {
        break;
      }
    }
  }

  return 0;
}
//-------------------------------------------------------------------------------------------------
