#!/usr/bin/env python3

from ws2811_decoder import Decoder
import numpy as np
import cv2

SAMPLERATE = 24000000

WIDTH = 16
HEIGHT = 16
SCALE = 40

frames = []
cur_frame = []

def put_data(sample_start, sample_end, type, value):
    global cur_frame

    if type == "RESET":
        frames.append(cur_frame)
        cur_frame = []
    elif type == "COLOR":
        cur_frame.append(value)

def pixel_to_index(x, y):
    if x % 2 != 0:
        y = (HEIGHT-1) - y;

    lednum = y + x * HEIGHT;

    return lednum

def frame_to_image(frame):
    image = np.zeros((HEIGHT, WIDTH, 3), np.uint8)
    for x in range(WIDTH):
        for y in range(HEIGHT):
            rgb = frame[pixel_to_index(x, y)]
            image[y, x, 0] = rgb & 0xFF
            image[y, x, 1] = (rgb >> 8) & 0xFF
            image[y, x, 2] = (rgb >> 16) & 0xFF

    return image

def main():
    data = []

    print("Reading file...")
    with open("data.csv", 'r') as f:
        for line in f:
            if not line.startswith("#"):
                samplenum, sample = line.strip().split(", ")

                samplenum = int(samplenum)
                sample = sample == "1"

                data.append((samplenum, sample))

    print("Decoding...")
    d = Decoder(SAMPLERATE, put_data)
    d.decode(data)

    print("Got {} frames".format(len(frames)))

    fourcc = cv2.VideoWriter_fourcc(*'MJPG')
    out = cv2.VideoWriter('output.avi', fourcc, 100.0, (SCALE*HEIGHT, SCALE*WIDTH))

    for f in frames:
        if len(f) == WIDTH*HEIGHT:
            image = frame_to_image(f)
            image = cv2.resize(image, (SCALE*HEIGHT, SCALE*WIDTH), interpolation=cv2.INTER_NEAREST)
            out.write(image)

    out.release()

if __name__ == '__main__':
    main()
