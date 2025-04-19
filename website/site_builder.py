from bs4 import BeautifulSoup
import os
import sys

if len(sys.argv) != 3:
    print("Usage: python site_builder.py <source.html> <output.html>")
    sys.exit(1)

html_path = sys.argv[1]
output_path = sys.argv[2]
base_dir = os.path.dirname(html_path)

# Load the original HTML
with open(html_path, "r", encoding="utf-8") as f:
    soup = BeautifulSoup(f, "html.parser")

# Embed all <link rel="stylesheet" href="...">
for link in soup.find_all("link", rel="stylesheet", href=True):
    css_file = os.path.join(base_dir, link["href"])
    try:
        with open(css_file, "r", encoding="utf-8") as f:
            css_content = f.read()
        style_tag = soup.new_tag("style")
        style_tag.string = css_content
        link.replace_with(style_tag)
        print(f"[*] Embedded CSS: {link['href']}")
    except FileNotFoundError:
        print(f"[x] CSS file not found: {css_file}")

# Embed all <script src="...">
for script in soup.find_all("script", src=True):
    js_file = os.path.join(base_dir, script["src"])
    try:
        with open(js_file, "r", encoding="utf-8") as f:
            js_content = f.read()
        new_script = soup.new_tag("script")
        new_script.string = js_content
        script.replace_with(new_script)
        print(f"[*] Embedded JS: {script['src']}")
    except FileNotFoundError:
        print(f"[x] JS file not found: {js_file}")

# Save modified HTML
with open(output_path, "w", encoding="utf-8") as f:
    f.write(str(soup.prettify()))

print(f"Done! Saved to: {output_path}")
