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
  /*
    Formula taken from here: https://en.wikipedia.org/wiki/Row-_and_column-major_order.

    Could probably do some optimization here, like storing the products. I'm not going to do that yet.
   */
  size_t offset = 0;
  size_t d = point->dimensions->num_dimensions;
  for(size_t i = 0; i < d - 1; i++){
    size_t product = 1;
    for(size_t j = i + 1; j < d; j++){
      product *= point->dimensions->dimension_sizes[j];
    }
    assert(product > 0);
    offset += point->coordinates[i]*product;    
  }
  //last one doesn't get multiplied by anything
  offset += point->coordinates[d - 1];
  return offset;
}

double get_score(ScoringMatrix* score_matrix, Point* point){
  size_t index = point_to_index(point);
  assert(index < score_matrix->n);
  return score_matrix->scores[index];
}

typedef struct Data_ {
  Dimensionality dimensions;
  ScoringMatrix scores;
} Data;
