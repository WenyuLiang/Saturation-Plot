// MIT License

// Copyright (c) 2023 Wenyu Liang

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include <iostream>
#include <fstream>
#include <string>
#include <htslib/sam.h>
#include <algorithm>
#include <random>
#include <string.h>
#include "khash.h" 
#define N_THREADS 8
#define N_PARTS 100000
#define UMI_LEN 10
#define CB_LEN 30
#define SEQ_LEN 100
#define SIZE 300000000
KHASH_SET_INIT_STR(uniseqs)

static inline float saturation(std::vector<bool> &uni_vec, const size_t &n){
    static size_t sum = 0;
    static size_t i = 0;
    for (; i < n; ++i) {
        sum += uni_vec[i];
    }
    return 1.0f - static_cast<float>(sum) / n; 
}

static inline std::vector<size_t> partition(const size_t &n){
    std::vector<size_t> parts;
    if (n < N_PARTS) {
        for (size_t i = 0; i < n; ++i) {
            parts.emplace_back(i+1);
        }
    } else {
        size_t count = 0;
        size_t part_size = n/N_PARTS;
        int remainder = n%N_PARTS;
        
        for (size_t i = 0; i < N_PARTS; ++i) {
            count += part_size;
            if (remainder > 0) {
                count++;
                remainder--;
            }
            parts.emplace_back(count);
        }
    }
    return parts;
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argc > 3) {
        fprintf(stderr, "Usage: %s <input.bam> <output>\n", argv[0]);
        fprintf(stderr, "  <input.bam>  Input BAM file.\n");
        fprintf(stderr, "  <output> of saturation\n");
        fprintf(stderr, "  <output> is optional, stdout\n");
        return 1;
    } 
    std::ofstream out;
    if (argv[2]){
        out.open(argv[2]);  // Use ofstream for writing to files
        if (!out.is_open()) {
            std::cerr << "Error opening file " << argv[2] << std::endl;
            return 1;
        }
    }

    samFile *in = sam_open(argv[1], "r");
    if (!in) {
        fprintf(stderr, "Failed to open input BAM file.\n");
        return 1;
    }
    hts_set_threads(in, N_THREADS);
    khash_t(uniseqs) *h = kh_init(uniseqs);
    
    int ret;
    khiter_t k; 
    std::string cb_str;
    std::string ub_str;
    bam_hdr_t *header = sam_hdr_read(in);  // Initialize header
    bam1_t *record = bam_init1();  // Initialize record
    std::string cb_ub_seq;
    size_t N = 0;
    cb_ub_seq.reserve(UMI_LEN+CB_LEN);
    std::vector<std::string> cb_ub_seqs;
    cb_ub_seqs.reserve(SIZE);
    std::vector<bool> is_uniq_vec;
    is_uniq_vec.reserve(SIZE);
    unsigned seed = 20231018;
    std::mt19937 g(seed);
    while (sam_read1(in, header, record) >= 0) {
        if (record->core.flag & 0x100) continue; 
        uint8_t *cb = bam_aux_get(record, "CB");
        uint8_t *ub = bam_aux_get(record, "UB");
        uint8_t *gn = bam_aux_get(record, "GN");
        if (cb && ub && gn) {  
            std::string cb_str = bam_aux2Z(cb);
            if(cb_str == "-") continue;
            std::string ub_str = bam_aux2Z(ub);
            if(ub_str == "-") continue;
            std::string gene_name = bam_aux2Z(gn);
            if(gene_name == "-") continue;
            cb_ub_seq.clear();
            cb_ub_seq += cb_str;
            cb_ub_seq += ub_str;
            N++;
            cb_ub_seqs.emplace_back(cb_ub_seq);
        }
    }
    bam_destroy1(record);
    bam_hdr_destroy(header);
    sam_close(in); 
    std::shuffle(cb_ub_seqs.begin(), cb_ub_seqs.end(), g);
    kh_resize(uniseqs, h, N);
    for (auto &cb_ub_seq : cb_ub_seqs) {
         k = kh_put(uniseqs, h, cb_ub_seq.c_str(), &ret);
        if(ret == 0) {
            is_uniq_vec.emplace_back(false);
        }else if(ret == 1) {
            is_uniq_vec.emplace_back(true);
            kh_key(h, k) = strdup(cb_ub_seq.c_str());
        }else {
            std::cerr << "Error inserting " << cb_ub_seq << std::endl;
            return 1; // -1 or 2
        } 
    }
    std::vector<std::string>().swap(cb_ub_seqs);
    for (k = 0; k < kh_end(h); ++k)
        if (kh_exist(h, k))
            free((char*)kh_key(h, k));
    kh_destroy(uniseqs, h); 
    std::vector<size_t> parts;
    parts.reserve(N_PARTS + 1>is_uniq_vec.size() ? is_uniq_vec.size():N_PARTS + 1);
    parts = partition(is_uniq_vec.size());
    for (auto &i : parts) {
        float saturation_ = saturation(is_uniq_vec, i);
        if (out.is_open()) {out << i<< "\t" << saturation_ << "\n";}
        else {std::cout << i<< "\t" << saturation_ << "\n";}
    }
    std::cerr << "all count: " << is_uniq_vec.size() << "\n";
    std::cerr << "unique count: " << std::count(is_uniq_vec.begin(), is_uniq_vec.end(), true) << "\n";
    if (out.is_open()) out.close();
    return 0;
}