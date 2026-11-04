/** @file SamplingConfig.hpp
 * @brief Validated sampling intent, separate from future timers and configuration parsing.
 */
#pragma once
#include <chrono>
namespace tarmac::acquisition {
/** @brief Value configuration for a future periodic sampling adapter.
 * Interval uses a monotonic duration, not UTC time. This value creates no scheduler,
 * sleeps or background thread. Services store a copy; updates require reconstruction.
 */
struct SamplingConfig {
    std::chrono::nanoseconds interval{std::chrono::seconds{1}};
};
/** @brief Reject nonpositive sampling intervals before any device operation.
 * @param config Borrowed immutable configuration for this call.
 * @throws std::invalid_argument If interval is zero or negative.
 */
void validate(const SamplingConfig& config);
}
