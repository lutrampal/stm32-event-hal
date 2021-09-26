
#include <device/error_status.hpp>
#include <sstream>

#include "CppUTestExt/MockSupport.h"

namespace hal
{
namespace device
{
class ErrorStatusComparator : public MockNamedValueComparator
{
  public:
    virtual bool isEqual(const void* object1, const void* object2) override
    {
        return reinterpret_cast<const ErrorStatus*>(object1)->get_code()
               == reinterpret_cast<const ErrorStatus*>(object2)->get_code();
    }

    virtual SimpleString valueToString(const void* object) override
    {
        std::ostringstream stream;
        stream << *reinterpret_cast<const ErrorStatus*>(object);
        return StringFrom(stream.str().c_str());
    }
};

}  // namespace device
}  // namespace hal
