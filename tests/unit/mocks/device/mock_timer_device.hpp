
#include "CppUTestExt/MockSupport.h"

#include <device/timer_device.hpp>

namespace hal
{
namespace device
{
class MockTimerDevice : public TimerDevice
{
  public:
    WaitTimeUnitDuration getRemainingWaitTime() override
    {
        mock().actualCall(__func__).onObject(this);
        return TimerDevice::WaitTimeUnitDuration{programmed_count};
    }

    bool suspendWait() override
    {
        mock().actualCall(__func__).onObject(this);
        return true;
    }

    bool resumeWait() override
    {
        mock().actualCall(__func__).onObject(this);
        return true;
    }

    bool cancelWait() override
    {
        mock().actualCall(__func__).onObject(this);
        return true;
    }

    bool startWait(WaitTimeUnitDuration::rep count) override
    {
        mock().actualCall(__func__).onObject(this).withParameter("count",
                                                                 count);
        programmed_count = count;
        return true;
    }

    void usleep(WaitTimeUnitDuration::rep count) override
    {
        mock().actualCall(__func__).onObject(this);
    }

    /* Simulate timer going off */
    void mockWaitFinished(ErrorStatus&& err)
    {
        wait_complete_callback(std::move(err));
    }
};

}  // namespace device
}  // namespace hal
