#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}

DJLibraryService::DJLibraryService() 
    : playlist(""), library() {}

DJLibraryService::~DJLibraryService() {
    for (AudioTrack* track : library) {
        delete track;
    }
    library.clear();
}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    // Clear existing library if any
    for (AudioTrack* track : library) {
        delete track;
    }
    library.clear();

    for (const auto& info : library_tracks) {
        AudioTrack* new_track = nullptr;

        if (info.type == "MP3") {
            // Mapping based on SessionFileParser.h:
            // extra_param1 = bitrate
            // extra_param2 = has_tags (int treated as bool)
            new_track = new MP3Track(info.title, info.artists, info.duration_seconds, 
                                     info.bpm, info.extra_param1, static_cast<bool>(info.extra_param2));
            
            std::cout << "[INFO] MP3Track created: " << info.extra_param1 << " kbps" << std::endl;

        } else if (info.type == "WAV") {
            // Mapping based on SessionFileParser.h:
            // extra_param1 = sample_rate
            // extra_param2 = bit_depth
            new_track = new WAVTrack(info.title, info.artists, info.duration_seconds, 
                                     info.bpm, info.extra_param1, info.extra_param2);
            
            std::cout << "[INFO] WAVTrack created: " << info.extra_param1 << "Hz/" << info.extra_param2 << "bit" << std::endl;
        }

        if (new_track) {
            library.push_back(new_track);
        }
    }

    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded" << std::endl;
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;

    // Create a new local playlist
    Playlist new_playlist(playlist_name);

    for (int index : track_indices) {
        // Convert 1-based index (from config) to 0-based index (vector)
        int vec_index = index - 1;

        if (vec_index >= 0 && vec_index < static_cast<int>(library.size())) {
            AudioTrack* source_track = library[vec_index];

            // POLYMORPHIC CLONE: 
            // The library owns the original. The Playlist needs its own copy.
            // clone() returns a PointerWrapper, we release() it to give raw pointer to Playlist
            PointerWrapper<AudioTrack> wrapper = source_track->clone();
            
            // Safety check
            if (wrapper) {
                // Unwrap the raw pointer
                AudioTrack* cloned_track = wrapper.release();
                
                // Simulate loading/analysis on the playlist copy
                cloned_track->load();
                cloned_track->analyze_beatgrid();

                // Add to the new playlist
                new_playlist.add_track(cloned_track);
            } else {
                 std::cerr << "[ERROR] Failed to clone track index: " << index << std::endl;
            }

        } else {
            std::cout << "[WARNING] Invalid track index: " << index << std::endl;
        }
    }

    // Move the new playlist into the member variable
    // This requires Playlist move assignment operator
    playlist = std::move(new_playlist);

    std::cout << "[INFO] Playlist loaded: " << playlist_name 
              << " (" << playlist.get_track_count() << " tracks)" << std::endl;
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    
    for (const auto* track : tracks) {
        if (track) {
            titles.push_back(track->get_title());
        }
    }
    return titles;
}
