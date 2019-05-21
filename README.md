# Match SRTM3 data to real photo (Prototype!)

## Description

This is my experimental project to get a feeling of SRTM3 Digital Elevation data. As well as several other technologies (WebAssembly!).

The idea is to create digital representation of terrain that matches the real photo. So that you can know which moutains you see on the picture.

The image EXIF should countain GPS location information. Most modern cameras has build-in ability to place this info. Another way is to match the image timestamp to GPS track - there are utilities (e.g. gpsprune) which could update EXIF geolocation like that.

Digital elevation is stored as heightmap in SRTM tiles. My code generates 3D mesh with this info. It's very straightforward - just place the vertices in data points and connect them. Just add a little color and lighting to make the elevation more visible.

I target the images from photo cameras. Unlike phones, there's usually no information on orientation (and inclination!) of camera. So, to match the real picture, need to manually rotate the virtual camera. In some cases it should be possible to make this automatic using Computer Vision algorithms. But this certainly goes out of scope of my little experiment. And I believe there would be serious problems for CV to match the snow-covered mountaintops on the cloudy background.

## Illustration (example)
**Original (real) image:**
![Original picture](/home/centurn/_null/match-mountain/doc/img/source.png  "Original picture")

**3D terrain from SRTM3:**
![Generated terrain](/home/centurn/_null/match-mountain/doc/img/terrain.png  "Generated terrain")

**Overlay of image over generated terrain:**
![Match](/home/centurn/_null/match-mountain/doc/img/match.png  "Match")

## What's done
1. Load an image, get geo-location from EXIF, generate 3D terrain around it
2. Initialize virtual camera parameters to approximate the real camera FOV.
3. Overlay real image on top of virtual, toggle transparency and visibility: I, O
4. Incline the image (some are misalignes): with right mouse button
5. Rotate, move around the virtual terrain: WASD to move, QE for up/down, hold mouse to pan.
6. Virtual 'sun' (light) rotation toggle: L
7. Change the image: V
8. Adjust virtual camera FOV: mouse wheel
9. Load GPX track and display if over 3D terrain. As of now, it's ugly - need to update drawing code. So it's disabled by default.
10. Imgui integrated. But no real use yet.
11. Builds as native (SDL) application, as well as in Browser (WebAssembly)

## What's not done / limitations
1. No auto-download of SRTM3 files. Necessary should be placed manually to assets folder.
2. No auto-match for image rotation.
3. EXIF usually does not have enough information on camera FOV. There's info on focal distance, but not on senor size. Some cameras put info on normalized focal distance, some not. There are open databases on sensor size, but they are incomplete. So, I make a guess on FOV, but it's unreliable. It's possible to manually adjust.
4. Only one SRTM tile is processed. It's not enough for a general case (see conclusions below)
5. WebAssembly (actually web itself) has quite different approach on file handling. I haven't implemented this properly. So, now it can process only those files that are placed in the pre-loaded data bundle.

## Conclusions

1. For a Hills on the middle (several kilometers) distance, there's usually a good match of virtual terrain to real data. I was really surprised to see near-perfect match for such a straightforward terrain generation
2. For relief features on near distance, it's unreliable. SRTM3 resolution (90m) is not enough. Maybe SRTM1 is better.
3. For distant mountains, current approach is not enough. Since as of now I only process one tile per picture, big mountain sometimes just go out of scope. Also, on a scale of dozens kilometers, the earth curvature comes into play. And probably optical effects in the atmosphere on the bigger distance. In reality, the mountain ranges could be seen from more than 200 kilometers of distance! This implies that I have to re-write the generation algorithm to a more complex geometry model, add LODs.. and probably asynchronous loading.
4. WebAssembly technology is usable already. The performance is good (but CPU tasks is still several times slower than native code!). And there's lots of inconveniences for cross-plafrorm code. For real application, need to write too much browser-specific code.

## Build

**For web (needs emcripten installed and configured)**
source web_env.sh
./cmake.sh
make
./run.sh
In browser - open http://localhost:8088

**Native: standard cmake**
mkdir buid
cd build
cmake ..
make
./bin/match-mountain

Or just use the IDE with cmake support
