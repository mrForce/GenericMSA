from cffi import FFI

builder = FFI()
builder.cdef("""typedef struct SpatialPointSet_{
  double** coordinates;
  size_t num_points;
  size_t num_dimensions;
} SpatialPointSet;

typedef struct SequencePointSets_{
  /*
    If we have a bunch of 9-mers, then spatial_points is of length 9. 
   */
  SpatialPointSet** spatial_points;
  size_t length;
} SequencePointSets;
SpatialPointSet get_spatial_ps(double** coordinates, size_t num_points, size_t num_dimensions);
double sequence_score(SequencePointSets** sps_array, size_t* indices, size_t num_indices);""")

builder.set_source('_score', """#include "points_distance.h" """, sources=['points_distance.c'], libraries=['m'])

if __name__ == '__main__':
    builder.compile(verbose=True)

