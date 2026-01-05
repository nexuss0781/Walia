#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "web_surface.h"
#include "memory.h"
#include "vm.h"

// ==========================================
// SURFACE LIFECYCLE
// ==========================================

ObjSurface* walia_surface_new(uint32_t width, uint32_t height) {
    // 1. Calculate Aligned Stride
    // We ensure every scanline is a multiple of 64 bytes for AVX-512.
    uint32_t bytesPerLine = width * sizeof(WldPixel);
    uint32_t stride = (bytesPerLine + 63) & ~63;
    size_t totalBytes = stride * height;

    // 2. Allocate Descriptor on Sovereign Heap
    ObjSurface* surface = (ObjSurface*)reallocate(NULL, 0, sizeof(ObjSurface));
    surface->obj.type = OBJ_NATIVE;
    surface->obj.isMarked = false;
    surface->obj.next = vm.objects;
    vm.objects = (struct Obj*)surface;

    surface->width = width;
    surface->height = height;
    surface->stride = stride;
    surface->bufferSize = totalBytes;
    surface->isHardware = false;
    surface->isPersistent = true;

    // 3. Allocate Padded Pixel Buffer (Persistent)
    // We request extra 64 bytes to ensure alignment.
    uint8_t* raw = (uint8_t*)reallocate(NULL, 0, totalBytes + 64);
    surface->pixels = (WldPixel*)(((uintptr_t)raw + 63) & ~(uintptr_t)63);

    // Initial clear (Zero-out VRAM)
    memset(surface->pixels, 0, totalBytes);

    // 4. Persistence Sentry
    markCard(surface);
    markCard(surface->pixels);

    printf(">> Walia Surface: Persistent Framebuffer initialized [%dx%d]\n", width, height);
    return surface;
}

SerResult walia_surface_map_hardware(ObjSurface* surface, const char* fbPath) {
    int fd = open(fbPath, O_RDWR);
    if (fd == -1) return SER_ERR_IO;

    // Map the hardware device directly into the surface's pixel pointer
    // This bypasses the persistent heap and talks to the monitor.
    void* hwPtr = mmap(NULL, surface->bufferSize, PROT_READ | PROT_WRITE, 
                       MAP_SHARED, fd, 0);

    if (hwPtr == MAP_FAILED) {
        close(fd);
        return SER_ERR_IO;
    }

    surface->pixels = (WldPixel*)hwPtr;
    surface->isHardware = true;
    surface->isPersistent = false;
    
    close(fd);
    printf(">> Walia Surface: Bare-Metal hardware link established (%s)\n", fbPath);
    return SER_OK;
}

// ==========================================
// ELEMENT MANAGEMENT
// ==========================================

ObjElement* walia_element_new(WldElementType type) {
    ObjElement* el = (ObjElement*)reallocate(NULL, 0, sizeof(ObjElement));
    el->obj.type = OBJ_NATIVE;
    el->obj.isMarked = false;
    el->obj.next = vm.objects;
    vm.objects = (struct Obj*)el;

    el->type = type;
    el->x = 0; el->y = 0;
    el->width = 0; el->height = 0;
    el->color = WLD_RGB(255, 255, 255);
    el->isVisible = true;
    el->zIndex = 0;

    markCard(el);
    return el;
}

void walia_surface_clear(ObjSurface* surface, WldColor color) {
    // UFO Speed Logic:
    // If the color is solid (e.g. Black), we use a fast memset.
    // If it's a specific pattern, we use the SIMD kernels from Group 3.
    if (color == 0) {
        memset(surface->pixels, 0, surface->bufferSize);
    } else {
        // Broad-sweep fill (optimized for 64-bit word size)
        uint64_t dword = ((uint64_t)color << 32) | color;
        uint64_t* ptr = (uint64_t*)surface->pixels;
        size_t count = surface->bufferSize / 8;
        for (size_t i = 0; i < count; i++) {
            ptr[i] = dword;
        }
    }
    
    // Mark the entire surface for synchronization
    markCard(surface->pixels);
}