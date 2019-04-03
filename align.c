#include "align.h"
#include <math.h>

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
  p->dimensions = &(table->dimensions);
  p->coordinates = (size_t*) malloc(sizeof(size_t)*(table->dimensions.num_dimensions));
  index_to_point(index, &(table->dimensions), p);
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

Alignments* run_alignment(ScoringFunction* scoring, size_t alignment_length, size_t* sequence_sizes, size_t num_sequences){
  /*
    Note: for each element in sequence_sizes, alignment_length - sequences_size is the number of gaps that need to be inserted into it.

  */
  Dimensionality* dp_dimensions = (Dimensionality*) malloc(sizeof(Dimensionality));
  dp_dimensions->num_dimensions = num_sequences;
  dp_dimensions->dimension_sizes = sequence_sizes;
  DPTable* table = initialize_dp_table(dp_dimensions, scoring);
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
    Point point;
    point.dimensions = dp_dimensions;
    index_to_point(i, dp_dimensions, &point);
    char valid = location_valid(sequence_sizes, &point, alignment_length);
    if(i == table->num_elements - 1){
      assert(valid);
    }
    if(valid){
      //now loop through the recursion.
      DPElement element = table->elements[i];
      element.valid = 1;
      for(unsigned int j = 1; j < recursion_limit; j++){
	/*
	  The bits of j tell us which indices to decrement.
	 */
	size_t temp_coordinates[num_sequences];
	//set up the coordinates
	char success = get_recurse_point(j, point.coordinates, temp_coordinates, num_sequences);
	if(success){
	  Point temp_point;
	  temp_point.coordinates = temp_coordinates;
	  temp_point.dimensions = dp_dimensions;
	  size_t temp_index = point_to_index(&temp_point);
	  if(table->elements[temp_index].valid){
	    /*
	      We add the score stored in the element to the one calculated by evaluate_move
	     */
	    valid_indices[j - 1] = 1;
	    double temp_score = table->elements[temp_index].score + evaluate_move(scoring, &point, temp_coordinates);
	    scores[j - 1] = temp_score;
	    temp_indices[j - 1] = temp_index;
	  }else{
	    valid_indices[j - 1] = 0;
	    scores[j - 1] = 0;
	    temp_indices[j - 1] = 0;
	  }
	  
	  
	}

	/* 
	   Now we've evaluated all possible recursions, we need to go through them and find out the max score. 
	   Then, loop through them once more, and add the ones with score equal to the max to the backtracking store
	*/
	double max_score = -INFINITY;
	char max_score_changed = 0;
	for(unsigned int k = 0; k < recursion_limit; k++){
	  if(valid_indices[k] && (max_score < scores[k])){
	    max_score = scores[k];
	    max_score_changed = 1;
	  }
	}
	assert(max_score_changed);
	BacktrackStore* store = &(element.backtrack);
	for(unsigned int k = 0; k < recursion_limit; k++){	  
	  if(valid_indices[k] && (max_score == scores[k])){
	    add_to_backtrackstore(store, temp_indices[k]);
	  }
	}
	if(i == table->num_elements - 1){
	  final_score = max_score;
	  final_score_set = 1;
	}
	
      }
    } else{
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
  return alignments;
}

int main(){
  return 0;
}
