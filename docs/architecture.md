# DataAcquisition architecture foundation

DataAcquisition owns sensor/device acquisition. It does not own archive folder
watching or ingestion (ArchiveManager), nor VanBuild control or safety decisions.
It remains independent of both applications. DataCore is the intended future shared
library; no application dependency, submodule or implicit source fetch is present.

## Present boundaries

| Component | Responsibility | Ownership |
| --- | --- | --- |
| DeviceAdapter<Sample> | Synchronous typed read extension point | Future concrete adapters own handles; service borrows adapter |
| AcquisitionService<Sample> | Validate configuration and coordinate one explicit read | Borrows device and monitor; owns configuration snapshot |
| AcquisitionMonitor | Nonthrowing synchronous read-outcome observations | Composition owns observer; implementation owns monitoring state |
| SamplingConfig | Positive nanosecond interval intent | Copied immutable-by-service value |
| CLI | Help and unsupported-argument handling | No devices or services constructed yet |

Composition must construct device and observer before service and destroy the
service first. Dependencies and all calls stay on a single externally serialized
thread. Borrowed references must remain valid throughout a call; observers cannot
reenter the service. No global service owner, base Service class, registry, device
selection enum or event bus is introduced.

sample_once() performs exactly one explicit read, reports read failure before
rethrowing the original exception, or reports read completion before returning an
owning sample. Read completion is not successful persistence or durable delivery.
If a sample's later move to the caller throws, the read-completed observation is
still accurate. Read adapters define units and timestamp provenance. No retry,
background work, sleep, resource transfer or hardware control occurs automatically.
A configuration interval expresses intent only; it does not promise a running timer.

Error carries device/acquisition diagnostics. Configuration validation throws
std::invalid_argument before device access; allocation and sample exceptions can
propagate. Observers are noexcept so diagnostic reporting cannot mask read failures.
Adapters release resources through RAII; cleanup must not throw. There is currently
no cancellation/deadline contract or upper bound on synchronous device reads.

## Future DataCore integration

Applications will supply concrete owning record types, optionally using
DataCore's RecordEnvelope. A composition-layer adapter can submit returned records
to DataCore::RecordWriter<Record>. Acquisition should not decide which CSV/JSON/
SQLite writer to use or inherit storage concerns. Future CMake integration should
use an explicitly installed find_package(DataCore CONFIG REQUIRED) target, without
submodules or implicit downloads. Do not duplicate DataCore models here: the current
Sample template accepts an ordinary externally defined value.

Acquire and persist are distinct outcomes. A later design must define backpressure,
delivery acknowledgment, failed-write handling and shutdown before connecting a
continuous acquisition loop to writers. DataCore must remain independent of this
application. ArchiveManager and VanBuild integration requires separate approval.

## Deferred decisions

Before implementing drivers or periodic execution, review device resource
construction, timeout/cancellation guarantees, monotonic scheduling and missed-tick
policy, timestamp/clock provenance, configuration file format and reload behavior,
monitoring snapshots/events and failure escalation. A daemon needs separately
reviewed thread ownership, graceful stop/join and interruption behavior. No hardware
drivers, config parser, timers, persistent queue, web/API or daemon exist today.

Tests use isolated device/monitor doubles to verify configuration-before-I/O,
configuration snapshot ownership, one read per request, no implicit retry, unchanged
exception diagnostics and explicit recovery via a later call. CLI tests exercise
help/invalid arguments; standalone compilation checks each public header. Linux
and macOS workflows run CMake and CTest. No physical hardware is used in CI.
