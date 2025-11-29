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
    for (AudioTrack* track : library)
        delete track;
    library.clear();
}

/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    DJLibraryService::~DJLibraryService();
    for (const auto& info : library_tracks) {
        AudioTrack* new_track = nullptr;
        if (info.type == "MP3") {
            new_track = new MP3Track(info.title, info.artists, info.duration_seconds, 
                                     info.bpm, info.extra_param1, static_cast<bool>(info.extra_param2));
            std::cout << "[INFO] MP3Track created: " << info.extra_param1 << " kbps" << std::endl;

        } else if (info.type == "WAV") {
            new_track = new WAVTrack(info.title, info.artists, info.duration_seconds, 
                                     info.bpm, info.extra_param1, info.extra_param2);
            std::cout << "[INFO] WAVTrack created: " << info.extra_param1 << "Hz/" << info.extra_param2 << "bit" << std::endl;
        }
        if (new_track)
            library.push_back(new_track);
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
    playlist.display();
    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    return playlist;
}

AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    Playlist new_playlist(playlist_name);
    for (int index : track_indices) {
        int vec_index = index - 1;
        if (vec_index >= 0 && vec_index < static_cast<int>(library.size())) {
            AudioTrack* source_track = library[vec_index];
            PointerWrapper<AudioTrack> wrapper = source_track->clone();
            if (wrapper) {
                AudioTrack* cloned_track = wrapper.release();                
                cloned_track->load();
                cloned_track->analyze_beatgrid();
                new_playlist.add_track(cloned_track);
            } else std::cerr << "[ERROR] Failed to clone track index: " << index << std::endl;
        } else std::cout << "[WARNING] Invalid track index: " << index << std::endl;
    }
    playlist = std::move(new_playlist);
    std::cout << "[INFO] Playlist loaded: " << playlist_name 
              << " (" << playlist.get_track_count() << " tracks)" << std::endl;
}

std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for (const auto* track : tracks)
        if (track)
            titles.push_back(track->get_title());
    return titles;
}