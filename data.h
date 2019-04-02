typedef struct ScoringMatrix_ {
  double* scores;
  size_t n;
} ScoringMatrix;

typedef struct Dimensionality_{
  size_t* dimension_sizes;
  size_t num_dimensions;
} Dimensionality;

typedef struct Point_{
  size_t* coordinates;
  Dimensionality* dimensions;
} Point;

size_t point_to_index(Point* point){
  
}

typedef struct Data_ {
  Dimensionality dimensions;
  ScoringMatrix scores;
  
} Data;
