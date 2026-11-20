# ESPHome OpenThread Border Router packages

Shared Wi-Fi and OpenThread configuration plus Thread, Wi-Fi and system diagnostic entities for ESPHome 2026.11+.
Requires an ESP32 with ESP-IDF 5.5.5 or newer, local secrets for Wi-Fi and the Thread dataset.

```yaml
packages:
  diagnostics:
    url: https://github.com/wardmatter/esphome-otbr
    ref: main
    files: [packages/common.yaml]
    refresh: 1d
```

The package owns `wifi:` and the common `openthread:` settings, including `device_type: FTD`, `border_router: {}`, `tlv: !secret thread_tlv` and `force_dataset: true`. Secret references resolve from the consuming device’s local `secrets.yaml`; no secret values are published. Keep device name, hardware/flash size and API/OTA security local. The S3/H2 example adds only its hardware-specific `openthread.border_router.rcp` mapping, merged into the common settings. Never publish secrets.

Examples:
- `examples/esp32-c6-devkitc-1.yaml`: official DevKitC-1, documented 8MB flash, native radio.
- `examples/esp32-s3-h2.yaml`: user's 4MB S3/H2 board, external RCP at 460800 baud. Verify your particular hardware; do not assume all revisions have this capacity. H2 requires compatible RCP firmware separately.

## Diagnostics

Thread role, network name/channel, OMR IPv6, PAN IDs, RLOC16, TX/RX counters, retries/errors, attachment attempts, parent changes/RSSI; Wi-Fi connectivity, IP, SSID/BSSID and RSSI; API status, uptime, reset reason, heap, loop time, CPU temperature and ESPHome version. Parent RSSI only publishes while attached as a child. Thread network keys are not exposed.

## Optional Ethernet

`packages/ethernet.yaml` is a ready-to-use optional package for the S3/H2 W5500 daughterboard. Include it alongside the common package:

```yaml
packages:
  otbr:
    url: https://github.com/wardmatter/esphome-otbr
    ref: main
    files:
      - packages/common.yaml
      - packages/ethernet.yaml
    refresh: 1d
```

Omit `packages/ethernet.yaml` to leave Ethernet disabled. Package merging combines the network and entity sections safely.

The C6 example does not enable Ethernet. The S3 daughterboard GPIO assignments do not apply to C6.
Ethernet-first priority affects ordinary ESPHome traffic; current OTBR uses Wi-Fi STA as its backbone. Both interfaces must be enabled for automatic route failover. API Status reports API client connectivity, not Internet reachability. The preferred-interface sensor follows Ethernet-first connectivity, not end-to-end health.

## Install and verification

Create secrets for `wifi_ssid`, `wifi_password`, `thread_tlv`, `otbr_api_key` (32-byte base64) and `otbr_ota_password`. Copy the appropriate example into Device Builder, validate and compile it, then perform the first installation over USB. No fallback AP or captive portal: OTBR requires Wi-Fi STA enabled at boot. `force_dataset: true` reapplies your configured dataset at startup.

The common diagnostics were compiled and run on the user's S3/H2 setup before extraction; the C6 device configuration passed validation. Both the C6 and 4MB S3/H2 configurations fetched this remote common package and passed configuration validation in ESPHome Device Builder 2026.12.0-dev on November 20, 2026. Ethernet was disabled for these checks. Config validation alone is not a hardware test; this extraction has not been reflashed.

Sources: [ESPHome OpenThread](https://esphome.io/components/openthread/), [packages](https://esphome.io/components/packages/), [DevKitC-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html).
