#include <assert.h>
#include <stdlib.h>


typedef struct BacktrackStore_{
  
  size_t* array;
  size_t num_elements;
  size_t capacity;
} BacktrackStore;



typedef struct Dimensionality_{
  size_t* dimension_sizes;
  size_t num_dimensions;
} Dimensionality;


typedef struct ScoringMatrix_ {
  double* scores;
  size_t n;
  Dimensionality dimensions;
} ScoringMatrix;


typedef struct Point_{
  size_t* coordinates;
  Dimensionality* dimensions;
} Point;




typedef struct Data_ {
  Dimensionality dimensions;
  ScoringMatrix scores;
} Data;

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



void add_to_backtrackstore(BacktrackStore* store, size_t index);
BacktrackResult* duplicate_backtrack_result_add_space(BacktrackResult* result);
size_t point_to_index(Point* point);
void index_to_point(size_t index, Dimensionality* dimensions, Point* p);
double evaluate_move(ScoringMatrix* score_matrix, Point* current_point, size_t* next_point_coordinates);
char location_valid(size_t* sequence_sizes, Point* point, size_t alignment_length);
DPTable* initialize_dp_table(size_t num_dimensions, size_t length, ScoringMatrix* scoring);
Point get_recurse_point(unsigned int bits, size_t* coordinates, size_t coordinates_size);
