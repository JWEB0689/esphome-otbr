# ESPHome OpenThread Border Router packages

Shared Thread, Wi-Fi and system diagnostic entities for ESPHome 2026.11+.
Requires an ESP32 with ESP-IDF 5.5.5 or newer, Wi-Fi STA and OpenThread configured locally.

```yaml
packages:
  diagnostics:
    url: https://github.com/wardmatter/esphome-otbr
    ref: main
    files: [packages/common.yaml]
    refresh: 1d
```

Keep device name, hardware/flash size, Wi-Fi credentials, Thread dataset and API/OTA credentials in your local YAML and secrets.yaml. Never publish secrets.

Examples:
- `examples/esp32-c6-devkitc-1.yaml`: official DevKitC-1, documented 8MB flash, native radio.
- `examples/esp32-s3-h2.yaml`: user's 4MB S3/H2 board, external RCP at 460800 baud. Verify your particular hardware; do not assume all revisions have this capacity. H2 requires compatible RCP firmware separately.

## Diagnostics

Thread role, network name/channel, OMR IPv6, PAN IDs, RLOC16, TX/RX counters, retries/errors, attachment attempts, parent changes/RSSI; Wi-Fi connectivity, IP, SSID/BSSID and RSSI; API status, uptime, reset reason, heap, loop time, CPU temperature and ESPHome version. Parent RSSI only publishes while attached as a child. Thread network keys are not exposed.

## Optional Ethernet (commented out)

`packages/ethernet.yaml` is deliberately entirely commented out. For the S3/H2 W5500 daughterboard, copy it locally as `ethernet.yaml`, remove the leading comment prefixes from its YAML lines and include it as a separate package. Do not paste its top-level keys into the common file: package merging combines the network and entity sections safely.

The C6 example does not enable Ethernet. The S3 daughterboard GPIO assignments do not apply to C6.
Ethernet-first priority affects ordinary ESPHome traffic; current OTBR uses Wi-Fi STA as its backbone. Both interfaces must be enabled for automatic route failover. API Status reports API client connectivity, not Internet reachability. The preferred-interface sensor follows Ethernet-first connectivity, not end-to-end health.

## Install and verification

Create secrets for `wifi_ssid`, `wifi_password`, `thread_tlv`, `otbr_api_key` (32-byte base64) and `otbr_ota_password`. Copy the appropriate example into Device Builder, validate and compile it, then perform the first installation over USB. No fallback AP or captive portal: OTBR requires Wi-Fi STA enabled at boot. `force_dataset: true` reapplies your configured dataset at startup.

The common diagnostics were compiled and run on the user's S3/H2 setup before extraction; the C6 device configuration passed validation. Remote package integration is validated separately when consumed. Config validation alone is not a hardware test.

Sources: [ESPHome OpenThread](https://esphome.io/components/openthread/), [packages](https://esphome.io/components/packages/), [DevKitC-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html).
