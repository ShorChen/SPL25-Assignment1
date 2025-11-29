#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}

int DJControllerService::loadTrackToCache(AudioTrack& track) {
    std::string title = track.get_title();
    if (cache.contains(title)) {
        cache.get(title);
        return 1;
    }
    PointerWrapper<AudioTrack> cloned_track = track.clone();
    if (!cloned_track) {
        std::cerr << "[ERROR] Track: \"" << title << "\" failed to clone" << std::endl;
        return 0;
    }
    cloned_track->load();
    cloned_track->analyze_beatgrid();
    bool evicted = cache.put(std::move(cloned_track));
    if (evicted)
        return -1;
    return 0;
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}

void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    return cache.get(track_title);
}