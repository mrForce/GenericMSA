import _score
import gc

def make_struct(sps_list):
    """
    sps list goes like this: [[[point at position 0 of sequence 0 in group 0], [point at position 0 of sequence 1 in group 0], ...], [[point at position 0 of sequence 0 in group 1], [point at position 1 of sequence 1 in group 1]]]
    """
    struct_list = []
    references = []
    for group_index in range(0, len(sps_list)):
        spatial_points_sets = []
        for pos in range(0, len(sps_list[group_index])):
            coordinates_list = []
            for point in sps_list[group_index][pos]:
                print('point')
                print(point)
                p = _score.ffi.new('double[]', len(point))
                references.append(p)
                for i in range(0, len(point)):
                    p[i] = point[i]
                coordinates_list.append(p)
            coordinates_pa = _score.ffi.new('double*[]', coordinates_list)
            references.append(coordinates_pa)
            coordinates_double_p = _score.ffi.cast('double**', coordinates_pa)
            references.append(coordinates_double_p)
            print('num points: %d' % len(sps_list[group_index][pos]))
            thing = _score.ffi.new('SpatialPointSet*', {'coordinates': coordinates_double_p, 'num_points': len(sps_list[group_index][pos]), 'num_dimensions': len(sps_list[group_index][pos][0])})
            spatial_points_sets.append(thing)
            references.append(thing)
        
        a = _score.ffi.new('SpatialPointSet*[]', spatial_points_sets)
        references.append(a)
        b = _score.ffi.cast('SpatialPointSet**', a)
        references.append(b)
        gc.collect()
        print(spatial_points_sets[0].num_points)
        sequence_point_set = _score.ffi.new('SequencePointSets*', {'spatial_points': b, 'length': len(spatial_points_sets)})
        references.append(sequence_point_set)
        struct_list.append(sequence_point_set)
    c = _score.ffi.new('SequencePointSets*[]', struct_list)
    references.append(c)
    d = _score.ffi.cast('SequencePointSets**', c)
    references.append(d)
    return (d, _score.ffi.addressof(_score.lib, 'sequence_score'), references)
            
"""                
a = [[[(1, 2), (3, 4)], [(5, 6), (7, 8), (9, 10)]], [[(70, 76), (650, 10)]]]
b, func, references = make_struct(a)
print(b[0])
print(b[0].spatial_points[0].num_dimensions)
"""
