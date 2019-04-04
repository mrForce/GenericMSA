from points_distance import make_struct
import _score
import math

def distance(point_one, point_two):
    return math.sqrt(sum([(point_one[i] - point_two[i])**2 for i in range(0, len(point_one))]))


def average_distance(groups):
    #groups[i] is a list of tuples. Each tuple is a point.
    total = 0
    n = 0
    for i in range(0, len(groups) - 1):
        for j in range(i + 1, len(groups)):
            for k in range(0, len(groups[i])):
                for l in range(0, len(groups[j])):
                    total += distance(groups[i][k], groups[j][l])
                    n += 1
    return total/n

points = [[[(1, 2), (3, 4)]], [[(70, 76), (650, 10)]]]

data, fun, ref = make_struct(points)
indices = [1, 1]

indices_data = _score.ffi.new('size_t[]', indices)
ref.append(indices_data)
indices_p = _score.ffi.cast('size_t*', indices_data)
print('average distance: %f' % average_distance([[(1, 2), (3, 4)], [(70, 76), (650, 10)]]))
print('data')
print(fun(data, indices_p, 2))


