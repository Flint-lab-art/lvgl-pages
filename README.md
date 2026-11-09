# lvgl-pages

在没有显示器的机器上渲染 LVGL 界面，并用 GitHub Actions 自动构建 + 发布到 GitHub Pages。

## 目录

```
demo/main.c          离屏渲染演示：480×800 温控面板，导出 raw 帧
demo/CMakeLists.txt  通过 -DLVGL_DIR 指向 LVGL 源码
demo/lv_conf.h       最小化 LVGL 配置（ARGB8888、Montserrat 字体）
scripts/raw2png.py   raw(ARGB8888) -> PNG / GIF
lvgl-panel.html      展示页（CI 里会复制成 index.html）
```

截图与动图不提交到仓库，由 CI 每次构建时现场生成。

## 本地跑一次

```bash
git clone --depth 1 --branch v9.3.0 https://github.com/lvgl/lvgl.git third_party/lvgl
mkdir -p frames
cmake -S demo -B build -DCMAKE_BUILD_TYPE=Release -DLVGL_DIR="$PWD/third_party/lvgl"
cmake --build build -j
./build/render            # 产出 frame.raw + frames/*.raw
python3 scripts/raw2png.py
```

依赖：gcc、cmake、python3 + pillow + numpy。**不需要 SDL，也不需要显示器。**

## CI

`.github/workflows/pages.yml`：push 到 `main` 后编译、渲染、转图，然后部署 Pages。

首次使用需要在仓库 **Settings → Pages → Build and deployment → Source 选 GitHub Actions**。
