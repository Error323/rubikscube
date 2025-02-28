#define MAGIC 0xfeffc2f9

struct Database {
    enum Type { INVALID, CORNER, EDGE1, EDGE2, PERMUTATION };
    struct Header {
        u32 magic{MAGIC};
        Type type{INVALID};
        u64 num_entries{0};
        u64 size{0};
    };

    Header *hdr{nullptr};
    u8 *data{nullptr};
    void *map{nullptr};

    bool MemoryMapReadWrite(const char *path, u64 n, Type type) {
        s32 fd = -1;
        if (access(path, R_OK | W_OK) != 0) {
            fd = creat(path, 0666);
            close(fd);
        }

        fd = open(path, O_RDWR);

        if (fd == -1) {
            perror("open");
            return false;
        }

        // allocate virtual memory on disk
        u64 size = sizeof(Header) + n / 2;
        if (ftruncate(fd, size) == -1) {
            perror("ftrucate");
            return false;
        }

        // Create the virtual memory space
        map = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
            perror("mmap");
            return false;
        }
        hdr = (Header*) map;
        data = (u8*) map + sizeof(Header);

        hdr->magic = MAGIC;
        hdr->type = type;
        hdr->num_entries = n;
        hdr->size = n >> 1;
        memset(data, 0xff, hdr->size);

        return true;
    }

    bool MemoryMapReadOnly(const char *path, Type type) {
        int fd = open(path, O_RDONLY);
        if (fd == -1) {
            perror("open");
            return false;
        }

        struct stat st;
        if (fstat(fd, &st) == -1) {
            perror("fstat");
            close(fd);
            return false;
        }

        map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (map == MAP_FAILED) {
            perror("mmap");
            return false;
        }

        hdr = (Header*) map;
        data = (u8*) map + sizeof(Header);

        printf("%x %d\n", hdr->magic, hdr->type);

        return hdr->magic == MAGIC && hdr->type == type;
    }

    bool Update(u64 i, u8 depth) {
        assert(i < hdr->num_entries);
        u8 shift = i & 1;
        shift *= 4;
        i >>= 1;
        u8 mask = 0xf << shift;
        u8 v = (data[i] >> shift) & 0xf;
        if (v > depth) {
            data[i] &= ~mask;
            data[i] |= depth << shift;
        }
        return v > depth;
    }

    u8 Get(u64 i) {
        assert(i < hdr->num_entries);
        u8 shift = i & 1;
        shift *= 4;
        i >>= 1;
        return (data[i] >> shift) & 0xf;
    }

    f64 Mean() {
        u64 sum = 0;
        u64 n = hdr->size / sizeof(u64);
        assert(hdr->num_entries % n == 0);
        u64 *ptr = (u64*) data;
        for (u64 i = 0; i < n; i++) {
            u64 v = ptr[i];
            while (v) {
                sum += v & 0xf;
                v >>= 4;
            }
        }
        return sum / (f64)hdr->num_entries;
    }
};
