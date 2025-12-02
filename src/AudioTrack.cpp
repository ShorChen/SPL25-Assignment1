#include "AudioTrack.h"
#include <iostream>
#include <cstring>
#include <random>

AudioTrack::AudioTrack(const std::string& title, const std::vector<std::string>& artists, 
                      int duration, int bpm, size_t waveform_samples)
    : title(title), artists(artists), duration_seconds(duration), bpm(bpm),
      waveform_data(nullptr), 
      waveform_size(waveform_samples) {
    waveform_data = new double[waveform_size];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-1.0, 1.0);
    for (size_t i = 0; i < waveform_size; ++i)
        waveform_data[i] = dis(gen);
    #ifdef DEBUG
    std::cout << "AudioTrack created: " << title << " by " << std::endl;
    for (const auto& artist : artists)
        std::cout << artist << " ";
    std::cout << std::endl;
    #endif
}

AudioTrack::~AudioTrack() {
    #ifdef DEBUG
    std::cout << "AudioTrack destructor called for: " << title << std::endl;
    #endif
    clear();
}

AudioTrack::AudioTrack(const AudioTrack& other)
    : title(other.title),
      artists(other.artists),
      duration_seconds(other.duration_seconds),
      bpm(other.bpm),
      waveform_data(nullptr),
      waveform_size(other.waveform_size)
{
    #ifdef DEBUG
    std::cout << "AudioTrack copy constructor called for: " << other.title << std::endl;
    #endif
    copy_from(other);
}

AudioTrack& AudioTrack::operator=(const AudioTrack& other) {
    #ifdef DEBUG
    std::cout << "AudioTrack copy assignment called for: " << other.title << std::endl;
    #endif
    if (this != &other) {
        clear();
        title = other.title;
        artists = other.artists;
        duration_seconds = other.duration_seconds;
        bpm = other.bpm;
        waveform_size = other.waveform_size;
        copy_from(other);
    }
    return *this;
}

AudioTrack::AudioTrack(AudioTrack&& other) noexcept 
    : title(std::move(other.title)),
      artists(std::move(other.artists)),
      duration_seconds(other.duration_seconds),
      bpm(other.bpm),
      waveform_data(nullptr),
      waveform_size(other.waveform_size)
{
    #ifdef DEBUG
    std::cout << "AudioTrack move constructor called for: " << other.title << std::endl;
    #endif
    move_from(std::move(other));
}

AudioTrack& AudioTrack::operator=(AudioTrack&& other) noexcept {
    #ifdef DEBUG
    std::cout << "AudioTrack move assignment called for: " << other.title << std::endl;
    #endif
    if (this != &other) {
        clear();
        title = std::move(other.title);
        artists = std::move(other.artists);
        duration_seconds = other.duration_seconds;
        bpm = other.bpm;
        waveform_size = other.waveform_size;
        move_from(std::move(other));
    }
    return *this;
}

void AudioTrack::get_waveform_copy(double* buffer, size_t buffer_size) const {
    if (buffer && waveform_data && buffer_size <= waveform_size)
        std::memcpy(buffer, waveform_data, buffer_size * sizeof(double));
}

void AudioTrack::clear() {
    delete[] waveform_data;
    waveform_data = nullptr;
    waveform_size = 0;
}

void AudioTrack::copy_from(const AudioTrack& other) {
    waveform_data = new double[waveform_size];
    for (size_t i = 0; i < waveform_size; i++)
        waveform_data[i] = other.waveform_data[i];
}

void AudioTrack::move_from(AudioTrack&& other) {
    waveform_data = other.waveform_data;
    other.waveform_data = nullptr;
    other.waveform_size = 0;
}