# DataAcquisition

**DataAcquisition** is an independent C++20 foundation for collecting measurements from sensors, instruments, and other data-producing devices. Its architecture separates device access, acquisition coordination, sampling intent, and monitoring from storage and archive-management responsibilities.

> **Status: architectural foundation (v0.1.0).** The current executable is a CLI scaffold. No production device drivers, periodic scheduler, acquisition daemon, or storage backend have been implemented.

## What exists today

- `tarmac::DataAcquisition` namespace and typed C++20 interfaces.
- `devices/DeviceAdapter.hpp`: synchronous, typed device-read extension point.
- `services/AcquisitionService.hpp`: explicit, one-shot acquisition coordination.
- `monitoring/AcquisitionMonitor.hpp`: nonthrowing observation of read outcomes.
- `config/SamplingConfig.hpp`: validated sampling-interval configuration, **not** a running timer.
- `Error.hpp`: acquisition-domain diagnostic type.
- `data-acquisition` executable with help and invalid-argument handling.
- Fake-device/observer contract tests, standalone-header checks, and Linux/macOS CI.

Calling the present CLI does **not** initiate device collection. It constructs no hardware adapters or acquisition services.

## Separation of responsibilities

```text
Sensors / instruments
         |
         v
DataAcquisition
  device adapter -> acquisition service -> typed sample
                                           |
                                           v
                                optional future DataCore writer
                                           |
                                           v
                                   CSV / JSON / SQLite
                                           |
                                           v
                          ArchiveManager may ingest output files
```

This diagram is a **future integration concept**, not an implemented pipeline.

- **DataAcquisition** owns sensors, device protocols, sampling, and measurement monitoring.
- **DataCore** will provide reusable typed data and writer contracts; integration is planned but not linked today.
- **ArchiveManager** independently watches locations and catalogs or ingests produced files. Acquisition does not perform archival ingestion.
- **VanBuild** remains an independent vehicle-control application with its own operational and safety responsibilities.

DataAcquisition depends on neither ArchiveManager nor VanBuild. Hardware control and archive management must not leak into this application's generic contracts.

See [Architecture](docs/architecture.md) for detailed ownership, error, and deferred design decisions.

## Build and test

Requires a C++20-capable compiler and CMake 3.20 or newer.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/data-acquisition --help
```

Currently, unknown CLI options return exit status 2. The tests exercise one-shot service contracts using isolated fakes; they require no sensors or hardware access.

## Ownership and execution rules

Composition owns the device adapter and monitor; `AcquisitionService` borrows them and owns a validated configuration snapshot. The existing service performs exactly one explicit synchronous read per `sample_once()`, with no background scheduling or retry. Operations are serialized and non-reentrant. Device adapters own their future hardware handles through RAII.

A completed read is **not** an acknowledgment of persistence or durable delivery. Before adding continuous sampling or writer integration, we must define backpressure, error recovery, delivery acknowledgments, cancellation, and shutdown semantics.

## Planned development

- Review device construction, timeouts, cancellation, and failure handling.
- Specify monotonic sampling schedules, missed-sample behavior, and timestamp provenance.
- Define configuration loading/reloading and monitoring output.
- Add explicitly chosen hardware adapters and tests.
- Integrate DataCore through a separately reviewed, pinned/installed CMake package.
- Consider a daemon only after its thread ownership, stop/join, and recovery model are established.

## Development and licensing

Follow [AGENTS.md](AGENTS.md). Keep changes in small, tested PRs. GitHub Actions builds and tests on Linux and macOS.

**License:** No license has been selected. The presence of source code does not itself grant permission to reuse or redistribute it.
