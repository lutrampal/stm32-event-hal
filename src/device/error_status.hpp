
/*******************************************************************************
 * The error status object is used in event callbacks to indicate wether or not
 * an operation succeeded
 ******************************************************************************/

#ifndef _HAL_DEVICE_ERROR_STATUS_HPP
#define _HAL_DEVICE_ERROR_STATUS_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <ostream>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

enum class ErrorCode {
    Success,  // Successful completion of operation
    Aborted,  // The operation was cancelled/aborted
    Failure,  // Failure of operation (unspecified cause)
    NbCodes
};

class ErrorStatus
{
  public:
    ErrorStatus(ErrorCode code);

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

    friend std::ostream& operator<<(std::ostream& os, ErrorStatus const& err)
    {
        switch (err.code) {
            case ErrorCode::Success:
                return os << "Success";
            case ErrorCode::Aborted:
                return os << "Aborted";
            case ErrorCode::Failure:
                return os << "Failure";
            default:
                return os << "UnknownErrorCode";
        }
    }
};


}  // namespace device
}  // namespace hal

#endif
