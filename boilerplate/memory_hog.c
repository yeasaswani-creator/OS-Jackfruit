/*
 * memory_hog.c - Memory pressure workload for soft / hard limit testing.
 *
 * Default behavior:
 *   - allocate 8 MiB every second
 *   - touch each page so RSS actually grows
 *
 * Usage:
 *   /memory_hog [chunk_mb] [sleep_ms]
 *
 * If you plan to copy this binary into an Alpine rootfs, build it in a way
 * that is runnable inside that filesystem, such as static linking or
 * rebuilding it from inside the rootfs/toolchain you choose.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * memory_hog:
 * Continuously allocates memory in chunks (MB) and touches it
 * so RSS actually increases (important for your kernel monitor)
 */

static size_t parse_size_mb(const char *arg, size_t fallback)
{
    if (!arg) return fallback;

    char *end = NULL;
    unsigned long val = strtoul(arg, &end, 10);

    if (*arg == '\0' || (end && *end != '\0') || val == 0)
        return fallback;

    return (size_t)val;
}

static useconds_t parse_sleep_ms(const char *arg, useconds_t fallback)
{
    if (!arg) return fallback;

    char *end = NULL;
    unsigned long val = strtoul(arg, &end, 10);

    if (*arg == '\0' || (end && *end != '\0'))
        return fallback;

    return (useconds_t)(val * 1000); // ms → µs
}

int main(int argc, char *argv[])
{
    // default: allocate 10MB every 100ms
    size_t chunk_mb = (argc > 1) ? parse_size_mb(argv[1], 10) : 10;
    useconds_t sleep_us = (argc > 2) ? parse_sleep_ms(argv[2], 100) : 100000;

    size_t chunk_bytes = chunk_mb * 1024 * 1024;

    int count = 0;

    printf("Starting memory hog: %zu MB per step\n", chunk_mb);

    while (1)
    {
        char *mem = malloc(chunk_bytes);
        if (!mem)
        {
            printf("malloc failed after %d allocations\n", count);
            break;
        }

        // IMPORTANT: touch memory so kernel counts it in RSS
        memset(mem, 1, chunk_bytes);

        count++;

        printf("Allocated %d chunks → total ~%zu MB\n",
               count, (size_t)count * chunk_mb);

        fflush(stdout);

        usleep(sleep_us);
    }

    return 0;
}
