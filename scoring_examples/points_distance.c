#include "points_distance.h"
#include <math.h>
#include <assert.h>

SpatialPointSet get_spatial_ps(double** coordinates, size_t num_points, size_t num_dimensions){
  SpatialPointSet sps;
  sps.coordinates = coordinates;
  sps.num_points = num_points;
  sps.num_dimensions = num_dimensions;
  return sps;
}
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
double sequence_score(SequencePointSets** sps_array, size_t* indices, size_t num_indices){
  /*
    sps_array should point to an array of length num_indices.

   */
  //use the recursive mean to avoid numerical error.
  double mean = 0;
  size_t n = 1;
  for(size_t i = 0; i < num_indices - 1; i++){
    for(size_t j = i + 1; j < num_indices; j++){
      if(indices[i] > 0 && indices[j] > 0){
	size_t pos_one = indices[i] - 1;
	size_t pos_two = indices[j] - 1;
	/*
	  Loop through the cartesian product of the two groups. 	  
	 */
	assert(sps_array[i]->length > pos_one);
	SpatialPointSet* spatial_one = sps_array[i]->spatial_points[pos_one];
	double** coordinates_one = spatial_one->coordinates;
	assert(sps_array[j]->length > pos_two);
	SpatialPointSet* spatial_two = sps_array[j]->spatial_points[pos_two];
	double** coordinates_two = spatial_two->coordinates;
	assert(spatial_one->num_dimensions == spatial_two->num_dimensions);
	/*
	  Take the pairwise distances between points in coordinates_one and coordinates_two
	 */

	for(size_t k = 0; k < spatial_one->num_points; k++){
	  for(size_t l = 0; l < spatial_two->num_points; l++){
	    double d = distance(coordinates_one[k], coordinates_two[l], spatial_one->num_dimensions);
	    mean += (d - mean)/n;
	    n++;
	  }
	}
      }
    }
  }
  return mean;
}
/*
int main(){
  return 0;
}
*/
