#include "align.h"
#include <string.h>
#include <math.h>
#undef NDEBUG
#include <assert.h>

size_t** recover_alignment(BacktrackResult* result, Dimensionality* dimensions, size_t recovery_length){
  printf("num dimensions: %zu\n", dimensions->num_dimensions);
  size_t** alignment = (size_t**) malloc((dimensions->num_dimensions)*sizeof(size_t*));
  
  for(size_t i = 0; i < dimensions->num_dimensions; i++){
    alignment[i] = (size_t*) malloc(recovery_length*sizeof(size_t));
  }
  Point** points = result->points;
  Point* last_point = points[0];
  Point* current_point;
  size_t current_index = recovery_length - 1;
  size_t num_points = result->num_points;
  for(size_t i = 0; i < num_points - 1; i++){
    fprintf(stderr, "(%zu, %zu, %zu)->", points[i]->coordinates[0], points[i]->coordinates[1], points[i]->coordinates[2]);
  }
  fprintf(stderr, "(%zu, %zu, %zu)\n", points[num_points - 1]->coordinates[0], points[num_points - 1]->coordinates[1], points[num_points - 1]->coordinates[2]);
  assert(num_points == recovery_length + 1);
  for(size_t i = 1; i < num_points; i++){
    current_point = points[i];
    assert(num_points - i - 1 < recovery_length);
    fprintf(stderr, "i: %zu, num_points: %zu, Num points thing: %zu, recovery length: %zu\n", i, num_points, num_points - i - 1, recovery_length);
    for(size_t j = 0; j < dimensions->num_dimensions; j++){
      if(last_point->coordinates[j] == current_point->coordinates[j]){
	//then insert a gap
	alignment[j][num_points - i - 1] = 0;
      }else{
	if(last_point->coordinates[j] <= current_point->coordinates[j]){
	  printf("hi%d\n", current_point->coordinates[j]);
	}
	assert(last_point->coordinates[j] > current_point->coordinates[j]);
	printf("Things: %zu, i: %zu, j: %zu, num_points - i - 1: %zu\n", last_point->coordinates[j], i, j, num_points - i - 1);
	alignment[j][num_points - i - 1] = last_point->coordinates[j];
      }
    }
    last_point = current_point;
    current_index--;
  }
  return alignment;
}


void add_to_alignments(BacktrackResult* result, Alignments* alignments){
  if(alignments->capacity == 0){
    assert(alignments->alignments == NULL);
    alignments->alignments = (BacktrackResult**) malloc(sizeof(BacktrackResult*));
    alignments->capacity = 1;
    alignments->num_alignments = 0;
  }else if(alignments->num_alignments == alignments->capacity){
    assert(alignments->alignments != NULL);
    size_t new_cap = 1.5*alignments->num_alignments;
    if(new_cap == alignments->num_alignments){
      //rounding
      new_cap++;
    }
    alignments->alignments = (BacktrackResult**) realloc(alignments->alignments, new_cap*sizeof(BacktrackResult*));
    alignments->capacity = new_cap;
  }
  assert(alignments->capacity > 0 && alignments->capacity > alignments->num_alignments);
  assert(alignments->alignments != NULL);
  alignments->alignments[alignments->num_alignments] = result;
  alignments->num_alignments++;
}



void backtrack(Alignments*, DPTable*, BacktrackResult*, size_t);

void backtrack(Alignments* alignments, DPTable* table, BacktrackResult* parent_result, size_t index){
  //duplicate the parent result
  Point* p = (Point*) malloc(sizeof(Point));
  p->dimensions = table->dimensions;
  p->coordinates = (size_t*) malloc(sizeof(size_t)*(table->dimensions->num_dimensions));
  index_to_point(index, table->dimensions, p);
  BacktrackResult* new_result = duplicate_backtrack_result_add_space(parent_result);
  new_result->points[parent_result->num_points] = p;
  if(index == 0){
    add_to_alignments(new_result, alignments);
  }else{
    DPElement element = table->elements[index];
    assert(element.valid);
    BacktrackStore backtracks = element.backtrack;
    assert(backtracks.num_elements > 0);
    assert(backtracks.array);
    for(size_t i = 0; i < backtracks.num_elements; i++){
      backtrack(alignments, table, new_result, backtracks.array[i]);
    }
    //since we copied it in the next backtrack call, we don't need it anymore.
    free(new_result);
  }
}

FinalResults* run_alignment(ScoringFunction* scoring, size_t alignment_length, size_t* sequence_sizes, size_t num_sequences){
  /*
    Note: for each element in sequence_sizes, alignment_length - sequences_size is the number of gaps that need to be inserted into it.

  */
  Dimensionality* dp_dimensions = (Dimensionality*) malloc(sizeof(Dimensionality));
  dp_dimensions->num_dimensions = num_sequences;
  size_t* dimension_sizes = (size_t*) malloc(sizeof(size_t)*num_sequences);
  size_t max_sequence_length = 0;
  for(size_t i = 0; i < num_sequences; i++){
    dimension_sizes[i] = 1 + sequence_sizes[i];
    if(sequence_sizes[i] > max_sequence_length){
      max_sequence_length = sequence_sizes[i];
    }
  }
  dp_dimensions->dimension_sizes = dimension_sizes;
  DPTable* table = initialize_dp_table(dp_dimensions, scoring, max_sequence_length, alignment_length);
  /*
    When we recurse, do this.
   */
  unsigned int recursion_limit = table->recursion_limit;
  size_t temp_indices[recursion_limit - 1];
  double scores[recursion_limit - 1];
  char valid_indices[recursion_limit - 1];
  char success;
  double final_score;
  char final_score_set = 0;
  //now that we have the DP table, start at index 1 (since 0 has already been filled in!), and loop to the end.
  for(size_t i = 1; i < table->num_elements; i++){
    size_t thing = sizeof(Point);
    Point* point = (Point*) malloc(sizeof(Point));
    point->dimensions = dp_dimensions;
    point->coordinates = (size_t*) malloc(sizeof(size_t)*dp_dimensions->num_dimensions);
    index_to_point(i, dp_dimensions, point);
    char valid = location_valid(sequence_sizes, point, alignment_length);
    if(i == table->num_elements - 1){
      assert(valid);
    }
    if(valid){
      //now loop through the recursion.
      DPElement* element = &(table->elements[i]);
      element->valid = 1;
      for(unsigned int j = 1; j < recursion_limit; j++){
	/*
	  The bits of j tell us which indices to decrement.
	 */
	size_t temp_coordinates[num_sequences];
	//set up the coordinates
	char success = get_recurse_point(j, point->coordinates, temp_coordinates, num_sequences);
	if(success){
	  Point* temp_point = (Point*) malloc(sizeof(Point));
	  temp_point->coordinates = temp_coordinates;
	  temp_point->dimensions = dp_dimensions;
	  size_t temp_index = point_to_index(temp_point);
	  table->elements[temp_index].valid = location_valid(sequence_sizes, temp_point, alignment_length);
	  if(table->elements[temp_index].valid){
	    /*
	      We add the score stored in the element to the one calculated by evaluate_move
	     */
	    valid_indices[j - 1] = 1;
	    double temp_score = table->elements[temp_index].score + evaluate_move(scoring, point, temp_coordinates);
	    scores[j - 1] = temp_score;
	    temp_indices[j - 1] = temp_index;
	  }else{
	    valid_indices[j - 1] = 0;
	    scores[j - 1] = 0;
	    temp_indices[j - 1] = 0;
	  }
	  
	  
	}else{
	  valid_indices[j-1] = 0;
	  scores[j - 1] = 0;
	}
      }
	/* 
	   Now we've evaluated all possible recursions, we need to go through them and find out the max score. 
	   Then, loop through them once more, and add the ones with score equal to the max to the backtracking store
	*/
	double max_score = -INFINITY;
	char max_score_changed = 0;
	for(unsigned int k = 0; k < recursion_limit - 1; k++){
	  printf("scores: %f, k: %d\n", scores[k], k);
	  printf("valid: %d\n", valid_indices[k]);
	  if(valid_indices[k] && (max_score < scores[k])){
	    max_score = scores[k];
	    max_score_changed = 1;
	  }
	}
	element->score = max_score;
	assert(max_score_changed);
	BacktrackStore* store = &(element->backtrack);
	for(unsigned int k = 0; k < recursion_limit - 1; k++){	  
	  if(valid_indices[k] && (max_score == scores[k])){
	    add_to_backtrackstore(store, temp_indices[k]);
	  }
	}
	if(i == table->num_elements - 1){
	  final_score = max_score;
	  final_score_set = 1;
	}
    }else{
      table->elements[i].valid = 0;
    }
	
  }
  assert(final_score_set);
  Alignments* alignments = (Alignments*) malloc(sizeof(Alignments));
  alignments->alignments = NULL;
  alignments->num_alignments = 0;
  alignments->capacity = 0;
  alignments->score = final_score;
  BacktrackResult starting_result;
  starting_result.points = NULL;
  starting_result.num_points = 0;
  backtrack(alignments, table, &starting_result, table->num_elements - 1);
  FinalResults* results = (FinalResults*) malloc(sizeof(FinalResults));
  results->alignments = (size_t***) malloc(sizeof(size_t**)*(alignments->num_alignments));
  results->num_alignments = alignments->num_alignments;
  int num_good_alignments = 0;
  for(size_t i = 0; i < alignments->num_alignments; i++){
    if(alignments->alignments[i]->num_points  == alignment_length + 1){
      num_good_alignments++;
      results->alignments[i] = recover_alignment(alignments->alignments[i], dp_dimensions, alignment_length);
    }
  }
  fprintf(stderr, "num good aligments: %d\n", num_good_alignments);

  /*
    I don't care if this leaks memory!
   */
  return results;
}

typedef struct SequenceData_ {
  char* seq_one;
  char* seq_two;
} SequenceData;

double test_scoring(SequenceData** data, size_t* coordinates, size_t num_dimensions){
  assert(num_dimensions == 2);
  if(coordinates[0] == 0 || coordinates[1] == 0){
    return -1;
  }
  char* seq_one = (*data)->seq_one;
  char* seq_two = (*data)->seq_two;
  
  if(seq_one[coordinates[0] - 1] == seq_two[coordinates[1] - 1]){
    return 1;
  }else{
    return -1;
  }
}

int main(){
  char* seq_one = "GAAC";
  char* seq_two = "CAAGAC";
  SequenceData* store = (SequenceData*) malloc(sizeof(SequenceData));
  store->seq_one = seq_one;
  store->seq_two = seq_two;
  ScoringFunction func;
  func.data = (void**) &store;
  func.score = (double (*)(void**, size_t*, size_t))&(test_scoring);
  size_t sizes[2];
  sizes[0] = strlen(seq_one);
  sizes[1] = strlen(seq_two);
  size_t alignment_length = 6;
  size_t num_sequences = 2;
  FinalResults* results = run_alignment(&func, alignment_length, sizes, num_sequences);
  for(size_t i = 0; i < results->num_alignments; i++){
    /*
      In the shape [num_seq][alignment_length]
     */
    size_t** seq_alignments = results->alignments[i];
    for(size_t j = 0; j < alignment_length; j++){
      if(seq_alignments[0][j] == 0){
	printf("-");
      }else{
	printf("%c", seq_one[seq_alignments[0][j] - 1]);
      }
    }
    printf("\n");
    for(size_t j = 0; j < alignment_length; j++){
      if(seq_alignments[1][j] == 0){
	printf("-");
      }else{
	printf("%c", seq_two[seq_alignments[1][j] - 1]);
      }
    }
    printf("\n");
  }
  
  return 0;
}

