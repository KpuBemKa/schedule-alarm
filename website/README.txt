This directory contains a website which will be hosted on the ESP32. It containts multiple files which is kinda a hastle to have on the ESP.

The python script `site_builder.py` embeds all the files included by `index.html` into the `index.html` itself, and places the output `index.html` into the `data_image/` directory.

For now, this script cannot embed files included by sub-files which are included by `index.html`, so it's advised to not include anything in the sub-files.