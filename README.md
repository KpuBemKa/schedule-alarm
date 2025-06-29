# Alarm scheduler
A simple project to automate scheduled relay open-closing. For example, but not limited to, a school bell.

# Framework
Developed using ESP-IDF version `5.4.1`.

# Features
- Sleep mode with a wake-up button;
- SNTP time syncronization;
- Remote schedule & settings update. Hosts a HTTP server with a website to update these;
- Changable settings: 
    - Remote Wi-Fi SSID & password;
    - Own Wi-Fi station SSID & password;
    - Device timezone;
- Reset settings to default. Press and hold the wake-up button for more than 10 seconds.
- Schedule points are defined by day minute;
- Schedule loops daily, for now.

# Website
Website source code can be found in `components/website/source`. `site_builder.py` in the `website` directory is a helper script to embed all `.css` & `.js` files included by `index.html` into a single `index.html` file, and then place it into `components/data_image` directory, which will be flashed onto the ESP32.

CMake is configured to run the `site_builder.py` script automatically before the build starts if any source files have been changed.

`website/source` directory also contains a `test.html` file, which runs some unit-tests on some components. Results are displayed in the console. It is not included in the final, merged website as it's not included by `index.html`