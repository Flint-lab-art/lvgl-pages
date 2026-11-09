"""把 LVGL 离屏渲染导出的 raw 帧转成 PNG / GIF。

main.c 按 ARGB8888 写出 frame.raw（BGRA 字节序，480×800），
frames/ 目录下是滑杆动画的逐帧序列。
"""
import glob
import os

import numpy as np
from PIL import Image

W, H = 480, 800


def load(path):
    raw = np.fromfile(path, dtype=np.uint8).reshape(H, W, 4)
    return Image.fromarray(raw[:, :, :3][:, :, ::-1], "RGB")  # BGRA -> RGB


img = load("frame.raw")
img.save("lvgl-demo.png")

seq = sorted(glob.glob("frames/*.raw"))
if seq:
    frames = [load(f).convert("P", palette=Image.ADAPTIVE, colors=64) for f in seq]
    frames[0].save(
        "lvgl-demo.gif", save_all=True, append_images=frames[1:], duration=120, loop=0
    )

print("png:", os.path.getsize("lvgl-demo.png"), "gif frames:", len(seq))
