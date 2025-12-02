#include "MixingEngineService.h"
#include <iostream>
#include <memory>
#include <cmath>

MixingEngineService::MixingEngineService()
    : decks(),
      active_deck(1), 
      auto_sync(false), 
      bpm_tolerance(0)
{
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks." << std::endl;
    for (int i = 0; i < 2; ++i)
        if (decks[i]) {
            delete decks[i];
            decks[i] = nullptr;
        }
}


/**
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> cloned_track = track.clone();
    if (!cloned_track) {
        std::cerr << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return -1;
    }
    size_t target_deck = (decks[active_deck] == nullptr) ? active_deck : (1 - active_deck);
    std::cout << "[Deck Switch] Target deck: " << target_deck << std::endl;
    if (decks[target_deck]) {
        delete decks[target_deck];
        decks[target_deck] = nullptr;
    }
    cloned_track->load();
    cloned_track->analyze_beatgrid();
    if (decks[active_deck] && auto_sync)
        if (!can_mix_tracks(cloned_track))
            sync_bpm(cloned_track);
    decks[target_deck] = cloned_track.release();
    std::cout << "[Load Complete] '" << decks[target_deck]->get_title() 
              << "' is now loaded on deck " << target_deck << std::endl;
    if (target_deck != active_deck && decks[active_deck]) {
        std::cout << "[Unload] Unloading previous deck " << active_deck 
                  << " (" << decks[active_deck]->get_title() << ")" << std::endl;
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }
    active_deck = target_deck;
    std::cout << "[Active Deck] Switched to deck " << target_deck << std::endl;
    return static_cast<int>(target_deck);
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i)
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else std::cout << "Deck " << i << ": [EMPTY]\n";
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if (!decks[active_deck] || !track)
        return false;
    int current_bpm = decks[active_deck]->get_bpm();
    int new_bpm = track->get_bpm();    
    return std::abs(current_bpm - new_bpm) <= bpm_tolerance;
}

/**
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if (!decks[active_deck] || !track) return;
    int current_bpm = decks[active_deck]->get_bpm();
    int new_bpm = track->get_bpm();
    int avg_bpm = (current_bpm + new_bpm) / 2;
    std::cout << "[Sync BPM] Syncing BPM from " << new_bpm << " to " << avg_bpm << std::endl;
    track->set_bpm(avg_bpm);
}