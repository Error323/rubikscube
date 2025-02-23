#define MAGIC 0xfeffc2f9

struct Database {
    enum Type { CORNER, EDGE1, EDGE2, PERMUTATION };
    struct Header {
        u32 magic{MAGIC};
        Type type{CORNER};
        u64 num_entries{0};
        u64 size{0};
    };

    Header hdr;
    u8 *data{nullptr};

    void Init(u64 n, Type type) {
        assert(data == nullptr);
        hdr.type = type;
        hdr.num_entries = n;
        hdr.size = n >> 1;
        data = new u8[hdr.size];
        assert(data != nullptr);
        memset(data, 0xff, hdr.size);
    }

    bool Load(const char *path) {
        Header hdr_{0};
        FILE *file;
        file = fopen(path, "rb");
        bool valid = true;
        valid &= fread(&hdr_, sizeof(hdr_), 1, file) != 0;
        valid &= (hdr_.magic == MAGIC);
        valid &= (hdr_.type == hdr.type);
        valid &= (hdr_.size == hdr.size);
        valid &= (hdr_.num_entries == hdr.num_entries);
        if (!valid) {
            fprintf(stderr, "Invalid database file: '%s'\n", path);
            return false;
        }
        fread(data, 1, hdr_.size, file);
        fclose(file);
        return true;
    }

    void Write(const char *path) {
        FILE *file;
        file = fopen(path, "wb");
        fwrite(&hdr, sizeof(hdr), 1, file);
        fwrite(data, 1, hdr.size, file);
        fclose(file);
    }

    bool Update(u64 i, u8 depth) {
        assert(i < hdr.num_entries);
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
        assert(i < hdr.num_entries);
        u8 shift = i & 1;
        shift *= 4;
        i >>= 1;
        return (data[i] >> shift) & 0xf;
    }

    f64 Mean() {
        u64 sum = 0;
        u64 n = hdr.size / sizeof(u64);
        assert(hdr.num_entries % n == 0);
        u64 *ptr = (u64*) data;
        for (u64 i = 0; i < n; i++) {
            u64 v = ptr[i];
            while (v) {
                sum += v & 0xf;
                v >>= 4;
            }
        }
        return sum / (f64)hdr.num_entries;
    }
};
