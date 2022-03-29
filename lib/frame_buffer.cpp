#include <chrono>
#include <cstring>
#include <iostream>

#include "frame_buffer.hpp"
#include "log.hpp"

Frame::Frame(unsigned b, char* p) : _bytes(b), _mem{p} {}

char* Frame::get() {
    return _mem;
}

unsigned Frame::getBytes() const {
    return _bytes;
}

void FrameBuffer::addFrame(const Frame&& f) {
    logger->debug("Adding new frame to buffer");
    _mutex.lock();

    _buf = std::move(f);
    _available_frames++;
    _cv.notify_one();
    _mutex.unlock();
}

int FrameBuffer::moveLastFrame(unsigned ms) {
    logger->debug("Waiting for available frames");
    unsigned bytes = 0;
    std::mutex cv_m;
    std::unique_lock<std::mutex> lk{cv_m};

    // wait for a frame
    if(ms == 0) {
        _cv.wait(lk, [&] { return _cancel || _available_frames > 0; });
    } else {
        auto ret = _cv.wait_for(lk, std::chrono::milliseconds(ms),
                                [&] { return _cancel || _available_frames > 0; });
        if(!ret) return -2;
    }

    if(_cancel) {
        _cancel = false;
        return -1;
    }

    logger->debug("Retrieving frame");

    _mutex.lock();

    auto& frame = _buf;
    bytes = frame.getBytes();

    incReadFrame();

    _available_frames--;
    _mutex.unlock();

    return bytes;
}

void FrameBuffer::cancel() {
    _cancel = true;
    _cv.notify_one();
}

void FrameBuffer::reset() {
    _mutex.lock();
    _curr_write_frame = 0;
    _curr_read_frame = 0;
    _available_frames = 0;
    _mutex.unlock();
}

std::size_t FrameBuffer::getWriteFrame() const {
    return _curr_write_frame;
}

std::size_t FrameBuffer::getReadFrame() const {
    return _curr_read_frame;
}

std::size_t FrameBuffer::currFrames() const {
    return _available_frames;
}

void FrameBuffer::incWriteFrame() {
    ++_curr_write_frame;
    _curr_write_frame %= CACHE_SIZE;
}

void FrameBuffer::incReadFrame() {
    ++_curr_read_frame;
    _curr_read_frame %= CACHE_SIZE;
}
