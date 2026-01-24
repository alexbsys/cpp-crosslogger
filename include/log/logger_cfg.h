
#ifndef LOGGER_CFG_HEADER
#define LOGGER_CFG_HEADER

#include <string>
#include <list>

namespace logging {
namespace cfg {
  typedef std::pair<std::string, std::string> KeyValueType;
  typedef std::list<KeyValueType> KeyValueTypeList;
}//namespace cfg
}//logging

#endif /*LOGGER_CFG_HEADER*/
