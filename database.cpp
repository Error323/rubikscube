struct Database {
    u8 *data{nullptr};
    u64 num_entries{0};
    u64 size{0};

    void Init(u64 n) {
        assert(data == nullptr);
        num_entries = n;
        size = n >> 1;
        data = new u8[size];
        assert(data != nullptr);
        memset(data, 0xff, size);
    }

    void Write(const char *path) {
        FILE *file;
        file = fopen(path, "wb");
        fwrite(data, 1, size, file);
        fclose(file);
    }

    bool Update(u64 i, u8 depth) {
        assert(i < num_entries);
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
        assert(i < num_entries);
        u8 shift = i & 1;
        shift *= 4;
        i >>= 1;
        return (data[i] >> shift) & 0xf;
    }
};
