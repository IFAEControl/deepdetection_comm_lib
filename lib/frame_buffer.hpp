#pragma once

#include <condition_variable>
#include <cstddef>
#include <vector>
#include <mutex>
#include <atomic>

constexpr unsigned CACHE_SIZE = 1;

class Frame {
public:
	Frame() =default;
	explicit Frame(unsigned b, char* p);

	char* get();
	unsigned getBytes() const;

private:
	bool _delete{false};
	unsigned _bytes;
	char* _mem;
};

class FrameBuffer {
public:
	void addFrame(const Frame&& f);
	int moveLastFrame(unsigned ms = 0);
	void cancel();
	void reset();

	std::size_t getWriteFrame() const;
	std::size_t getReadFrame() const;
	std::size_t currFrames() const;
private:
	void incWriteFrame();
	void incReadFrame();

	bool _cancel{false};

	Frame _buf;
	std::size_t _curr_write_frame{0};
	std::size_t _curr_read_frame{0};
	std::atomic_size_t _available_frames{0};

	std::mutex _mutex;
	std::condition_variable _cv{};
};
