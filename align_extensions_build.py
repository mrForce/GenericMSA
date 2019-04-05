from cffi import FFI

builder = FFI()

builder.cdef("""
typedef struct BacktrackStore_{
  
  size_t* array;
  size_t num_elements;
  size_t capacity;
} BacktrackStore;



typedef struct Dimensionality_{
  size_t* dimension_sizes;
  size_t num_dimensions;
} Dimensionality;


typedef struct ScoringFunction_{
  double (*score)(void**, size_t*, size_t);
  void** data;
} ScoringFunction;



typedef struct Point_{
  size_t* coordinates;
  Dimensionality* dimensions;
} Point;





typedef struct DPElement_ {
  char valid;
  double score;
  BacktrackStore backtrack;
} DPElement;


typedef struct DPTable_{
  DPElement* elements;
  size_t num_elements;
  Dimensionality dimensions;
  //go from 1 up to BUT NOT including recursion_limit.
  unsigned int recursion_limit;
} DPTable;


typedef struct BacktrackResult_{
  Point** points;
  size_t num_points;
} BacktrackResult;

typedef struct Alignments_{
  double score;
  BacktrackResult** alignments;
  size_t num_alignments;
  size_t capacity;
} Alignments;






void add_to_backtrackstore(BacktrackStore*, size_t);
void add_to_alignments(BacktrackResult*, Alignments*);
BacktrackResult* duplicate_backtrack_result_add_space(BacktrackResult*);
size_t point_to_index(Point*);
void index_to_point(size_t, Dimensionality*, Point*);
double evaluate_move(ScoringFunction*, Point*, size_t*);
char location_valid(size_t*, Point*, size_t);
DPTable* initialize_dp_table(Dimensionality*, ScoringFunction*);
char get_recurse_point(unsigned int, size_t*, size_t*, size_t);
Alignments* run_alignment(ScoringFunction* scoring, size_t alignment_length, size_t* sequence_sizes, size_t num_sequences);""")
builder.set_source('_align', """#include "align.h" """, sources=['align.c'], libraries=['m'])

if __name__ == '__main__':
    builder.compile(verbose=True)

