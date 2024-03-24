#!/usr/bin/env python

from PIL import Image

# Create a new image with mode 'RGB' and size 4x5
img = Image.new('RGB', (4, 5), color='white')

#paint columns red green blue yellow
for i in range(4):
    for j in range(5):
        if i == 0:
            img.putpixel((i, j), (255, 0, 0))
        elif i == 1:
            img.putpixel((i, j), (0, 255, 0))
        elif i == 2:
            img.putpixel((i, j), (0, 0, 255))
        elif i == 3:
            img.putpixel((i, j), (255, 255, 0))

# Save the image
img.save('Images/4x5.png')

# Optional: Display the image
img.show()
