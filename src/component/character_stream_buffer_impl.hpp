
/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "character_stream_buffer.hpp"

#include <functional>


/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

template<typename T>
hal::component::CharacterStreamBuffer<T>::CharacterStreamBuffer(
    hal::driver::CharacterDriver<T>& driver)
: driver{driver}, buf_out{new std::vector<T>}
{
    this->setp(buf_out->data(), buf_out->data() + buf_out->capacity());
}

/*******************************************************************************
 * OPERATOR IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE METHOD IMPLEMENTATIONS
 ******************************************************************************/

template<typename T>
void hal::component::CharacterStreamBuffer<T>::bufferWrittenCallback(
    size_t nb_written,
    device::ErrorStatus& status)
{
    (void)nb_written;
    (void)status;

    using namespace std;
    using namespace hal::component;

    /* Remove the write op that was just performed, schedule the next one if
     * any. There is not much we can do if write fails. */
    pending_out.pop_front();
    if (!pending_out.empty()) {
        driver.asyncWrite(pending_out.front()->data(),
                          pending_out.front()->size(),
                          bind(&CharacterStreamBuffer::bufferWrittenCallback,
                               this, placeholders::_1, placeholders::_2));
    }
}

/*******************************************************************************
 * PROTECTED METHOD IMPLEMENTATIONS
 ******************************************************************************/

template<typename T>
typename std::basic_streambuf<T>::int_type
    hal::component::CharacterStreamBuffer<T>::overflow(
        typename std::basic_streambuf<T>::int_type ch)
{
    using namespace std;

    if (!char_traits<T>::eq_int_type(ch, char_traits<T>::eof())) {
        buf_out->push_back(ch);
        this->setp(buf_out->data(), buf_out->data() + buf_out->capacity());
        this->pbump(buf_out->size());
    }

    return char_traits<T>::not_eof(ch);
}

template<typename T>
std::streamsize
    hal::component::CharacterStreamBuffer<T>::xsputn(const T* s,
                                                     std::streamsize count)
{
    buf_out->insert(buf_out->end(), s, s + count);
    this->setp(buf_out->data(), buf_out->data() + buf_out->capacity());
    this->pbump(buf_out->size());

    return count;
}

template<typename T>
int hal::component::CharacterStreamBuffer<T>::sync()
{
    using namespace std;
    using namespace hal::component;

    pending_out.push_back(unique_ptr<vector<T>>{buf_out.release()});
    if (pending_out.size() == 1) {
        /* There are no other pending write requests, send this one now to the
         * driver */
        driver.asyncWrite(pending_out.back()->data(),
                          pending_out.back()->size(),
                          bind(&CharacterStreamBuffer::bufferWrittenCallback,
                               this, placeholders::_1, placeholders::_2));
    }

    /* Assumption: The size of all writes on this stream will be roughly the
     * same so we pre-allocate new output buffer with the same capacity as the
     * previous one. */
    buf_out.reset(new vector<T>);
    buf_out->reserve(pending_out.back()->capacity());
    this->setp(buf_out->data(), buf_out->data() + buf_out->capacity());

    return 0;
}

/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/
