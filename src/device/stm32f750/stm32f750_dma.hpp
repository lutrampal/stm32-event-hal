
/*******************************************************************************
 * Header file for STM32F750 DMA
 ******************************************************************************/

#ifndef _HAL_DEVICE_STM32F75_DMA_HPP
#define _HAL_DEVICE_STM32F75_DMA_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <array>
#include <device/dma_device.hpp>
#include <device/exceptions/dma_exceptions.hpp>
#include <hardware/mcu.hpp>
#include <memory>
#include <string>
#include <utility>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class Stm32f750Dma : public DmaDevice
{
  public:
    static constexpr unsigned nb_streams = 8;

    Stm32f750Dma(DMA_TypeDef* dma,
                 std::array<IRQn_Type, nb_streams>&& irq_nbs,
                 volatile uint32_t* clk_en_reg,
                 uint32_t clk_en_msk,
                 volatile uint32_t* rst_reg,
                 uint32_t rst_msk);
    ~Stm32f750Dma();

    void startTransfer(unsigned stream_id,
                       const Location& src,
                       const Location& dst,
                       size_t count,
                       TransferDirection dir,
                       TransferPriority prio) override;
    bool suspendTransfer(unsigned stream_id) override;
    bool resumeTransfer(unsigned stream_id) override;
    bool cancelTransfer(unsigned stream_id) override;

    /** When transferring to or from a peripheral, this function must be called
     * prior to starting the transfer in order to select which peripheral will
     * send the request signal to the stream. (cf reference manual)
     * The default is channel 0. */
    void setChannel(unsigned stream_id, unsigned channel_id);

    void onTransferComplete(unsigned stream_id);
    void onTransferError(unsigned stream_id);
    void onDirectModeError(unsigned stream_id);
    void onFifoError(unsigned stream_id);

    /* The following functions output the masks of specific bits in DMA_ISR
     * and DMA_IFCR registers for a given stream. (cf reference manual)
     * These are public because they're also used by the interrupt handlers */
    static constexpr uint32_t CFEIFx(unsigned stream_id)
    {
        unsigned stream_id_mod = stream_id % 4; /* CFEIF0 <=> CFEIF4 */
        return 1 << (4 * (stream_id_mod >> 1) + 6 * stream_id_mod);
    }

    static constexpr uint32_t CDMEIFx(unsigned stream_id)
    {
        unsigned stream_id_mod = stream_id % 4; /* CDMEIF0 <=> CDMEIF4 */
        return 1 << (4 * (stream_id_mod >> 1) + 6 * stream_id_mod + 2);
    }

    static constexpr uint32_t CTEIFx(unsigned stream_id)
    {
        unsigned stream_id_mod = stream_id % 4; /* CTEIF0 <=> CTEIF4 */
        return 1 << (4 * (stream_id_mod >> 1) + 6 * stream_id_mod + 3);
    }

    static constexpr uint32_t CHTIFx(unsigned stream_id)
    {
        unsigned stream_id_mod = stream_id % 4; /* CHTIF0 <=> CHTIF4 */
        return 1 << (4 * (stream_id_mod >> 1) + 6 * stream_id_mod + 4);
    }

    static constexpr uint32_t CTCIFx(unsigned stream_id)
    {
        unsigned stream_id_mod = stream_id % 4; /* CTCIF0 <=> CTCIF4 */
        return 1 << (4 * (stream_id_mod >> 1) + 6 * stream_id_mod + 5);
    }

    static constexpr uint32_t FEIFx(unsigned stream_id)
    {
        return CFEIFx(stream_id);
    }

    static constexpr uint32_t DMEIFx(unsigned stream_id)
    {
        return CDMEIFx(stream_id);
    }

    static constexpr uint32_t TEIFx(unsigned stream_id)
    {
        return CTEIFx(stream_id);
    }

    static constexpr uint32_t HTIFx(unsigned stream_id)
    {
        return CHTIFx(stream_id);
    }

    static constexpr uint32_t TCIFx(unsigned stream_id)
    {
        return CTCIFx(stream_id);
    }

  private:
    /** Running transfers are represented by their size in bytes (uint32_t) and
     * their data width which are used to deduce, at the end of the transfer,
     * how much data was actually copied. */
    struct RunningTransfer {
        size_t count;
        DataWidth width;
    };

    inline volatile uint32_t* DMA_ISR_ptr(unsigned stream_id);
    inline volatile uint32_t* DMA_IFCR_ptr(unsigned stream_id);
    inline volatile uint32_t* DMA_SxCR_ptr(unsigned stream_id);
    inline volatile uint32_t* DMA_SxNDTR_ptr(unsigned stream_id);
    inline volatile uint32_t* DMA_SxPAR_ptr(unsigned stream_id);
    inline volatile uint32_t* DMA_SxM0AR_ptr(unsigned stream_id);
    inline volatile uint32_t* DMA_SxM1AR_ptr(unsigned stream_id);
    inline volatile uint32_t* DMA_SxFCR_ptr(unsigned stream_id);

    static inline uint32_t priorityToPLBits(TransferPriority prio);
    static inline uint32_t dataWidthToXSIZEBits(DataWidth data_width);
    static inline uint32_t transferDirectionToDIRBits(TransferDirection dir);

    static constexpr unsigned nb_channels_per_stream = 8;

    DMA_TypeDef* const dma;
    /** There is one IRQ line per stream */
    const std::array<IRQn_Type, nb_streams> irq_nbs;
    std::array<unsigned, nb_streams> selected_channels;
    std::array<std::unique_ptr<RunningTransfer>, nb_streams> running_transfers;
    volatile uint32_t* const clk_en_reg;
    const uint32_t clk_en_msk;
    volatile uint32_t* const rst_reg;
    const uint32_t rst_msk;
};

struct InvalidChannelIdException : DmaException {
    unsigned channel_id;

    InvalidChannelIdException(unsigned channel_id): channel_id{channel_id}
    {
    }

    const char* what() const noexcept override
    {
        return ("Invalid channel ID: " + std::to_string(channel_id)).c_str();
    }
};

}  // namespace device
}  // namespace hal

#endif
