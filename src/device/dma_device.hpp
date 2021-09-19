
/*******************************************************************************
 * Interface file for DMA devices
 ******************************************************************************/

#ifndef _HAL_DEVICE_DMA_DEVICE_HPP
#define _HAL_DEVICE_DMA_DEVICE_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "error_status.hpp"

#include <cstdint>
#include <functional>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class DmaDevice
{
  public:
    enum class TransferDirection { PeriphToMem, MemToPeriph, MemToMem };
    enum class DataWidth { Byte = 1, HalfWord = 2, Word = 4 };
    enum class TransferPriority { Low, Medium, High, VeryHigh };
    /** A data structure to represent either the source or the destination of a
     * DMA transfer. */
    struct Location {
        std::uintptr_t addr;
        DataWidth data_width;
        /** True if the address should be incremented each time a data item is
         * transferred, before transferring the next item.
         * If the location is a peripheral register FIFO, this should be
         * false. On the contrary, if the location is the start of a memory
         * buffer, then it should be true.*/
        bool incr_addr;
    };

    /** Start a new DMA transfer on the given stream.
     * @param stream_id
     *  Select which stream should handle the transfer. Stream IDs start at 0.
     * @param src
     *  The source location for the transfer
     * @param dst
     *  The destination location for the transfer
     * @param count
     *  The number of bytes to be transfered
     * @param dir
     *  Transfer direction
     * @param prio
     *  Transfer priority */
    virtual void startTransfer(unsigned stream_id,
                               const Location& src,
                               const Location& dst,
                               size_t count,
                               TransferDirection dir,
                               TransferPriority prio) = 0;
    virtual bool suspendTransfer(unsigned stream_id)  = 0;
    virtual bool cancelTransfer(unsigned stream_id)   = 0;
    virtual bool resumeTransfer(unsigned stream_id)   = 0;

    /** Set the callback function that will be called once the transfer
     * operation is complete. This will be called from an interrupt context.
     * @param callback
     *  The new callback function. It will receive as paramater the stream ID,
     * the number of bytes transfered and an error status indicating if the
     * transfer operation was succesfully executed or not. */
    void setTransferCompleteCallback(
        std::function<void(unsigned, size_t, ErrorStatus&&)>&& callback)
    {
        transfer_complete_callback = callback;
    }

  protected:
    std::function<void(unsigned, size_t, ErrorStatus&&)>
        transfer_complete_callback;
};

}  // namespace device
}  // namespace hal

#endif
