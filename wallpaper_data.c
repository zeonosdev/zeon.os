#ifndef WALLPAPER_DATA_H
#define WALLPAPER_DATA_H

// Ukuran Gambar Wallpaper (Contoh 320x240 piksel)
#define BG_WIDTH  320
#define BG_HEIGHT 240

// Data piksel warna dalam format 0x00RRGGBB (Contoh array piksel)
// Di real project, array ini berisi ribuan/jutaan kode warna gambar lo
const UINT32 wallpaper_pixels[BG_WIDTH * BG_HEIGHT] = {
    0x000000FF, 0x000000FF, 0x00112233, // ... data warna piksel gambar
};

#endif
