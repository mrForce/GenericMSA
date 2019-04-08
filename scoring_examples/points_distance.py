import _score
import gc

def make_struct(sps_list):
    """
    sps list goes like this: [[[point at position 0 of sequence 0 in group 0], [point at position 0 of sequence 1 in group 0], ...], [[point at position 0 of sequence 0 in group 1], [point at position 1 of sequence 1 in group 1]]]
    """
    struct_list = []
    references = []
    num_dimensions = len(sps_list[0][0][0])
    
    for group_index in range(0, len(sps_list)):
        spatial_points_sets = []
        #length of all sequences in the group. They should all be the same.
        sequence_length = len(sps_list[group_index][0])
        for sequence in sps_list[group_index]:
            assert(len(sequence) == sequence_length)
        for position in range(0, sequence_length):
            coordinates_at_position = []
            for sequence_index in range(0, len(sps_list[group_index])):
                point = sps_list[group_index][sequence_index][position]
                assert(len(point) == num_dimensions)                
                print('point')
                print(point)
                p = _score.ffi.new('double[]', len(point))
                references.append(p)
                for i in range(0, len(point)):
                    p[i] = point[i]
                coordinates_at_position.append(p)
            coordinates_position_array = _score.ffi.new('double*[]', coordinates_at_position)
            references.append(coordinates_position_array)
            coordinates_position_pointer = _score.ffi.cast('double**', coordinates_position_array)
            references.append(coordinates_position_pointer)
            thing = _score.ffi.new('SpatialPointSet*', {'coordinates': coordinates_position_pointer, 'num_points': len(sps_list[group_index]), 'num_dimensions': num_dimensions})
            spatial_points_sets.append(thing)
            references.append(thing)
        print(spatial_points_sets)
        assert(len(spatial_points_sets) >= 3)
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
            

#a = [[[(1, 2), (3, 4)], [(5, 6), (7, 8), (9, 10)]], [[(70, 76), (650, 10)]]]
"""
points = [[[(0, 0), (1.0, 0.), (2, 19), (5, 2.1), (10.5, 7.9)]], [[(0,0), (2.1, 20), (5.1, 2), (10, 8)]], [[(0, 0), (1.1, 0), (4.9, 2.2)]]]
b, func, references = make_struct(points)
print(b)
print(b[0].spatial_points[0].num_dimensions)
"""
