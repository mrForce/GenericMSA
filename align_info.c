#include "align_info.h"



void add_to_backtrackstore(BacktrackStore* store, size_t index){
  if(store->capacity == 0){
    assert(store->array == NULL);
    store->array = (size_t*) malloc(sizeof(size_t));
    store->num_elements = 0;
    store->capacity = 1;
  }else if(store->num_elements == store->capacity){
    assert(store->array != NULL);
    size_t new_capacity = 1.5*store->num_elements;
    if(new_capacity == store->num_elements){
      //rounding ;)
      new_capacity++;
    }
    store->array = (size_t*) realloc(store->array, new_capacity*sizeof(size_t));
    store->capacity = new_capacity;
  }
  assert(store->capacity > 0 && store->capacity > store->num_elements);
  assert(store->array != NULL);
  store->array[store->num_elements] = index;
  store->num_elements++;
}

BacktrackResult* duplicate_backtrack_result_add_space(BacktrackResult* result){
  /*
    Returns a copy of result, except it adds an entry for another Point*, and increases num_points by 1.
   */
  BacktrackResult* new_result = (BacktrackResult*) malloc(sizeof(BacktrackResult));
  new_result->num_points = result->num_points + 1;
  new_result->points = (Point**) malloc(sizeof(Point*)*(result->num_points + 1));
  if(result->num_points > 0){
    memcpy(new_result->points, result->points, sizeof(Point*)*(result->num_points));
  }
  new_result->points[result->num_points] = NULL;
  return new_result;
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

void index_to_point(size_t index, Dimensionality* dimensions, Point* p){
  /*
    The formula for point to index is:

    n_d + N_d*(n_{d - 1} + N_{d - 1}(n_{d - 2} + N_{d - 2}(...)))

    So, to get n_d, we take index % N_d. Then, we divide the index by N_d, which gives is the next term n_{d - 1} + N_{d - 1}(n_{d - 2} + N_{d - 2}(...))
   */
  size_t n, next_term = index, num_dimensions = dimensions->num_dimensions;
  for(size_t i = num_dimensions - 1; i >= 0; i++){
    p->coordinates[i] = next_term % dimensions->dimension_sizes[i];
    assert(p->coordinates[i] < dimensions->dimension_sizes[i]);
    next_term = next_term/dimensions->dimension_sizes[i];
  }
}

double evaluate_move(ScoringFunction* score_func, Point* current_point, size_t* next_point_coordinates){
  /*
    Considering moving from current_point to next_point_coordinates.
   */


  size_t num_dimensions = current_point->dimensions->num_dimensions;
  size_t coordinates[num_dimensions];

  for(size_t i = 0; i < num_dimensions; i++){
    assert(current_point->coordinates[i] >= next_point_coordinates[i]);
    if(current_point->coordinates[i] == next_point_coordinates[i]){
      //then gap.
      coordinates[i] = 0;
    }else{
      /*
	Since we added 1 to each axis to allow for normal Needleman-Wunsch table shape, we don't need to add 1 to this anymore.
      */
      coordinates[i] = next_point_coordinates[i];
    }
  }
  
  double score = score_func->score(score_func->data, coordinates, num_dimensions);
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

DPTable* initialize_dp_table(Dimensionality* dimensions, ScoringFunction* scoring, size_t max_sequence_length, size_t alignment_length){
  assert(max_sequence_length <= alignment_length);
  assert(sizeof(int) == 4);
  assert(dimensions->num_dimensions <= 32);
  /*
    We want an unsigned int that's 2^{num_dimensions}
   */
  unsigned int recursion_limit = 0;
  size_t num_elements = 1;
  for(size_t i = 0; i < dimensions->num_dimensions; i++){
    num_elements *= (1 + dimensions->dimension_sizes[i]);
    recursion_limit *= 2;
  }
  DPTable* table = (DPTable*) malloc(sizeof(DPTable));
  table->recursion_limit = recursion_limit;
  table->elements = (DPElement*) malloc(sizeof(DPElement)*num_elements);
  table->num_elements = num_elements;
  for(size_t i = 0; i < num_elements; i++){
    table->elements[i].score = 0;    
    table->elements[i].backtrack.array = NULL;
    table->elements[i].valid = 0;
    table->elements[i].backtrack.num_elements = 0;
    table->elements[i].backtrack.capacity = 0;
  }
  /*
    Fill in element (0, 0, ..., 0) of the table. It has index 0.
  */
  if(max_sequence_length < alignment_length){ 
    size_t coordinates[dimensions->num_dimensions];
    table->elements[0].valid = 1;
    table->elements[0].score = 0;
  }
  return table;
}

char get_recurse_point(unsigned int bits, size_t* coordinates, size_t* new_coordinates, size_t coordinates_size){
  /*
    Returns 0 if one of the elements of new_coordinates is negative.
    Returns 1 otherwise.
   */
  for(unsigned int i = 0; i < coordinates_size; i++){
    assert(coordinates[i] >= 0);
    if(bits & (1 << i)){      
      //then we need to subtract 1
      new_coordinates[i] = coordinates[i] - 1;
      if(coordinates[i] < 0){
	return 0;
      }      
    }else{
      new_coordinates[i] = coordinates[i];
    }
  }
  return 1;
}
