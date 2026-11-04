/** @file AcquisitionService.hpp
 * @brief One-shot acquisition coordination independent of input and storage adapters.
 */
#pragma once
#include <tarmac/DataAcquisition/config/SamplingConfig.hpp>
#include <tarmac/DataAcquisition/devices/DeviceAdapter.hpp>
#include <tarmac/DataAcquisition/monitoring/AcquisitionMonitor.hpp>
namespace tarmac::DataAcquisition {
/** @brief Explicitly composed synchronous acquisition operation.
 * @tparam Sample Owning typed sample supplied by a device adapter.
 * Borrows device and monitor, both of which must outlive this service and all calls.
 * Configuration is an owned snapshot. Calls must not overlap or reenter. No thread,
 * schedule, persistence, automatic retry or control/safety responsibility is owned.
 */
template<class Sample>
class AcquisitionService final {
public:
    /** @brief Validate configuration before any device I/O.
     * @param device Borrowed adapter. @param monitor Borrowed nonthrowing observer.
     * @param config Configuration snapshot; interval is intent for future scheduling.
     * @throws std::invalid_argument On invalid configuration.
     */
    AcquisitionService(DeviceAdapter<Sample>& device, AcquisitionMonitor& monitor,
                       SamplingConfig config = {})
        : device_(device), monitor_(monitor), config_(config) { validate(config_); }
    /** @brief Request exactly one sample, reporting read outcome and propagating errors.
     * @return Owned typed sample. No persistence or scheduling is performed.
     * @throws Error On device failure; original exceptions propagate unchanged.
     * Allocation/sample transfer errors may also propagate. Monitoring reports the
     * read itself; a later throwing move to the caller does not undo read completion.
     */
    Sample sample_once() {
        auto sample = [&]() {
            try { return device_.read(); }
            catch (...) { monitor_.read_failed(); throw; }
        }();
        monitor_.read_completed();
        return sample;
    }
    /** @brief Copy immutable sampling intent; no runtime scheduler is implied.
     * @return Configuration snapshot.
     */
    SamplingConfig sampling_config() const noexcept { return config_; }
private:
    DeviceAdapter<Sample>& device_;
    AcquisitionMonitor& monitor_;
    SamplingConfig config_;
};
}
