/** @file SamplingConfig.cpp
 * @brief Sampling configuration validation without I/O or device access.
 */
#include <tarmac/acquisition/config/SamplingConfig.hpp>
#include <stdexcept>
namespace tarmac::acquisition {
void validate(const SamplingConfig& config) {
    if (config.interval <= std::chrono::nanoseconds::zero())
        throw std::invalid_argument("Sampling interval must be positive");
}
}
