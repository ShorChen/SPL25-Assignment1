#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    std::string title = track.get_title();

    // 1. Check if track exists in cache (HIT)
    if (cache.contains(title)) {
        // Refresh MRU status by accessing it
        cache.get(title);
        return 1; // HIT
    }

    // 2. Track not in cache (MISS) - Prepare a new copy
    // Polymorphic clone: Library owns original, Cache owns this new copy
    PointerWrapper<AudioTrack> cloned_track = track.clone();

    if (!cloned_track) {
        std::cerr << "[ERROR] Track: \"" << title << "\" failed to clone" << std::endl;
        return 0; // Treat as no-op/failure
    }

    // Simulate loading work on the CLONE (not the original)
    // This represents reading from disk into the controller's limited memory
    cloned_track->load();
    cloned_track->analyze_beatgrid();

    // Insert into cache
    // put() returns true if an eviction occurred, false otherwise
    bool evicted = cache.put(std::move(cloned_track));

    if (evicted) {
        return -1; // MISS with eviction
    } else {
        return 0; // MISS (inserted into empty slot)
    }
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    // Your implementation here
    return cache.get(track_title);
}
