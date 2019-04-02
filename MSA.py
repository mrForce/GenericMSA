def point_to_index(loc, matrix_dim):
    assert(len(loc) == len(matrix_dim))
    off = 0
    for i in range(0, len(loc)):
	if i == len(loc) - 1:
	    off += loc[i]
	else:
	    prod = 1
	    for j in range(i + 1, len(loc)):
		prod *= matrix_dim[j]
		off += prod*loc[i]
    return off
