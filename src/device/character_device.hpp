
/*******************************************************************************
 * Interface file for character-like devices
 ******************************************************************************/

#ifndef _HAL_DEVICE_CHARACTER_DEVICE_HPP
#define _HAL_DEVICE_CHARACTER_DEVICE_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "error_status.hpp"

#include <cstddef>
#include <functional>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

template<typename T>
class CharacterDevice
{
  public:
    /** Set the callback function that will be called once the write operation
     * is complete (i.e. the specified number of bytes was written or the
     * operation failed). This will be called from an interrupt context.
     * @param callback
     *  The new callback function. It will receive as paramater the number of
     * bytes written and an error status indicating if the write operation was
     * succesfully executed or not. */
    void setWriteCompleteCallback(
        std::function<void(size_t, ErrorStatus&&)>&& callback)
    {
        write_complete_callback = callback;
    }

    /** Begin a write operation
     * @param buf
     *  Pointer to the first character to write
     * @param buf_size
     *  The number of characters that will be written in the next write
     * operation */
    virtual void startWrite(const T* buf, size_t buf_size) = 0;
    virtual bool suspendWrite()                            = 0;
    virtual bool resumeWrite()                             = 0;

  protected:
    const T* buf;
    size_t nb_to_write = 0;
    size_t nb_written  = 0;
    std::function<void(size_t, ErrorStatus&&)> write_complete_callback;
};

}  // namespace device
}  // namespace hal

#endif
