
/*******************************************************************************
 * Enum representing various kinds of errors
 ******************************************************************************/

#ifndef _HAL_DEVICE_ERROR_CODE_HPP
#define _HAL_DEVICE_ERROR_CODE_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/


namespace hal
{
namespace device
{
/*******************************************************************************
 * TYPE DEFINITION
 ******************************************************************************/

enum class ErrorCode {
    Success,          // Successful completion of operation
    Aborted,          // The operation was cancelled/aborted
    BufferOverflow,   // The buffer is full but read termination condition is
                      // false
    HwProtocolError,  // Hardware peripheral reported protocol error
    Timeout,          // The operation took too much time
    NbCodes
};

}  // namespace device
}  // namespace hal

#endif
