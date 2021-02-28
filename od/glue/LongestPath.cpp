#include <od/glue/LongestPath.h>
#include <od/extras/Utils.h>

namespace od
{

  LongestPath::LongestPath()
  {
    // TODO Auto-generated constructor stub
  }

  LongestPath::~LongestPath()
  {
    // TODO Auto-generated destructor stub
  }

  void LongestPath::add(const char *path)
  {
    mPaths.push_back(path);
  }

  std::string LongestPath::calculate()
  {
    return longestPath(mPaths);
  }

} /* namespace od */
