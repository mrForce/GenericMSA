import _score
import gc

def make_struct(sps_list):
    """
    sps list goes like this: [[[point at position 0 of sequence 0 in group 0], [point at position 0 of sequence 1 in group 0], ...], [[point at position 0 of sequence 0 in group 1], [point at position 1 of sequence 1 in group 1]]]
    """
    struct_list = []
    
    for group_index in range(0, len(sps_list)):
        spatial_points_sets = []
        for pos in range(0, len(sps_list[group_index])):
            coordinates_list = []
            for point in sps_list[group_index][pos]:
                print('point')
                print(point)
                p = _score.ffi.new('double[]', len(point))
                for i in range(0, len(point)):
                    p[i] = point[i]
                coordinates_list.append(p)
            coordinates_pa = _score.ffi.new('double*[]', coordinates_list)
            coordinates_double_p = _score.ffi.cast('double**', coordinates_pa)
            print('num points: %d' % len(sps_list[group_index][pos]))
            spatial_points_sets.append(_score.ffi.new('SpatialPointSet*', {'coordinates': coordinates_double_p, 'num_points': len(sps_list[group_index][pos]), 'num_dimensions': len(sps_list[group_index][pos][0])}))
        
        a = _score.ffi.new('SpatialPointSet*[]', spatial_points_sets)
        b = _score.ffi.cast('SpatialPointSet**', a)
        gc.collect()
        print(spatial_points_sets[0].num_points)
        sequence_point_set = _score.ffi.new('SequencePointSets*', {'spatial_points': b, 'length': len(spatial_points_sets)})
        struct_list.append(sequence_point_set)
    c = _score.ffi.new('SequencePointSets*[]', struct_list)
    d = _score.ffi.cast('SequencePointSets**', c)
    
    return (d, weak_dict)
            
                
a = [[[(1, 2), (3, 4)], [(5, 6), (7, 8), (9, 10)]], [[(70, 76), (650, 10)]]]
b = make_struct(a)
print(b[0])
print(b[0].spatial_points[0].num_dimensions)
