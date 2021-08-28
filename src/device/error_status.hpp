
/*******************************************************************************
 * The error status object is used in event callbacks to indicate wether or not
 * an operation succeeded
 ******************************************************************************/

#ifndef _HAL_DEVICE_ERROR_STATUS_HPP
#define _HAL_DEVICE_ERROR_STATUS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "error_code.hpp"

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class ErrorStatus
{
  public:
    ErrorStatus(ErrorCode code = ErrorCode::Success);

    ErrorCode get_code() const;

    /* @brief boolean conversion operator.
     * @details
     * Returns true if error code is not equal 0 i.e. any error will return
     * true, success value will return false. */
    operator bool() const;

    /** @brief Same as !(static_cast<bool>(*this)). */
    bool operator!() const;

  private:
    ErrorCode code;
};


}  // namespace device
}  // namespace hal

#endif
