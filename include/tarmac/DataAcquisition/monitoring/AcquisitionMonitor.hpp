/** @file AcquisitionMonitor.hpp
 * @brief Synchronous observation of device-read outcomes without an event bus.
 */
#pragma once
namespace tarmac::DataAcquisition {
/** @brief Borrowed operational observer; implementations own any diagnostic state.
 * Invoked synchronously on the service caller's thread. Must not throw, reenter
 * the service or retain references to it. No global state or implicit threading.
 * Completion reports device-read completion, not persistence or durable delivery.
 */
class AcquisitionMonitor {
public:
    /** @brief Release observer resources without throwing. */
    virtual ~AcquisitionMonitor() = default;
    /** @brief Observe a successfully returned device sample before caller transfer. */
    virtual void read_completed() noexcept = 0;
    /** @brief Observe an exception from device read, before that exception propagates. */
    virtual void read_failed() noexcept = 0;
};
}
