#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <algorithm>
#include <chrono>
#include <driver/timer_driver.hpp>
#include <event_loop.hpp>
#include <functional>
#include <iostream>
#include <mocks/device/mock_timer_device.hpp>
#include <utility>
#include <vector>

using namespace hal;
using namespace driver;
using namespace device;
using namespace std;
using namespace chrono;

typedef TimerDevice::WaitTimeUnitDuration WaitTimeUnitDuration;

static void mock_callback(int call_id, ErrorStatus& err)
{
    mock()
        .actualCall(__func__)
        .withParameter("call_id", call_id)
        .withParameterOfType("ErrorStatus", "err", static_cast<void*>(&err));
}

// clang-format off
TEST_GROUP(TestTimerDriver)
{
    void setup()
    {
        mock().ignoreOtherCalls();
        mock().strictOrder();
    }

    void teardown()
    {
    }
}
;
// clang-format on

/** Basic TimerDriver test :
 * - Request async wait
 * - Check that device receives start wait request
 * - Check that event loop receives callback */
TEST(TestTimerDriver, Test_TimerDriver_Basic)
{
    EventLoop el;
    MockTimerDevice dev;
    TimerDriver t{el, dev};

    seconds wait1{5};
    ErrorStatus expected_err{ErrorCode::Success};

    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count",
                       duration_cast<WaitTimeUnitDuration>(wait1).count());
    t.asyncWait(wait1, bind(mock_callback, 0, placeholders::_1));

    dev.mockWaitFinished(ErrorCode::Success);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 0)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}

/** Chain wait requests :
 * - Send several async wait requests, in order from the shortest to longest;
 * - Check that device receives all start wait requests one after the other with
 *      proper wait
 * - Check that event loop receives callbacks */
TEST(TestTimerDriver, Test_TimerDriver_Chain)
{
    EventLoop el;
    MockTimerDevice dev;
    TimerDriver t{el, dev};

    microseconds wait1{30};
    milliseconds wait2{200};
    seconds wait3{5};
    minutes wait4{16};
    vector<WaitTimeUnitDuration> waits{
        duration_cast<WaitTimeUnitDuration>(wait1),
        duration_cast<WaitTimeUnitDuration>(wait2),
        duration_cast<WaitTimeUnitDuration>(wait3),
        duration_cast<WaitTimeUnitDuration>(wait4)};

    ErrorStatus expected_err{ErrorCode::Success};

    /* used to keep track of the total wait time already requested */
    uint32_t count_acc = 0;
    for (int i = 0; i < waits.size(); ++i) {
        mock()
            .expectOneCall("startWait")
            .onObject(&dev)
            .withParameter("count", waits[i].count() - count_acc);

        t.asyncWait(waits[i], bind(mock_callback, i, placeholders::_1));

        /* Add the new wait time minus what is already requested, that's how
         * long we're expected to wait once this wait request is passed to
         * device */
        count_acc += waits[i].count() - count_acc;
    }

    for (int i = 0; i < waits.size(); ++i) {
        dev.mockWaitFinished(ErrorCode::Success);

        mock()
            .expectOneCall("mock_callback")
            .withParameter("call_id", i)
            .withParameterOfType("ErrorStatus", "err",
                                 static_cast<void*>(&expected_err));
    }

    el.run();
}

/** Chain wait requests. This time, the requested wait times out of order and we
 * expect the timer driver to reorder them. */
TEST(TestTimerDriver, Test_TimerDriver_ChainUnordered)
{
    EventLoop el;
    MockTimerDevice dev;
    TimerDriver t{el, dev};

    milliseconds wait1{200};
    microseconds wait2{30};
    minutes wait3{16};
    seconds wait4{5};

    ErrorStatus expected_err{ErrorCode::Success};

    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count",
                       duration_cast<WaitTimeUnitDuration>(wait1).count());
    t.asyncWait(wait1, bind(mock_callback, 0, placeholders::_1));
    /* pushing a smaller wait reschedules the previous one */
    mock().expectOneCall("cancelWait").onObject(&dev);
    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count",
                       duration_cast<WaitTimeUnitDuration>(wait2).count());
    t.asyncWait(wait2, bind(mock_callback, 1, placeholders::_1));

    t.asyncWait(wait3, bind(mock_callback, 2, placeholders::_1));

    t.asyncWait(wait4, bind(mock_callback, 3, placeholders::_1));


    uint32_t count_acc = duration_cast<WaitTimeUnitDuration>(wait2).count();

    /* End first wait and start next one, substracting the time that already
     * elapsed */
    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count",
                       duration_cast<WaitTimeUnitDuration>(wait1).count()
                           - count_acc);
    dev.mockWaitFinished(ErrorCode::Success);

    count_acc += duration_cast<WaitTimeUnitDuration>(wait1).count() - count_acc;
    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count",
                       duration_cast<WaitTimeUnitDuration>(wait4).count()
                           - count_acc);
    dev.mockWaitFinished(ErrorCode::Success);

    count_acc += duration_cast<WaitTimeUnitDuration>(wait4).count() - count_acc;
    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count",
                       duration_cast<WaitTimeUnitDuration>(wait3).count()
                           - count_acc);
    dev.mockWaitFinished(ErrorCode::Success);

    /* end last wait, no new start expected */
    dev.mockWaitFinished(ErrorCode::Success);

    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 1)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 0)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 3)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 2)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}

/** Cancel wait  :
 * - Send 3 async wait request
 * - Cancel the 2nd one
 * - Check that only the 1st and 3rd callbacks are executed */
TEST(TestTimerDriver, Test_TimerDriver_Cancel)
{
    EventLoop el;
    MockTimerDevice dev;
    TimerDriver t{el, dev};

    microseconds wait1{30};
    milliseconds wait2{200};
    seconds wait3{5};
    vector<WaitTimeUnitDuration> waits{
        duration_cast<WaitTimeUnitDuration>(wait1),
        duration_cast<WaitTimeUnitDuration>(wait2),
        duration_cast<WaitTimeUnitDuration>(wait3)};

    ErrorStatus expected_err{ErrorCode::Success};
    ErrorStatus canceled{ErrorCode::Aborted};

    /* used to keep track of the total wait time already requested */
    uint32_t count_acc = 0;

    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count", waits[0].count());
    t.asyncWait(waits[0], bind(mock_callback, 0, placeholders::_1));
    count_acc += waits[0].count();

    /* This will be canceled so we don't expect a call */
    TimerDriver::Timer timer =
        t.asyncWait(waits[1], bind(mock_callback, 1, placeholders::_1));

    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count", waits[2].count() - count_acc);
    t.asyncWait(waits[2], bind(mock_callback, 2, placeholders::_1));

    /* Wait hasn't started yet so we dont expected any message on the device
     * side */
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 1)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&canceled));
    timer.cancelWait();

    dev.mockWaitFinished(ErrorCode::Success);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 0)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));

    dev.mockWaitFinished(ErrorCode::Success);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 2)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));

    el.run();
}

/** Cancel a wait that was already executed */
TEST(TestTimerDriver, Test_TimerDriver_CancelExecd)
{
    EventLoop el;
    MockTimerDevice dev;
    TimerDriver t{el, dev};

    WaitTimeUnitDuration wait{microseconds{30}};

    ErrorStatus expected_err{ErrorCode::Success};

    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count", wait.count());
    TimerDriver::Timer timer =
        t.asyncWait(wait, bind(mock_callback, 0, placeholders::_1));

    dev.mockWaitFinished(ErrorCode::Success);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 0)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));

    CHECK_THROWS(CancelAsyncOpFailure, timer.cancelWait());

    el.run();
}

/** Cancel a running wait request
 * Check that the next wait op is started properly */
TEST(TestTimerDriver, Test_TimerDriver_CancelRunning)
{
    EventLoop el;
    MockTimerDevice dev;
    TimerDriver t{el, dev};

    WaitTimeUnitDuration wait1{microseconds{30}}, wait2{seconds{42}};

    ErrorStatus canceled{ErrorCode::Aborted}, successful{ErrorCode::Success};

    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count", wait1.count());
    TimerDriver::Timer timer =
        t.asyncWait(wait1, bind(mock_callback, 0, placeholders::_1));


    t.asyncWait(wait2, bind(mock_callback, 1, placeholders::_1));

    mock()
        .expectOneCall("startWait")
        .onObject(&dev)
        .withParameter("count", wait2.count());
    timer.cancelWait();

    dev.mockWaitFinished(ErrorCode::Success);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 0)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&canceled));
    mock()
        .expectOneCall("mock_callback")
        .withParameter("call_id", 1)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&successful));

    el.run();
}