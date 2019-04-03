#include "points_distance.h"
#include <math.h>
/*
  The average pairwise distance of a bunch of points
 */
double sequence_score(SequencePointSets* sps_array, size_t* indices, size_t num_indices){
  /*
    sps_array should point to an array of length num_indices.

   */
  //use the recursive mean to avoid numerical error.
  double mean = 0;
  size_t n = 1;
  /* 
     Start with within group points.
  */
  assert(num_indices == groups_pointer->num_groups);
  for(size_t i = 0; i < num_indices; i++){
    if(indices[i] > 0){
      //position 0, 1, ..., 8 of a 9-mer, for example.
      size_t sequence_position = indices[i] - 1;
      for(size_t j = 0; j < groups_pointer->groups[i]->; j++){
	for(size_t k = 0; k < groups_pointer->groups[i].length; k++){
	  if(j != k){
	    
	  }
	}
      }
    }else{
      /* Then we have a gap */
    }
  }
}
