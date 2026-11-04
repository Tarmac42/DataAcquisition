/** @file main.cpp
 * @brief Foundation executable: usage only; no device or background operation.
 */
#include <iostream>
#include <string_view>
/** @brief Present the available foundation behavior.
 * @param argc Argument count. @param argv Borrowed process argument strings.
 * @return Zero for usage, two for unsupported arguments, one for output failure.
 */
int main(int argc, char** argv) {
    if (argc == 1 || (argc == 2 && std::string_view(argv[1]) == "--help")) {
        std::cout << "DataAcquisition foundation\nUsage: data-acquisition [--help]\n"
                     "No hardware drivers or background jobs are implemented.\n";
        return std::cout ? 0 : 1;
    }
    std::cerr << "Unsupported arguments; use --help\n";
    return 2;
}
