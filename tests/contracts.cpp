/** @file contracts.cpp
 * @brief Disposable device doubles verify configuration, ownership and error boundaries.
 */
#include <tarmac/acquisition/Error.hpp>
#include <tarmac/acquisition/services/AcquisitionService.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>
namespace {
/** @brief Owning sample independent of a storage format. */
struct Sample { int value; };
/** @brief Test device counting reads; owns all fixture state. */
class Device final : public tarmac::acquisition::DeviceAdapter<Sample> {
public:
    int calls = 0;
    bool fail = false;
    /** @copydoc tarmac::acquisition::DeviceAdapter::read */
    Sample read() override {
        ++calls;
        if (fail) throw tarmac::acquisition::Error("device failed");
        return {calls};
    }
};
/** @brief Test observer owning counters; no callbacks escape the call. */
class Monitor final : public tarmac::acquisition::AcquisitionMonitor {
public:
    int completed = 0;
    int failed = 0;
    /** @copydoc tarmac::acquisition::AcquisitionMonitor::read_completed */
    void read_completed() noexcept override { ++completed; }
    /** @copydoc tarmac::acquisition::AcquisitionMonitor::read_failed */
    void read_failed() noexcept override { ++failed; }
};
/** @brief Fail a contract in all build modes.
 * @param value Contract expression. @throws std::runtime_error On failure.
 */
void require(bool value) { if (!value) throw std::runtime_error("contract failed"); }
}
/** @brief Test no-I/O validation, read count, errors and continued explicit use.
 * @return Zero on success. Exceptions fail the process/test.
 */
int main() {
    using namespace tarmac::acquisition;
    static_assert(std::has_virtual_destructor_v<DeviceAdapter<Sample>>);
    static_assert(std::has_virtual_destructor_v<AcquisitionMonitor>);
    Device device;
    Monitor monitor;
    for (auto interval : {std::chrono::nanoseconds{0}, std::chrono::nanoseconds{-1}}) {
        bool caught = false;
        try { AcquisitionService<Sample> invalid(device, monitor, {interval}); }
        catch (const std::invalid_argument& e) { caught = std::string(e.what()) == "Sampling interval must be positive"; }
        require(caught && device.calls == 0 && monitor.completed == 0 && monitor.failed == 0);
    }
    SamplingConfig config{std::chrono::milliseconds{50}};
    AcquisitionService<Sample> service(device, monitor, config);
    config.interval = std::chrono::nanoseconds{0};
    require(service.sampling_config().interval == std::chrono::milliseconds{50});
    require(service.sample_once().value == 1 && device.calls == 1 && monitor.completed == 1);
    device.fail = true;
    bool caught = false;
    try { (void)service.sample_once(); }
    catch (const Error& e) { caught = std::string(e.what()) == "device failed"; }
    require(caught && device.calls == 2 && monitor.failed == 1 && monitor.completed == 1);
    device.fail = false;
    require(service.sample_once().value == 3 && device.calls == 3 && monitor.completed == 2 && monitor.failed == 1);
}
