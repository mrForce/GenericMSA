#include "align_info.h"
#include <math.h>

Alignments* run_alignment(ScoringMatrix* scoring, size_t alignment_length, size_t* sequence_sizes, size_t num_sequences){
  /*
    Note: for each element in sequence_sizes, alignment_length - sequences_size is the number of gaps that need to be inserted into it.

  */
  Dimensionality dp_dimensions;
  dp_dimensions.num_dimensions = num_sequences;
  dp_dimensions.dimension_sizes = sequence_sizes;
  DPTable* table = initialize_dp_table(&dp_dimensions, scoring);
  /*
    When we recurse, do this.
   */
  unsigned int recursion_limit = table->recursion_limit;
  size_t temp_indices[recursion_limit - 1];
  double scores[recursion_limit - 1];
  char valid_indices[recursion_limit - 1];
  char success;
  //now that we have the DP table, start at index 1 (since 0 has already been filled in!), and loop to the end.
  for(size_t i = 1; i < table->num_elements; i++){
    Point point;
    point.dimensions = dp_dimensions;
    index_to_point(i, dp_dimensions, &point);
    char valid = location_valid(sequence_sizes, &point, alignment_length);    
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
	  if(dp_table->elements[temp_index].valid){
	    /*
	      We add the score stored in the element to the one calculated by evaluate_move
	     */
	    valid_indices[j - 1] = 1;
	    double temp_score = dp_table->elements[temp_index].score + evaluate_move(scoring, &point, temp_coordinates);
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
	for(unsigned int i = 0; i < recursion_limit; i++){
	  if(valid_indices[i] && (max_score < scores[i])){
	    max_score = scores[i];
	    max_score_changed = 1;
	  }
	}
	assert(max_score_changed);
	BacktrackStore* store = &(element.backtrack);
	for(unsigned int i = 0; i < recursion_limit; i++){	  
	  if(valid_indices[i] && (max_score == scores[i])){
	    add_to_backtrackstore(store, temp_indices[i]);
	  }
	}
	
      }
    } else{
      table->elements[i].valid = 0;
    }

  }
  
}
