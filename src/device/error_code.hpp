
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
    Success,  // Successful completion of operation
    Aborted,  // The operation was cancelled/aborted
    Failure,  // Failure of operation (unspecified cause)
    NbCodes
};

}  // namespace device
}  // namespace hal

#endif
