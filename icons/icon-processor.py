import os
from PIL import Image

def process_image(file_path):
    with Image.open(file_path) as img:
        img = img.convert("RGBA")
        data = img.getdata()

        # Get bounding box of non-transparent pixels
        bbox = img.getbbox()
        if not bbox:
            return  # Fully transparent image

        cropped = img.crop(bbox)

        # Determine square size
        size = max(cropped.width, cropped.height)
        new_img = Image.new("RGBA", (size, size), (0, 0, 0, 0))

        # Center the cropped image
        offset_x = (size - cropped.width) // 2
        offset_y = (size - cropped.height) // 2
        new_img.paste(cropped, (offset_x, offset_y))

        new_img.save(file_path)

def scan_and_process(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.lower().endswith(".png"):
                full_path = os.path.join(root, file)
                process_image(full_path)

if __name__ == "__main__":
    dirs = [
        r".\tray",
        r".\taskbar"
    ]
    for d in dirs:
        scan_and_process(d)
