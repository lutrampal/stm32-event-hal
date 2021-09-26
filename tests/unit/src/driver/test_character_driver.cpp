#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include <driver/character_driver.hpp>
#include <event_loop.hpp>
#include <mocks/device/mock_character_device.hpp>
#include <optional>

using namespace hal;
using namespace driver;
using namespace device;
using namespace std;

static void mock_callback(size_t nb, ErrorStatus& err)
{
    mock().actualCall(__func__).withParameter("nb", nb).withParameterOfType(
        "ErrorStatus", "err", static_cast<void*>(&err));
}

// clang-format off
TEST_GROUP(TestCharacterDriver)
{
    void setup()
    {
        mock().ignoreOtherCalls();
        mock().strictOrder();
    }

    void teardown()
    {
    }
};
// clang-format on

/** Simple write request test */
TEST(TestCharacterDriver, Test_CharacterDriver_WriteBasic)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    const char* str = "Hello, World!\r\n";
    size_t sz       = 15;
    ErrorStatus expected_err{ErrorCode::Success};

    mock()
        .expectOneCall("startWrite")
        .onObject(&dev)
        .withConstPointerParameter("buf", str)
        .withParameter("buf_size", sz);
    drv.asyncWrite(str, sz, mock_callback);

    dev.mockWrite(sz);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("nb", sz)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}

/** Try to write null buffer */
TEST(TestCharacterDriver, Test_CharacterDriver_WriteNull)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    CHECK_THROWS(StartAsyncOpFailure,
                 drv.asyncWrite(nullptr, 0, mock_callback));
}

/** Cancel running write request */
TEST(TestCharacterDriver, Test_CharacterDriver_WriteCancel)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    const char* str = "Hello, World!\r\n";
    size_t sz = 15, sz_partial = 10;
    ErrorStatus expected_err{ErrorCode::Aborted};

    mock()
        .expectOneCall("startWrite")
        .onObject(&dev)
        .withConstPointerParameter("buf", str)
        .withParameter("buf_size", sz);
    drv.asyncWrite(str, sz, mock_callback);

    dev.mockWrite(sz_partial);

    mock().expectOneCall("cancelWrite").onObject(&dev);
    drv.cancelAsyncWrite();

    mock()
        .expectOneCall("mock_callback")
        .withParameter("nb", sz_partial)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}

/** Try to start a second write request while another one is already running
 */
TEST(TestCharacterDriver, Test_CharacterDriver_WriteTwice)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    const char* str = "Hello, World!\r\n";
    size_t sz       = 15;
    ErrorStatus expected_err{ErrorCode::Success};

    mock()
        .expectOneCall("startWrite")
        .onObject(&dev)
        .withConstPointerParameter("buf", str)
        .withParameter("buf_size", sz);
    drv.asyncWrite(str, sz, mock_callback);

    CHECK_THROWS(StartAsyncOpFailure, drv.asyncWrite(str, sz, mock_callback));

    dev.mockWrite(sz);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("nb", sz)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}

/** Simple read request test */
TEST(TestCharacterDriver, Test_CharacterDriver_ReadBasic)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    size_t sz = 15;
    char str[sz], stop_char = '\0';
    ErrorStatus expected_err{ErrorCode::Success};
    optional<char> opt_stop_char{stop_char};  // for expected call

    mock()
        .expectOneCall("startRead")
        .onObject(&dev)
        .withPointerParameter("buf", str)
        .withParameter("buf_size", sz)
        .withParameterOfType("optional<char>", "stop_char", &opt_stop_char);
    drv.asyncRead(str, sz, mock_callback, stop_char);

    dev.mockRead(sz);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("nb", sz)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}

/** Try to read to null buffer */
TEST(TestCharacterDriver, Test_CharacterDriver_ReadNull)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    CHECK_THROWS(StartAsyncOpFailure, drv.asyncRead(nullptr, 0, mock_callback));
}

/** Cancel running read request */
TEST(TestCharacterDriver, Test_CharacterDriver_ReadCancel)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    size_t sz = 15, sz_partial = 10;
    char str[sz];
    ErrorStatus expected_err{ErrorCode::Aborted};

    mock()
        .expectOneCall("startRead")
        .onObject(&dev)
        .withPointerParameter("buf", str)
        .withParameter("buf_size", sz)
        .ignoreOtherParameters();  // ignore optional stop_char
    drv.asyncRead(str, sz, mock_callback);

    dev.mockRead(sz_partial);

    mock().expectOneCall("cancelRead").onObject(&dev);
    drv.cancelAsyncRead();

    mock()
        .expectOneCall("mock_callback")
        .withParameter("nb", sz_partial)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}

/** Try to start a second read request while another one is already running */
TEST(TestCharacterDriver, Test_CharacterDriver_ReadTwice)
{
    EventLoop el;
    MockCharacterDevice dev;
    CharacterDriver<char> drv{el, dev};

    size_t sz = 15;
    char str[sz];
    ErrorStatus expected_err{ErrorCode::Success};

    mock()
        .expectOneCall("startRead")
        .onObject(&dev)
        .withPointerParameter("buf", str)
        .withParameter("buf_size", sz)
        .ignoreOtherParameters();  // ignore optional stop_char
    drv.asyncRead(str, sz, mock_callback);

    CHECK_THROWS(StartAsyncOpFailure, drv.asyncRead(str, sz, mock_callback));

    dev.mockRead(sz);
    mock()
        .expectOneCall("mock_callback")
        .withParameter("nb", sz)
        .withParameterOfType("ErrorStatus", "err",
                             static_cast<void*>(&expected_err));
    el.run();
}
