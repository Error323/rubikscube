template <typename T>
class Deque {
   public:
    Deque(u64 n) {
        u64 pagesize = getpagesize();
        maxsize_in_mem_ = RoundUp(n * sizeof(T), pagesize);
        maxsize_ = n * sizeof(T);
        fd_ = memfd_create("deque", 0);
        assert(fd_ != -1);

        // allocate virtual memory
        if (ftruncate(fd_, maxsize_in_mem_) == -1) {
            assert(false);
        }

        // Create the virtual memory space of 2x the buffer size
        buffer_ = (char *)mmap(NULL, 2 * maxsize_in_mem_, PROT_NONE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        assert(buffer_ != MAP_FAILED);
        back_ = buffer_;

        // Map the virtual memories to the same physical memory region
        for (int i = 0; i < 2; i++) {
            void *p =
                mmap(buffer_ + i * maxsize_in_mem_, maxsize_in_mem_,
                     PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd_, 0);
            assert(p != MAP_FAILED);
        }
    }

    ~Deque() {
        for (s32 i = 0; i < 2; i++) {
            if (munmap(buffer_ + i * maxsize_in_mem_, maxsize_in_mem_) == -1) {
                assert(false);
            }
        }
        close(fd_);
    }

    void Push(T x) {
        assert(size_ <= maxsize_ + s64(sizeof(x)));
        *(T*)back_ = x;
        back_ += sizeof(x);
        if (back_ >= buffer_ + maxsize_in_mem_) {
            back_ -= maxsize_in_mem_;
        }
        size_ = Min(size_ + (s64)sizeof(x), maxsize_);
    }

    T& Pop() {
        assert(size_ > 0);
        T* ptr = (T*) (back_ + maxsize_in_mem_ - size_);
        size_ -= sizeof(T);
        return *ptr;
    }

    u64 Size() { return size_ / sizeof(T); }

   private:
    s64 maxsize_in_mem_{0};
    s64 maxsize_{0};
    s64 size_{0};
    s32 fd_;
    char *buffer_{nullptr};
    char *back_{nullptr};
};