#include "Playlist.h"
#include "AudioTrack.h"
#include <iostream>
#include <algorithm>
Playlist::Playlist(const std::string& name) 
    : head(nullptr), playlist_name(name), track_count(0) {
    std::cout << "Created playlist: " << name << std::endl;
}

// Copy Constructor
Playlist::Playlist(const Playlist& other) 
    : head(nullptr), playlist_name(other.playlist_name), track_count(0) {
    
    // Deep copy the linked list
    PlaylistNode* current = other.head;
    PlaylistNode* last_added = nullptr;

    while (current) {
        // Create new node pointing to the SAME track (Borrowing)
        PlaylistNode* new_node = new PlaylistNode(current->track);
        
        if (!head) {
            head = new_node;
        } else {
            last_added->next = new_node;
        }
        
        last_added = new_node;
        current = current->next;
        track_count++;
    }
}

// Copy Assignment Operator
Playlist& Playlist::operator=(const Playlist& other) {
    if (this != &other) {
        // 1. Clean up existing nodes
        PlaylistNode* current = head;
        while (current) {
            PlaylistNode* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        track_count = 0;

        // 2. Copy data
        playlist_name = other.playlist_name;

        // 3. Deep copy the linked list
        current = other.head;
        PlaylistNode* last_added = nullptr;

        while (current) {
            PlaylistNode* new_node = new PlaylistNode(current->track);
            
            if (!head) {
                head = new_node;
            } else {
                last_added->next = new_node;
            }
            
            last_added = new_node;
            current = current->next;
            track_count++;
        }
    }
    return *this;
}

// TODO: Fix memory leaks!
// Students must fix this in Phase 1
Playlist::~Playlist() {
    #ifdef DEBUG
    std::cout << "Destroying playlist: " << playlist_name << std::endl;
    #endif
    PlaylistNode* next;
    while (head) {
        next = head->next;
        delete head;
        head = next;
    }
}

Playlist::Playlist(Playlist&& other) noexcept 
    : head(other.head), 
      playlist_name(std::move(other.playlist_name)), 
      track_count(other.track_count) 
{
    other.head = nullptr;
    other.track_count = 0;
}

Playlist& Playlist::operator=(Playlist&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        PlaylistNode* current = head;
        while (current) {
            PlaylistNode* next = current->next;
            delete current;
            current = next;
        }
        
        // Move resources
        head = other.head;
        playlist_name = std::move(other.playlist_name);
        track_count = other.track_count;
        
        // Reset source
        other.head = nullptr;
        other.track_count = 0;
    }
    return *this;
}

void Playlist::add_track(AudioTrack* track) {
    if (!track) {
        std::cout << "[Error] Cannot add null track to playlist" << std::endl;
        return;
    }

    // Create new node - this allocates memory!
    PlaylistNode* new_node = new PlaylistNode(track);

    // Add to front of list
    new_node->next = head;
    head = new_node;
    track_count++;

    std::cout << "Added '" << track->get_title() << "' to playlist '" 
              << playlist_name << "'" << std::endl;
}

void Playlist::remove_track(const std::string& title) {
    PlaylistNode* current = head;
    PlaylistNode* prev = nullptr;

    // Find the track to remove
    while (current && current->track->get_title() != title) {
        prev = current;
        current = current->next;
    }

    if (current) {
        // Remove from linked list
        if (prev) {
            prev->next = current->next;
        } else {
            head = current->next;
        }
        delete current;
        track_count--;
        std::cout << "Removed '" << title << "' from playlist" << std::endl;

    } else {
        std::cout << "Track '" << title << "' not found in playlist" << std::endl;
    }
}

void Playlist::display() const {
    std::cout << "\n=== Playlist: " << playlist_name << " ===" << std::endl;
    std::cout << "Track count: " << track_count << std::endl;

    PlaylistNode* current = head;
    int index = 1;

    while (current) {
        std::vector<std::string> artists = current->track->get_artists();
        std::string artist_list;

        std::for_each(artists.begin(), artists.end(), [&](const std::string& artist) {
            if (!artist_list.empty()) {
                artist_list += ", ";
            }
            artist_list += artist;
        });

        AudioTrack* track = current->track;
        std::cout << index << ". " << track->get_title() 
                  << " by " << artist_list
                  << " (" << track->get_duration() << "s, " 
                  << track->get_bpm() << " BPM)" << std::endl;
        current = current->next;
        index++;
    }

    if (track_count == 0) {
        std::cout << "(Empty playlist)" << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}

AudioTrack* Playlist::find_track(const std::string& title) const {
    PlaylistNode* current = head;

    while (current) {
        if (current->track->get_title() == title) {
            return current->track;
        }
        current = current->next;
    }

    return nullptr;
}

int Playlist::get_total_duration() const {
    int total = 0;
    PlaylistNode* current = head;

    while (current) {
        total += current->track->get_duration();
        current = current->next;
    }

    return total;
}

std::vector<AudioTrack*> Playlist::getTracks() const {
    std::vector<AudioTrack*> tracks;
    PlaylistNode* current = head;
    while (current) {
        if (current->track)
            tracks.push_back(current->track);
        current = current->next;
    }
    return tracks;
}