#pragma once

#include <vector>
#include <string>

namespace emu
{
  class CommandLine
  {
  public:
    CommandLine(int &argc, char **argv);
    const std::string &getOption(const std::string &option);
    bool optionExists(const std::string &option);

  private:
    std::vector<std::string> tokens;
  };

}
