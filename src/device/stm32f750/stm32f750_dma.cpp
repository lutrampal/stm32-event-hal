
/*******************************************************************************
 * Implementation file for DMAs of STM32F750
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "stm32f750_dma.hpp"

#include <cstdint>
#include <device/exceptions/dma_exceptions.hpp>

using namespace std;
using namespace hal::device;

/*******************************************************************************
 * LOCAL FUNCTION DECLARATIONS
 ******************************************************************************/

// namespace
// {
// inline volatile uint32_t* DMA_ISR(unsigned stream_id);
// inline volatile uint32_t* DMA_IFCR(unsigned stream_id);
// inline volatile uint32_t* DMA_SxCR(unsigned stream_id);
// inline volatile uint32_t* DMA_SxNDTR(unsigned stream_id);
// inline volatile uint32_t* DMA_SxPAR(unsigned stream_id);
// inline volatile uint32_t* DMA_SxM0AR(unsigned stream_id);
// inline volatile uint32_t* DMA_SxM1AR(unsigned stream_id);
// inline volatile uint32_t* DMA_SxFCR(unsigned stream_id);
// }  // namespace

/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

Stm32f750Dma::Stm32f750Dma(DMA_TypeDef* dma,
                           std::array<IRQn_Type, nb_streams>&& irq_nbs,
                           volatile uint32_t* clk_en_reg,
                           uint32_t clk_en_msk,
                           volatile uint32_t* rst_reg,
                           uint32_t rst_msk)
: dma{dma}, irq_nbs{irq_nbs}, clk_en_reg{clk_en_reg},
  clk_en_msk{clk_en_msk}, rst_reg{rst_reg}, rst_msk{rst_msk}
{
    *clk_en_reg |= clk_en_msk;
}

Stm32f750Dma::~Stm32f750Dma()
{
    for (auto& irq_nb : irq_nbs) { NVIC_DisableIRQ(irq_nb); }

    *rst_reg |= rst_msk;
    *rst_reg &= ~rst_msk;

    *clk_en_reg &= ~clk_en_msk;
}


/*******************************************************************************
 * OPERATOR IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE METHOD IMPLEMENTATIONS
 ******************************************************************************/

inline volatile uint32_t* Stm32f750Dma::DMA_ISR_ptr(unsigned stream_id)
{
    if (stream_id > 3) {
        return &dma->HISR;
    } else {
        return &dma->LISR;
    }
}

inline volatile uint32_t* Stm32f750Dma::DMA_IFCR_ptr(unsigned stream_id)
{
    if (stream_id > 3) {
        return &dma->HIFCR;
    } else {
        return &dma->LIFCR;
    }
}

inline volatile uint32_t* Stm32f750Dma::DMA_SxCR_ptr(unsigned stream_id)
{
    return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(dma) + 0x10
                                       + (0x18 * stream_id));
}

inline volatile uint32_t* Stm32f750Dma::DMA_SxNDTR_ptr(unsigned stream_id)
{
    return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(dma) + 0x14
                                       + (0x18 * stream_id));
}

inline volatile uint32_t* Stm32f750Dma::DMA_SxPAR_ptr(unsigned stream_id)
{
    return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(dma) + 0x18
                                       + (0x18 * stream_id));
}

inline volatile uint32_t* Stm32f750Dma::DMA_SxM0AR_ptr(unsigned stream_id)
{
    return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(dma) + 0x1C
                                       + (0x18 * stream_id));
}

inline volatile uint32_t* Stm32f750Dma::DMA_SxM1AR_ptr(unsigned stream_id)
{
    return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(dma) + 0x20
                                       + (0x18 * stream_id));
}

inline volatile uint32_t* Stm32f750Dma::DMA_SxFCR_ptr(unsigned stream_id)
{
    return reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(dma) + 0x24
                                       + (0x18 * stream_id));
}

inline uint32_t Stm32f750Dma::priorityToPLBits(TransferPriority prio)
{
    switch (prio) {
        case TransferPriority::Low:
            return 0b00;
        case TransferPriority::Medium:
            return 0b01;
        case TransferPriority::High:
            return 0b10;
        case TransferPriority::VeryHigh:
            return 0b11;
        default:
            /* Unreachable */
            throw DmaException{};
    }
}

inline uint32_t Stm32f750Dma::dataWidthToXSIZEBits(DataWidth data_width)
{
    switch (data_width) {
        case DataWidth::Byte:
            return 0b00;
        case DataWidth::HalfWord:
            return 0b01;
        case DataWidth::Word:
            return 0b10;
        default:
            /* Unreachable */
            throw DmaException{};
    }
}

inline uint32_t Stm32f750Dma::transferDirectionToDIRBits(TransferDirection dir)
{
    switch (dir) {
        case TransferDirection::PeriphToMem:
            return 0b00;
        case TransferDirection::MemToPeriph:
            return 0b01;
        case TransferDirection::MemToMem:
            return 0b10;
        default:
            /* Unreachable */
            throw DmaException{};
    }
}

/*******************************************************************************
 * PROTECTED METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/

void Stm32f750Dma::startTransfer(unsigned stream_id,
                                 const Location& src,
                                 const Location& dst,
                                 size_t count,
                                 TransferDirection dir,
                                 TransferPriority prio)
{
    if (stream_id >= nb_streams) {
        throw InvalidStreamIdException{stream_id};
    }

    volatile uint32_t* DMA_SxCR   = DMA_SxCR_ptr(stream_id);
    volatile uint32_t* DMA_IFCR   = DMA_IFCR_ptr(stream_id);
    volatile uint32_t* DMA_SxPAR  = DMA_SxPAR_ptr(stream_id);
    volatile uint32_t* DMA_SxMA0R = DMA_SxM0AR_ptr(stream_id);
    volatile uint32_t* DMA_SxNDTR = DMA_SxNDTR_ptr(stream_id);
    volatile uint32_t* DMA_SxFCR  = DMA_SxFCR_ptr(stream_id);
    DataWidth periph_width, mem_width;
    bool periph_incr_addr, mem_incr_addr;

    /* Config procedure is taken from reference manual §8.3.18 */

    /* Step 1: Reset the stream, this will block until the current
     * transfer is finished if there is any. Then all interrupt flags
     * must be cleared. */
    *DMA_SxCR &= ~DMA_SxCR_EN;
    while (*DMA_SxCR & DMA_SxCR_EN) {}
    *DMA_IFCR |= CTCIFx(stream_id) | CHTIFx(stream_id) | CTEIFx(stream_id)
                 | CDMEIFx(stream_id) | CFEIFx(stream_id);

    /* Step 2 & 3: Set peripheral & memory addresses for transfer */
    switch (dir) {
        case TransferDirection::PeriphToMem:
        case TransferDirection::MemToMem:
            /* During a memory to memory transfer, the peripheral port is
             * repurposed as the source memory address. */
            *DMA_SxPAR       = src.addr;
            *DMA_SxMA0R      = dst.addr;
            periph_width     = src.data_width;
            mem_width        = dst.data_width;
            periph_incr_addr = src.incr_addr;
            mem_incr_addr    = dst.incr_addr;
            break;
        case TransferDirection::MemToPeriph:
            *DMA_SxPAR       = dst.addr;
            *DMA_SxMA0R      = src.addr;
            periph_width     = dst.data_width;
            mem_width        = src.data_width;
            periph_incr_addr = dst.incr_addr;
            mem_incr_addr    = src.incr_addr;
            break;
        default:
            /* Unreachable */
            throw DmaException{};
            break;
    }

    /* Step 4: Configure transfer size */
    if (count / static_cast<size_t>(periph_width)
        > numeric_limits<uint16_t>::max()) {
        throw InvalidTransferSizeException{periph_width, count};
    }

    *DMA_SxNDTR = count / static_cast<size_t>(periph_width);

    /* Step 5: Configure FIFO usage */
    /* TODO: direct mode selection, threshold selection */
    *DMA_SxFCR |= DMA_SxFCR_FEIE;

    /* Step 6: Insert new transfer before enabling the hardware stream so that
     * if it fails the IRQ handler will release the memory immediately */
    running_transfers[stream_id].reset(
        new RunningTransfer{count, periph_width});

    /* Step 7: Configure the channel, stream priority, data transfer
     * direction, peripheral and memory incremented/fixed mode, single
     * or burst transactions, peripheral and memory data widths and IRQs
     * Then enable the stream */
    *DMA_SxCR &= ~DMA_SxCR_CHSEL & ~DMA_SxCR_PL & ~DMA_SxCR_MSIZE
                 & ~DMA_SxCR_PSIZE & ~DMA_SxCR_MINC & ~DMA_SxCR_PINC
                 & ~DMA_SxCR_DIR;
    NVIC_EnableIRQ(irq_nbs[stream_id]);
    *DMA_SxCR |= (selected_channels[stream_id] << DMA_SxCR_CHSEL_Pos)
                 | (priorityToPLBits(prio) << DMA_SxCR_PL_Pos)
                 | (dataWidthToXSIZEBits(mem_width) << DMA_SxCR_MSIZE_Pos)
                 | (dataWidthToXSIZEBits(periph_width) << DMA_SxCR_PSIZE_Pos)
                 | (mem_incr_addr << DMA_SxCR_MINC_Pos)
                 | (periph_incr_addr << DMA_SxCR_PINC_Pos)
                 | (transferDirectionToDIRBits(dir) << DMA_SxCR_DIR_Pos)
                 | DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | DMA_SxCR_EN;

    /* TODO: Bust mode configuration
     * it's a bit more complex than it seems because if we want to automatically
     * deduce the burst & fifo configs then we must take care not to cross the
     * 1KB address boundary or the AHB will raise an error. */
}

bool Stm32f750Dma::suspendTransfer(unsigned stream_id)
{
    /* TODO: Currently unimplemented
     * Suspending a transfer will generate a TransferComplete event so we need
     * to ignore it in the event handler. */
    return false;
}

bool Stm32f750Dma::resumeTransfer(unsigned stream_id)
{
    /* TODO: Currently unimplemented */
    return false;
}

bool Stm32f750Dma::cancelTransfer(unsigned stream_id)
{
    volatile uint32_t* DMA_SxCR = DMA_SxCR_ptr(stream_id);

    *DMA_SxCR &= ~DMA_SxCR_EN;
    while (*DMA_SxCR & DMA_SxCR_EN) {}

    return true;
}

void Stm32f750Dma::setChannel(unsigned stream_id, unsigned channel_id)
{
    if (stream_id >= nb_streams) {
        throw InvalidStreamIdException{stream_id};
    }

    if (channel_id >= nb_channels_per_stream) {
        throw InvalidChannelIdException{channel_id};
    }

    selected_channels[stream_id] = channel_id;
}

void Stm32f750Dma::onTransferComplete(unsigned stream_id)
{
    if (transfer_complete_callback) {
        size_t nb_transferred =
            running_transfers[stream_id]->count
            - (*DMA_SxNDTR_ptr(stream_id)
               * static_cast<uint32_t>(running_transfers[stream_id]->width));
        transfer_complete_callback(
            stream_id, nb_transferred,
            nb_transferred == running_transfers[stream_id]->count ?
                ErrorCode::Success :
                ErrorCode::Aborted);
    }
    running_transfers[stream_id].release();
}

void Stm32f750Dma::onTransferError(unsigned stream_id)
{
    if (transfer_complete_callback) {
        size_t nb_transferred =
            running_transfers[stream_id]->count
            - (*DMA_SxNDTR_ptr(stream_id)
               * static_cast<uint32_t>(running_transfers[stream_id]->width));
        transfer_complete_callback(stream_id, nb_transferred,
                                   ErrorCode::Failure);
    }
    running_transfers[stream_id].release();
}

void Stm32f750Dma::onDirectModeError(unsigned stream_id)
{
    onDirectModeError(stream_id);
}

void Stm32f750Dma::onFifoError(unsigned stream_id)
{
    /* TODO: Currently, we do nothing but disabling the stream but here we could
     * check if this is an underrun or overrun issue, resolve the issue and go
     * on. */

    volatile uint32_t* DMA_SxCR = DMA_SxCR_ptr(stream_id);
    *DMA_SxCR &= ~DMA_SxCR_EN;
    while (*DMA_SxCR & DMA_SxCR_EN) {}

    if (transfer_complete_callback) {
        size_t nb_transferred =
            running_transfers[stream_id]->count
            - (*DMA_SxNDTR_ptr(stream_id)
               * static_cast<uint32_t>(running_transfers[stream_id]->width));
        transfer_complete_callback(stream_id, nb_transferred,
                                   ErrorCode::Failure);
    }
    running_transfers[stream_id].release();
}


/*******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATIONS
 ******************************************************************************/
