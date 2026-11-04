# DataAcquisition

Standalone C++20 acquisition foundation in `tarmac::acquisition`, with no hardware
drivers, daemon, scheduler or production storage dependency.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/data-acquisition --help
```

The executable presents foundation usage only. It constructs no devices or services.
Unknown arguments return exit status 2; help performs no acquisition.

- `devices/DeviceAdapter.hpp`: typed synchronous device boundary.
- `services/AcquisitionService.hpp`: explicit one-shot coordination.
- `monitoring/AcquisitionMonitor.hpp`: synchronous read-outcome observation.
- `config/SamplingConfig.hpp`: validated sampling intent, without a timer.
- `Error.hpp`: acquisition-domain diagnostic boundary.
- `tests/`: isolated fake-device/observer and CLI contracts; no hardware access.

Linux and macOS GitHub Actions build and run CTest. DataCore integration is planned,
but no dependency is fetched or linked yet. ArchiveManager and VanBuild are never
dependencies. See [architecture](docs/architecture.md). Stop for API review before
adding hardware, scheduling, persistence or daemon functionality.
No license has been selected; this foundation does not grant a license.
