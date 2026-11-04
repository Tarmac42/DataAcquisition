/** @file DeviceAdapter.hpp
 * @brief Typed synchronous device-read extension point without hardware drivers.
 */
#pragma once
namespace tarmac::acquisition {
/** @brief Borrowable adapter for one concrete sample type.
 * @tparam Sample Owning application-defined sample value.
 * Concrete adapters own hardware resources via RAII. Reads are synchronous and
 * non-reentrant; callers serialize access. Cancellation, deadlines and driver
 * permissions require a later contract; this interface promises no bounded read time.
 */
template<class Sample>
class DeviceAdapter {
public:
    /** @brief Release adapter resources without throwing. */
    virtual ~DeviceAdapter() = default;
    /** @brief Read one owning sample; never return borrowed device buffers.
     * @return Owned sample with adapter-defined units and timestamp semantics.
     * @throws Error On device failure; std::bad_alloc and sample errors may propagate.
     * No automatic retry or hardware control action is implied.
     */
    virtual Sample read() = 0;
};
}
