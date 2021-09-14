
/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "character_driver.hpp"
#include "driver_exceptions.hpp"

#include <algorithm>

/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

template<class T>
hal::driver::CharacterDriver<T>::CharacterDriver(
    EventLoop& event_loop,
    device::CharacterDevice<T>& device)
: event_loop{event_loop}, device{device}
{
    using namespace std;

    device.setWriteCompleteCallback(bind(&CharacterDriver::completeWrite, this,
                                         placeholders::_1, placeholders::_2));
    device.setReadCompleteCallback(bind(&CharacterDriver::completeRead, this,
                                        placeholders::_1, placeholders::_2));
}

/*******************************************************************************
 * OPERATOR IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PROTECTED METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/


template<typename T>
void hal::driver::CharacterDriver<T>::asyncWrite(
    const T* buf,
    size_t nb_elem,
    std::function<void(size_t, hal::device::ErrorStatus&)>&& event_callback)
{
    using namespace std;
    using namespace hal::device;

    if (busy_w) {
        throw StartAsyncOpFailure{"Driver is busy"};
    }

    device.startWrite(buf, nb_elem);
    busy_w         = true;
    write_callback = event_callback;
}

template<typename T>
void hal::driver::CharacterDriver<T>::completeWrite(
    size_t nb_written,
    hal::device::ErrorStatus&& status)
{
    if (write_callback) {
        event_loop.pushEvent(bind(write_callback, nb_written, status));
    }

    busy_w = false;
}

template<typename T>
void hal::driver::CharacterDriver<T>::cancelAsyncWrite()
{
    using namespace hal::device;

    if (!busy_w) {
        throw CancelAsyncOpFailure{"Nothing to cancel"};
    }

    if (!device.cancelWrite()) {
        throw CancelAsyncOpFailure{"Failed to cancel operation"};
    }

    busy_w = false;
}
template<typename T>
void hal::driver::CharacterDriver<T>::asyncRead(
    T* buf,
    size_t nb_elem,
    std::function<void(size_t, hal::device::ErrorStatus&)>&& event_callback,
    std::optional<T> stop_char)
{
    using namespace std;
    using namespace hal::device;

    if (busy_r) {
        throw StartAsyncOpFailure{"Driver is busy"};
    }

    device.startRead(buf, nb_elem, stop_char);
    busy_r        = true;
    read_callback = event_callback;
}

template<typename T>
void hal::driver::CharacterDriver<T>::completeRead(
    size_t nb_read,
    hal::device::ErrorStatus&& status)
{
    if (read_callback) {
        event_loop.pushEvent(bind(read_callback, nb_read, status));
    }

    busy_r = false;
}

template<typename T>
void hal::driver::CharacterDriver<T>::cancelAsyncRead()
{
    using namespace hal::device;

    if (!busy_r) {
        throw CancelAsyncOpFailure{"Nothing to cancel"};
    }

    if (!device.cancelRead()) {
        throw CancelAsyncOpFailure{"Failed to cancel operation"};
    }

    busy_r = false;
}
