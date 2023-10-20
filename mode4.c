#include "mode4.h"
#include "font.h"

// Sets a pixel in mode 4
void setPixel4(int x, int y, unsigned char colorIndex) {
    unsigned short pixelData = videoBuffer[OFFSET(x, y, SCREENWIDTH) / 2];
    if (x & 1) {
        pixelData &= 0x00FF;
        pixelData |= (colorIndex << 8);
    } else {
        pixelData &= 0xFF00;
        pixelData |= colorIndex;
    }
    videoBuffer[OFFSET(x, y, SCREENWIDTH) / 2] = pixelData;
}

// Draws a rectangle in mode 4
void drawRectangle4(int x, int y, int width, int height, volatile unsigned char colorIndex) {
    volatile unsigned int color = colorIndex << 24 | colorIndex << 16 | colorIndex << 8 | colorIndex;
    if (width == 1) {
        setPixel4(x, y, colorIndex);
    } else if (width == 2) {
        setPixel4(x, y, colorIndex);
        setPixel4(x + 1, y, colorIndex);
    }
    else if ((width & 1) && (x & 1)) {
        for (int i = 0; i < height; i++) {
            setPixel4(x, y + i, colorIndex);
            DMANow(3, &color, &videoBuffer[OFFSET(x + 1, y + i, SCREENWIDTH) / 2], DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | (width / 2));
        } 
    } else if (x & 1) {
        for (int i = 0; i < height; i++) {
            setPixel4(x, y + i, colorIndex);
            DMANow(3, &color, &videoBuffer[OFFSET(x + 1, y + i, SCREENWIDTH )/ 2], DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | ((width - 2) / 2 ));
            setPixel4(x + width - 1, y + i, colorIndex);
        }
    } else if (width & 1) {
        for (int i = 0; i < height; i++) {
            DMANow(3, &color, &videoBuffer[OFFSET(x, y + i, SCREENWIDTH) / 2], DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | ((width - 1) / 2));
            setPixel4(x + width - 1, y + i, colorIndex);
        }
    } else {
        for (int i = 0; i < height; i++) {
            DMANow(3, &color, &videoBuffer[OFFSET(x, y + i, SCREENWIDTH) / 2], DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT |( width / 2));
        }
    }
}

// Fills the screen in mode 4 using specified palette index
void fillScreen4(volatile unsigned char colorIndex) {
    volatile unsigned short pixelData = colorIndex | (colorIndex << 8);
    DMANow(3, &pixelData, videoBuffer, DMA_SOURCE_FIXED | ((SCREENHEIGHT * SCREENWIDTH) / 2)); 
}

// Draws an image in mode 4
void drawImage4(int x, int y, int width, int height, const unsigned short *image) {
    for(int i = 0; i < height; i++){
        DMANow(3, &image[OFFSET(0, i, width / 2)], &videoBuffer[OFFSET(x, y + i, SCREENWIDTH) / 2], width / 2);
    }
}

// Draws a full-screen image in mode 4
void drawFullscreenImage4(const unsigned short *image) {
    DMANow(3, image, videoBuffer, SCREENWIDTH * SCREENHEIGHT / 2);
}

// Flips page being displayed and page being drawn to
void flipPage() {
    if (REG_DISPCTL & DISP_BACKBUFFER) {
        videoBuffer = BACKBUFFER;
    } else {
        videoBuffer = FRONTBUFFER;
    }
    REG_DISPCTL ^= DISP_BACKBUFFER;
}

// Draws the specified character at the specified location in Mode 4
void drawChar4(int x, int y, char ch, unsigned short color) {
    for (int c = 0; c < 6; c++) {
        for (int r = 0; r < 8; r++) {
            if (fontdata_6x8[(48 * ch + c + 6 * r)]) setPixel4(x + c, y + r, color);
        }
    }
}

// Draws the specified string at the specified location in Mode 3
void drawString4(int x, int y, char *str, unsigned short color) {
    while (*str) {
        drawChar4(x, y, *str, color);
        str++;
        x += 6;
    }
}