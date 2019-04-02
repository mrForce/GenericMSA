#include "align_info.h"


Alignments* run_alignment(ScoringMatrix* scoring, size_t num_dimensions, size_t alignment_length, size_t* sequence_sizes, size_t num_sequences){
  /*
    Note: for each element in sequence_sizes, alignment_length - sequences_size is the number of gaps that need to be inserted into it.

  */
  DPTable* dp_table = 
}
