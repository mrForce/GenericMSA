#include "points_distance.h"
#include <math.h>

double distance(double* point_one, double* point_two, size_t num_dimensions){

  double sum = 0;
  for(size_t i = 0; i < num_dimensions; i++){
    sum += (point_one[i] - point_two[i])*(point_one[i] - point_two[i]);    
  }
  return sqrt(sum);
}

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
     
  */
  assert(num_indices == groups_pointer->num_groups);
  for(size_t i = 0; i < num_indices - 1; i++){
    for(size_t j = i + 1; j < num_indices; j++){
      if(indices[i] > 0 && indices[j] > 0){
	size_t pos_one = indices[i] - 1;
	size_t pos_two = indices[j] - 1;
	/*
	  Loop through the cartesian product of the two groups. 	  
	 */
	assert(sps_array[i].length > pos_one);
	double** coordinates_one = sps_array[i].spatial_points[pos_one].coordinates;
	assert(sps_array[j].length > pos_two);
	double** coordinates_two = sps_array[j].spatial_points[pos_two].coordinates;
	assert(coordinates_one.num_dimensions == coordinates_two.num_dimensions);
	/*
	  Take the pairwise distances between points in coordinates_one and coordinates_two
	 */

	for(size_t k = 0; k < coordinates_one.num_points; k++){
	  for(size_t l = 0; l < coordinates_two.num_points; l++){
	    double d = distance(coordinates_one[k], coordinates_two[l], coordinates_one.num_dimensions);
	    mean += (d - mean)/n;
	    n++;
	  }
	}
      }
    }
  }
  return mean;
}
