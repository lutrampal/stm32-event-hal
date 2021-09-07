
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

    if (!write_queue.empty()) {
        /* A write op is running, suspend it as we don't want the interrupt to
         * go off while we're updating the internals of the class */
        if (!device.suspendWrite()) {
            throw StartAsyncOpFailure{};
        }

        write_queue.push_back(WriteOp{event_callback, buf, nb_elem});
        /* Nothing more to do but resume */
        if (!device.resumeWrite()) {
            /* Resume fail so we won't be able to start the new write => pop it
             * back */
            write_queue.pop_back();
            throw StartAsyncOpFailure{};
        }
    } else {
        /* We only have one write op going => start it immediately */
        write_queue.push_back(WriteOp{event_callback, buf, nb_elem});
        device.startWrite(buf, nb_elem);
    }
}

template<typename T>
void hal::driver::CharacterDriver<T>::completeWrite(
    size_t nb_written,
    hal::device::ErrorStatus&& status)
{
    if (write_queue.front().callback) {
        event_loop.pushEvent(
            bind(write_queue.front().callback, nb_written, status));
    }

    write_queue.pop_front();

    if (!write_queue.empty()) {
        WriteOp next_op = write_queue.front();
        device.startWrite(next_op.buf, next_op.nb_elem);
    }
}
