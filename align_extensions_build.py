from cffi import FFI

builder = FFI()

build.cdef("Alignments* run_alignment(ScoringFunction* scoring, size_t alignment_length, size_t* sequence_sizes, size_t num_sequences);");

