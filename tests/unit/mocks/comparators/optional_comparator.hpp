
#include "CppUTestExt/MockSupport.h"

#include <device/error_status.hpp>
#include <optional>
#include <sstream>

namespace hal
{
namespace device
{
template<typename T>
class OptionalComparator : public MockNamedValueComparator
{
  public:
    virtual bool isEqual(const void* object1, const void* object2) override
    {
        return *reinterpret_cast<const std::optional<T>*>(object1)
               == *reinterpret_cast<const T*>(object2);
    }

    virtual SimpleString valueToString(const void* object) override
    {
        std::ostringstream stream;
        const std::optional<T>* casted_ptr =
            reinterpret_cast<const std::optional<T>*>(object);
        if (casted_ptr->has_value()) {
            stream << "0x" << std::hex << (unsigned int)casted_ptr->value();
        } else {
            stream << "empty optional " << typeid(T).name();
        }

        return StringFrom(stream.str().c_str());
    }
};

}  // namespace device
}  // namespace hal
