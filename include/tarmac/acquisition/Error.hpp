/** @file Error.hpp
 * @brief Acquisition-domain error boundary without application dependencies.
 */
#pragma once
#include <stdexcept>
namespace tarmac::acquisition {
/** @brief Owning diagnostic for device or acquisition failures.
 * Thread-independent values; adapters may derive specific error types. Allocation
 * and configuration errors remain distinguishable and are never silently retried.
 */
class Error : public std::runtime_error {
public:
    /** @brief Own an acquisition diagnostic.
     * @param message Terminated human-readable diagnostic.
     * @throws std::bad_alloc If diagnostic allocation fails.
     */
    explicit Error(const char* message) : std::runtime_error(message) {}
};
}
