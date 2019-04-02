#include <assert.h>
#include <stdlib.h>


typedef struct BacktrackStore_{
  size_t* array;
  size_t num_elements;
  size_t capacity;
} BacktrackStore;

void add_to_backtrackstore(BacktrackStore* store, size_t index){
  if(store->capacity == 0){
    assert(store->array == NULL);
    store->array = (size_t*) malloc(sizeof(size_t));
    store->num_elements = 0;
    store->capacity = 0;
  }else if(store->num_elements == store->capacity){
    assert(store->array != NULL);
    size_t new_capacity = 1.5*store->num_elements;
    if(new_capacity == store->num_elements){
      //rounding ;)
      new_capacity++;
    }
    store->array = (size_t*) realloc(store->array, new_capacity*sizeof(size_t));
    store->capacity = new_capacity
  }
  assert(store->capacity > 0 && store->capacity > store->num_elements);
  assert(store->array != NULL);
  store->array[store->num_elements] = index;
  store->num_elements++;
}



typedef struct ScoringMatrix_ {
  double* scores;
  size_t n;
  Dimensionality dimensions;
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


double evaluate_move(ScoringMatrix* score_matrix, Point* current_point, size_t* next_point_coordinates){
  /*
    Considering moving from current_point to next_point_coordinates.
   */
  Point new_point;
  assert(score_matrix->dimensions->num_dimensions == current_point->dimensions->num_dimensions);
  size_t num_dimensions = current_point->dimensions->num_dimensions;
  size_t coordinates[num_dimensions];
  new_point.coordinates = coordinates;
  new_point.dimensions = score_matrix->dimensions;
  for(size_t i = 0; i < num_dimensions; i++){
    assert(current_point->coordinates[i] >= next_point_coordinates[i]);
    if(current_point->coordinates[i] == next_point_coordinates[i]){
      //then gap.
      new_point.coordinates[i] = 0;
    }else{
      /*
	Since we use 0 to indicate a gap, 
       */
      new_point.coordinates[i] = next_point_coordinates[i] + 1;
    }
  }
  
  size_t score_index = point_to_index(&new_point);
  
  assert(index < score_matrix->n);
  double score = score_matrix->scores[index];
  return score;
}

typedef struct Data_ {
  Dimensionality dimensions;
  ScoringMatrix scores;
} Data;

typedef struct DPElement_ {
  double score;
  BacktrackStore backtrack;
} DPElement;


typedef struct DPTable_{
  DPElement* elements;
  Dimensionality dimensions;
} DPTable;

DPTable* initialize_dp_table(size_t num_dimensions, size_t length){
  /*
    Length is really the target length. 
   */
  assert(num_dimensions >= 2);
  assert(length > 1);
  DPTable* table = (DPTable*) malloc(sizeof(DPTable));
  size_t* dimensions = (size_t*) malloc(sizeof(size_t)*num_dimensions);
  for(size_t i = 0; i < num_dimensions; i++){
    dimensions[i] = length;
  }
  table->dimensions.num_dimensions = num_dimensions;
  table->elements = (DPElement*) malloc(sizeof(DPElement)*num_dimensions*length);
  for(size_t i = 0; i < num_dimensions*length; i++){
    table->elements[i].score = 0;
    table->elements[i].backtrack.array = NULL;
    table->elements[i].backtrack.num_elements = 0;
    table->elements[i].backtrack.capacity = 0;
  }
  return table;
}
