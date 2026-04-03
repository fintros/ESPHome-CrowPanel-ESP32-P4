# CrowPanel Advanced ESP32-P4 ESPHome build and installation guide

This document explains how to create a Python virtual environment, install ESPHome, edit the YAML variables, build the firmware, flash the device, connect it to Wi‑Fi through the captive portal if needed, adopt it in Home Assistant, and includes Home Assistant WebView addon setup.

This guide is checked on the 10 inch ESP32-P4 Elecrow display: https://www.elecrow.com/crowpanel-advanced-10-1inch-esp32-p4-hmi-ai-display-1024x600-ips-touch-screen-wifi-6.html, but os also should works on smaller displays.

## Requirements

- Python 3.11 or newer is recommended for current ESPHome installations.
- A USB cable that supports data, not just charging.
- Access to the YAML configuration file, for example `CrowPanelAdv10.yaml`.
- Home Assistant OS with the ESPHome integration enabled or available for adoption over the native API.

## Create a virtual environment

Using a Python virtual environment is the recommended way to install ESPHome with pip.

### Linux and macOS

```bash
python3 -m venv venv
source venv/bin/activate
python -m pip install --upgrade pip wheel
pip install esphome
esphome version
```

### Windows PowerShell

```powershell
py -m venv venv
.\venv\Scripts\Activate.ps1
python -m pip install --upgrade pip wheel
pip install esphome
esphome version
```

Do not use `sudo pip install esphome`.

## Prepare the YAML file

Edit the `substitutions:` block in the YAML file before building so the device has the correct Wi‑Fi and Home Assistant API settings.

### Variables to update

Replace the example values with your own values:

```yaml
substitutions:
  wifi_ap: "YOUR_WIFI_SSID"
  wifi_password: "YOUR_WIFI_PASSWORD"
  fallback_ap: "crowpanel-esp32-p4"
  fallback_ap_password: "CHANGE_ME"
  ha_base_url: "http://homeassistant.local:8123/"
  ha_encryption_key: "GENERATED_API_KEY"
```

### What each variable means

| Variable | Purpose |
|---|---|
| `wifi_ap` | Main Wi‑Fi SSID used by the device to join your network. |
| `wifi_password` | Password for the main Wi‑Fi network. |
| `fallback_ap` | SSID of the fallback access point created if normal Wi‑Fi fails. |
| `fallback_ap_password` | Password for the fallback configuration AP. |
| `ha_base_url` | Base Home Assistant URL used as the initial Remote WebView target. |
| `ha_encryption_key` | Native API encryption key used by ESPHome to connect securely to Home Assistant. |

### Notes

- Keep the YAML syntax strict: use normal quoted strings, not Markdown links or rich text formatting.
- The `api:` section is required for Home Assistant connectivity.
- The `ota:` section is required for later over-the-air updates.
- The `captive_portal:` section allows temporary local configuration through the fallback Wi‑Fi hotspot.

## Build the firmware

From the directory that contains your YAML file, run:

```bash
esphome config CrowPanelAdv10.yaml
esphome compile CrowPanelAdv10.yaml
```

`esphome config` validates the YAML and catches syntax problems before compilation, while `esphome compile` builds the firmware image for the target board.

## Flash the device

For the first upload, connect the board over USB and flash it directly from the command line.

### Find the serial port

Common serial device examples:

- Linux: `/dev/ttyUSB0` or `/dev/ttyACM0`
- macOS: `/dev/cu.usbserial-*`
- Windows: `COM3`, `COM4`, and similar

### Flash command

```bash
esphome run CrowPanelAdv10.yaml
```

This command validates, compiles if needed, uploads the firmware, and then opens logs if the device comes online.

If you want to explicitly select a port, use:

```bash
esphome run CrowPanelAdv10.yaml --device /dev/ttyUSB0
```

After the first successful flash, later updates can usually be done over Wi‑Fi using OTA as long as the device remains reachable on the network and the `ota:` section stays enabled.

## Connect to Wi‑Fi with the captive portal

If the main Wi‑Fi credentials are missing or incorrect, the device can start its fallback access point because your YAML includes both `ap:` under `wifi:` and `captive_portal:`.

### How it works

When the board cannot connect to the configured Wi‑Fi network, it creates its own temporary Wi‑Fi network using these values:

```yaml
wifi:
  ap:
    ssid: $fallback_ap
    password: $fallback_ap_password

captive_portal:
```

### Connect through the captive portal

1. Power on the device after flashing.
2. Wait for it to fail joining the main Wi‑Fi, if the credentials are wrong or not yet configured.
3. On a phone or laptop, search for the fallback Wi‑Fi network, for example `crowpanel-esp32-p4`.
4. Connect to that Wi‑Fi network using the fallback AP password from the YAML file.
5. A captive portal page may open automatically. If it does not, open a browser and try `http://192.168.4.1`.
6. Enter the correct Wi‑Fi SSID and password for your normal network.
7. Save the settings and wait for the device to reboot or reconnect.
8. Once connected, the fallback AP should disappear and the board should join your normal Wi‑Fi network.

### When to use the captive portal

Use it when:

- the main Wi‑Fi password changed,
- the device was moved to another network,
- the SSID in YAML was incorrect,
- or you want a quick recovery path without reflashing.

## Connect the device to Home Assistant

Once flashed and connected to Wi‑Fi, the device should appear for adoption in Home Assistant through ESPHome’s native API if the `api:` section is present and the encryption key matches the Home Assistant-side configuration.

### Adoption steps

1. Wait for the board to boot and join Wi‑Fi.
2. Open Home Assistant.
3. Go to **Settings → Devices & Services**.
4. If Home Assistant discovers the device automatically, open the discovered ESPHome device and finish setup.
5. If it does not appear automatically, add the ESPHome integration manually and enter the device IP address or hostname.
6. Provide the API encryption key if Home Assistant asks for it.

### After adoption

- The entities defined in your YAML should appear in Home Assistant after the device is added through ESPHome’s API.
- OTA updates can then be initiated from ESPHome tooling or from the ESPHome Dashboard workflow if you use it.

## Typical command sequence

```bash
python3 -m venv venv
source venv/bin/activate
python -m pip install --upgrade pip wheel
pip install esphome
esphome version
esphome config CrowPanelAdv10.yaml
esphome compile CrowPanelAdv10.yaml
esphome run CrowPanelAdv10.yaml
```

## Install and configure RemoteWebViewServer in Home Assistant

RemoteWebViewServer is a headless Chromium-based server that renders Home Assistant dashboards and streams changed screen areas over WebSocket to the ESPHome `remote_webview` client. The project repository includes Home Assistant add-on files, and the upstream documentation notes that the HA OS add-on can expose an optional debug proxy and uses WebSocket port `8081` for the stream.

### Add the repository to Home Assistant

1. Open Home Assistant.
2. Go to **Settings → Add-ons → Add-on Store**.
3. Open the three-dot menu in the upper right corner.
4. Choose **Repositories**.
5. Add the repository URL for the RemoteWebViewServer project:

```text
https://github.com/fintros/RemoteWebViewServer
```

6. Close the dialog and wait for Home Assistant to refresh the add-on list.

If your Home Assistant installation does not immediately show the add-on, try the upstream repository instead, because the public documentation and released images are currently published from the `fintros` project namespace.

```text
https://github.com/fintros/RemoteWebViewServer
```

### Install the add-on

1. Find **Remote WebView Server** in the add-on store.
2. Open it and click **Install**.
3. After installation, enable:
   - **Start on boot**
   - **Watchdog**
4. Do not start it yet if you want to review the configuration first.

### Basic configuration

The upstream project documents these core settings and defaults for the server container:

- WebSocket stream port: `8081`
- Optional external DevTools proxy: `9222`
- Internal health endpoint: `18080`
- Typical browser locale setting: `en-US`
- Persistent browser profile directory: `/pw-data`

For a normal Home Assistant dashboard setup, verify or set these values in the add-on configuration if the add-on exposes them:

```yaml
ws_port: 8081
browser_locale: en-US
prefers_reduced_motion: false
expose_debug_proxy: false
```

If the add-on exposes advanced tuning parameters, the upstream example uses values similar to these:

```yaml
tile_size: 32
full_frame_tile_count: 4
full_frame_area_threshold: 0.5
full_frame_every: 50
every_nth_frame: 1
min_frame_interval_ms: 80
jpeg_quality: 85
max_bytes_per_message: 14336
```

### Start the add-on

1. Start the add-on.
2. Open the **Logs** tab.
3. Confirm that the service started successfully and is listening on the configured WebSocket port.
4. If you enabled the debug proxy, confirm that DevTools proxy access is also exposed.

### Log in to Home Assistant inside the remote browser

RemoteWebViewServer opens pages in a headless Chromium session. If the dashboard requires authentication, you usually need to log in once inside that browser session so it can keep cookies in its persistent profile directory.

If the add-on exposes the debug proxy feature:

1. Enable `expose_debug_proxy` in the add-on configuration.
2. Restart the add-on.
3. In Chrome, open `chrome://inspect/#devices`.
4. Add `HOME_ASSISTANT_HOST:9222` as an inspect target.
5. Open the remote Chromium tab and sign in to Home Assistant.
6. After successful login, the session should remain stored in the add-on browser profile.

### Point the ESPHome device to the server

Your ESPHome YAML should point `remote_webview.server` to the Home Assistant host on port `8081`, because upstream examples and the server documentation use that port for the WebSocket tile stream.

Example:

```yaml
remote_webview:
  id: rwv
  server: homeassistant.local:8081
  url: http://homeassistant.local:8123/
```

If `homeassistant.local` does not resolve reliably on the device, replace it with the Home Assistant LAN IP address.

### Verify the connection

After the add-on is running and the ESP device is online:

1. Open the device logs in ESPHome.
2. Confirm that the device connects to the RemoteWebViewServer endpoint.
3. Set or update the `rwv_url` text entity in Home Assistant.
4. Check that the screen loads the requested Home Assistant dashboard.
5. Test touch input and scrolling.


## Troubleshooting

### YAML syntax errors

Use `esphome config CrowPanelAdv10.yaml` first; it is the fastest way to catch invalid YAML or malformed lambda code before a full compile.

### Device does not appear in Home Assistant

Check these items first:

- The device joined the expected Wi‑Fi network.
- `api:` is present in the YAML.
- The encryption key in Home Assistant matches `ha_encryption_key` in the YAML.
- The board is reachable by IP on your LAN.

### `esphome` command not found

Make sure the virtual environment is activated before running ESPHome commands.

### Troubleshooting RemoteWebViewServer

#### The add-on starts but the screen stays blank

Check these items first:

- The server host in `remote_webview.server` is correct.
- Port `8081` is reachable from the ESP device.
- The dashboard URL in `rwv_url` is valid.
- The headless browser is logged into Home Assistant.
- Home Assistant is not forcing a login page that the browser session has not completed yet.

#### `homeassistant.local` does not work from ESPHome

mDNS resolution can be unreliable across some networks and Wi‑Fi stacks. In that case, use the Home Assistant IP address in both `ha_base_url` and `remote_webview.server`.

#### Debugging with Chrome DevTools

The upstream documentation explains that DevTools access works by exposing port `9222`, then adding `hostname_or_ip:9222` in Chrome’s inspect devices page.
