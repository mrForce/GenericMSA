#include "align_info.h"

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

char location_valid(size_t* sequence_sizes, Point* point, size_t alignment_length){
  /*
    This tells us if a location in the table is valid. 

    A location is invalid if the number of gaps inserted into one of the sequences is more than is allowed. That is, alignment_length - num_gaps > sequence_size. We get the number of gaps inserted by subtracting the index of the dimension from the maximum index of all the dimensions in the point. 

    (this limits the # of gaps, but is it possible to not have enough gaps? I don't think so).    
   */
  size_t max_coordinate = 0;
  for(size_t i = 0; i < point->dimensions->num_dimensions; i++){
    if(point->coordinates[i] > max_coordinate){
      max_coordinate = point->coordinates[i];
    }
  }
  for(size_t i = 0; i < point->dimensions->num_dimensions; i++){
    size_t num_gaps = max_coordinate - point->coordinates[i];
    if(num_gaps + point->coordinates[i] > sequence_sizes[i]){
      return 0;
    }       
  }
  return 1;
}

DPTable* initialize_dp_table(size_t num_dimensions, size_t length, ScoringMatrix* scoring){
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
    table->elements[i].valid = 0;
    table->elements[i].backtrack.num_elements = 0;
    table->elements[i].backtrack.capacity = 0;
  }
  /*
    Need to fill in the sides of the matrix.
   */
  
  
  
  return table;
}
