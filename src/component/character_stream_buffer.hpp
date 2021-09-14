
/*******************************************************************************
 * An implementation of std::streambuf capable of RW operations on
 * hal::device::characterDriver
 * TODO: Read op
 ******************************************************************************/

#ifndef _HAL_COMPONENT_CHARACTER_STREAM_BUFFER_HPP
#define _HAL_COMPONENT_CHARACTER_STREAM_BUFFER_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <array>
#include <device/error_status.hpp>
#include <driver/character_driver.hpp>
#include <list>
#include <memory>
#include <streambuf>
#include <vector>

namespace hal
{
namespace component
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

template<typename T>
class CharacterStreamBuffer : public std::basic_streambuf<T>
{
  public:
    CharacterStreamBuffer(driver::CharacterDriver<T>& driver);

  protected:
    typename std::basic_streambuf<T>::int_type
        overflow(typename std::basic_streambuf<T>::int_type ch) override;
    std::streamsize xsputn(const T* s, std::streamsize count) override;
    int sync() override;

  private:
    driver::CharacterDriver<T>& driver;
    /* Unique pointers are used here because the buffers will be moved to
     * pending lists when the sync() call is performed but flushing is not
     * complete yet */
    std::unique_ptr<std::vector<T>> buf_out;
    std::list<std::unique_ptr<std::vector<T>>> pending_out;

    void bufferWrittenCallback(size_t nb_written, device::ErrorStatus& status);
};
}  // namespace component
}  // namespace hal

#include "character_stream_buffer_impl.hpp"

#endif
