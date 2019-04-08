import _align

import scoring_examples._score as score
from scoring_examples.points_distance import make_struct

class Error(Exception):
    pass

class SequenceOfWrongSizeError(Error):
    def __init__(self, expected_sequence_len, sequence_len):
        self.message = 'Expected a list of length: %d, but got one of: %d' % (expected_sequence_len, sequence_len)


class SequenceAlignment:
    def __init__(self, alignment_array, sequence_length):
        self.alignment_array = alignment_array
        self.sequence_length = sequence_length
    def align(self, sequence):
        if len(sequence) == self.sequence_length:
            alignment = ['-']*len(self.alignment_array)
            for i in range(0, len(self.alignment_array)):
                if self.alignment_array[i] > 0:
                    #it's not a gap, so subtract 1 from the index.
                    alignment[i] = sequence[self.alignment_array[i] - 1]
            return alignment
        else:
            raise SequenceOfWrongSizeError(self.sequence_length, len(sequence))

    
    
"""

Example:

points = [[[(1, 2), (3, 4)]], [[(70, 76), (650, 10)]]]
"""

"""
scoring is a C function like this (need to got via CFFI!):

double score(void** data, size_t* indices, size_t num_indices)

That computes the score for one alignment column (given data).

data is the void** that eventually gets passed into the scoring function. To be clear: data is a pointer to a pointer, not an array of pointers, or anything like that. It's like this because of the quirkiness of CFFI (and quite possibly my ignorance on how to use it properly!)

Returns a list of SequenceAlignment objects, of the same length as the sequence_sizes list
"""
def run_alignment(scoring, data, alignment_length, sequence_sizes):
    references = []
    references.append(scoring)
    casted_scoring = _align.ffi.cast('double(*)(void**, size_t*, size_t)', scoring)
    references.append(casted_scoring)
    references.append(data)
    casted_data = _align.ffi.cast('void**', data)
    references.append(casted_data)
    sf_struct = _align.ffi.new('ScoringFunction*', {'score': casted_scoring, 'data': casted_data})
    sequence_sizes_array = _align.ffi.new('size_t[]', sequence_sizes)
    references.append(sequence_sizes_array)
    sequence_sizes_pointer = _align.ffi.cast('size_t*', sequence_sizes_array)
    references.append(sequence_sizes_pointer)
    final_results = _align.lib.run_alignment(sf_struct, alignment_length, sequence_sizes_pointer, len(sequence_sizes))
    num_alignments = final_results.num_alignments
    alignments = []
    """
    There are num_alignments alignments with the same max score.
    """
    for i in range(0, num_alignments):
        #now we have a single alignment of all the sequences
        alignment = []
        for j in range(0, len(sequence_sizes)):
            sequence_alignment = []
            for k in range(0, alignment_length):
                sequence_alignment.append(final_results.alignments[i][j][k])
            print('sequence alignment: ')
            print(sequence_alignment)
            sequence_alignment_object = SequenceAlignment(sequence_alignment, sequence_sizes[j])
            alignment.append(sequence_alignment_object)
        alignments.append(alignment)
    return alignments



points = [[[(0, 0), (1.0, 0.), (2, 19), (5, 2.1), (10.5, 7.9)]], [[(0,0), (2.1, 20), (5.1, 2), (10, 8)]], [[(0, 0), (1.1, 0), (4.9, 2.2)]]]
sequences = ['ABC', 'ABCD', 'ABCDE']
sequence_sizes = [3, 4, 5]
data, func, references = make_struct(points)

results = run_alignment(func, data, 5, sequence_sizes)
print(results[0])
print(results[0][0].align(sequences[0]))
print(results[0][1].align(sequences[1]))
print(results[0][2].align(sequences[2]))
