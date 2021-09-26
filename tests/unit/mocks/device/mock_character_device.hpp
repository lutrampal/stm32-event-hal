
#include "CppUTestExt/MockSupport.h"

#include <device/character_device.hpp>

namespace hal
{
namespace device
{
class MockCharacterDevice : public CharacterDevice<char>
{
  public:
    void startWrite(const char* buf, size_t buf_size) override
    {
        mock()
            .actualCall(__func__)
            .onObject(this)
            .withConstPointerParameter("buf", buf)
            .withParameter("buf_size", buf_size);
        w_size     = buf_size;
        nb_written = 0;
    }

    bool cancelWrite(size_t& nb_written) override
    {
        mock().actualCall(__func__).onObject(this);
        nb_written = this->nb_written;
        return true;
    }

    void startRead(char* buf,
                   size_t buf_size,
                   std::optional<char> stop_char = std::nullopt) override
    {
        mock()
            .actualCall(__func__)
            .onObject(this)
            .withPointerParameter("buf", buf)
            .withParameter("buf_size", buf_size)
            .withParameterOfType("optional<char>", "stop_char",
                                 static_cast<void*>(&stop_char));
        r_size  = buf_size;
        nb_read = 0;
    }

    bool cancelRead(size_t& nb_read) override
    {
        mock().actualCall(__func__).onObject(this);
        nb_read = this->nb_read;
        return true;
    }

    void mockRead(size_t nb_read)
    {
        this->nb_read += nb_read;
        if (this->nb_read >= r_size && read_complete_callback) {
            read_complete_callback(nb_read, ErrorCode::Success);
        }
    }

    void mockWrite(size_t nb_written)
    {
        this->nb_written += nb_written;
        if (this->nb_written >= w_size && write_complete_callback) {
            write_complete_callback(nb_written, ErrorCode::Success);
        }
    }

  private:
    size_t r_size;
    size_t nb_read;

    size_t w_size;
    size_t nb_written;
};

}  // namespace device
}  // namespace hal
