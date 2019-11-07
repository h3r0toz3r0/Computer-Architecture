# Prediction based on a finite state machine. Prediction is either strong taken, weakly taken, weakly not taken
# or strongly not taken. 
# 2-bit counter history stored (up to 16 bits) in a FIFO queue
# 11 = strongly taken, 10 = weakly taken, 01 = weakly not-taken, 00 = strongly not taken
def two_bit_counter(line,prev,branches,q2,hits2,misses2,total_branches2):
# Initialize variables
    element = 0
# If 16-bit queue is empty, the prediction automatically guesses Weakly Not-Taken and adds "1" (01) to queue
    if len(q2) == 0:
        q2.append(1)
# If 16-bit queue is full, the first element (oldest element) is removed from the queue - FIFO. Numbers stored as integers 0-3 so q2 length must be 8 or less
    if len(q2) >= 8:
        q2.pop(0) 
# 2-Bit Counter is initialized as last element in the queue
    counter = q2[len(q2)-1]
# 2-Bit Counter will guess taken if it is Weakly/Strongly Taken (10 or 11)
    if counter > 1:
        guess = 1
# 2-Bit Counter will guess not-taken if it is Weakly/Strongly Not-Taken (01 or 00)
    elif counter <= 1:
        guess = 0
# If the current line address equals the line address in the dictory at the previous line's prediction
# Then the prediction is considered a hit and 2-Bit Counter is updated.
    if line == branches[prev][guess]:
# If 2-bit Counter is strongly taken (11) and prediction is a hit, 2-Bit Counter remains 11
        if counter == 3:
            element = 3
# If 2-bit Counter is weakly taken (10) and prediction is a hit, 2-Bit Counter updates to be 11
        elif counter == 2:
            element = 3
# If 2-bit Counter is weakly not-taken (01) and prediction is a hit, 2-Bit Counter updates to be 00
        elif counter == 1:
            element = 0
# If 2-bit Counter is strongly not-taken (00) and prediction is a hit, 2-Bit Counter remains 00
        elif counter == 0:
            element = 0
# Prediction is added to queue
        q2.append(element)
        hits2 += 1
# Otherwise, prediction is considered a miss. 2-Bit Counter is updated.
    elif line != branches[prev][guess]:
# If 2-bit Counter is strongly taken (11) and prediction is a miss, 2-Bit Counter updates to be 10
        if counter == 3:
            element = 2
# If 2-bit Counter is weakly taken (10) and prediction is a miss, 2-Bit Counter updates to be 01
        elif counter == 2:
            element = 1
# If 2-bit Counter is weakly not-taken (01) and prediction is a miss, 2-Bit Counter updates to be 10
        elif counter == 1:
            element = 2
# If 2-bit Counter is strongly not-taken (00) and prediction is a miss, 2-Bit Counter updates to be 01
        elif counter == 0:
            element = 1
        q2.append(element)
        misses2 += 1
    total_branches2 += 1
    return (hits2,misses2,total_branches2)

# Prediction based on previous branch decision. If the last branch was taken, prediction assumes taken again.
# If last branch was not taken, prediction assumes not taken again. 
# Branch history (up to 32 bits) is stored in a branch history table which pops off elements as FIFO
def one_bit_predictor(line,prev,branches,q,hits,misses,total_branches):
# If 32-bit queue is empty, the prediction automatically guesses Taken and adds "1" to the history table
    if len(q) == 0:
        q.append(1)
# If 32-bit queue is full, the first element (oldest element) is removed from the queue - FIFO
    if len(q) >= 32:
        q.pop(0)
# If the current line address equals the line address in the dictory at the previous line's prediction
# Then the prediction is considered a hit
    if line == branches[prev][q[len(q)-1]]:
        element = q[len(q)-1]
# Last guess (taken/not taken) is added to the branch history table
        q.append(element)
        hits += 1
# Otherwise, prediction is considered a miss. Last element in branch history table is swapped
# If element previously taken, the branch history table has a new element that is not taken. And vice versa.
    else:
        element = abs(q[len(q)-1]-1)
        q.append(element)
        misses += 1
    total_branches += 1
    return (hits,misses,total_branches)

# Finds all conditional branches in itrace
# Maps conditional branch addresses with possible return addresses
def find_branches():
# Opens itrace and initializes functions with zero addresses and no previous lines
    f = open('itrace.out', 'r')
    addresses = {}
    prev = None
    for line in f:
        line = line.strip()
# If first line in itrace, address is added to address dictory. Prev is set to current address
        if prev is None:
            addresses[line] = set()
            prev = line
            continue
# If not the first line in itrace, address is added to address dictory with its previous line
# This links each address with the next address (determining return address/addresses of each unique address)
        addresses[prev].add(line)
        if line not in addresses:
            addresses[line] = set()
        prev = line

    f.close()
    branches = {}
# Remove unconditional branches and normal instruction flow
    for k in addresses.keys():
        if len(addresses[k]) > 1:
            branches[k] = list(addresses[k])
    return branches

def main():
    branches = find_branches()
    f = open('itrace.out', 'r')
# Initializes variables
    hits = 0
    misses = 0
    total_branches = 0
    hits2 = 0
    misses2 = 0
    total_branches2 = 0
    flag = False
# Creates queues (FIFO) for each predictor (32 for 1-bit and 16 for 2-bit)
    q = []
    q2 = []
# For each line in itrace, flag is set when conditional branch is determined
# For each conditional branch, the simulator utilizes a predictor and logs hits/misses/total branches
    for line in f:
        line = line.strip()
        if flag:
            hits,misses,total_branches = one_bit_predictor(line,prev,branches,q,hits,misses,total_branches)
            hits2,misses2,total_branches2 = two_bit_counter(line,prev,branches,q2,hits2,misses2,total_branches2)
            flag = False
# If current line is in branches, then line is a conditional branch. Flag is set.
        if line in branches:
            flag = True
        prev = line
# Prints results/accuracy of each predictor
    print("Total Hits:     {} ({}%)".format(hits, (float(hits)/float(total_branches) * 100)))
    print("Total Misses:   {} ({}%)".format(misses, (float(misses)/float(total_branches) * 100)))
    print("Total Branches: {}".format(total_branches))
    print("32 1-bit Predictor History Table:    {}".format(q))
# Converts 16-bit counters to binary for appearance
    n = 0
    for i in q2:
        q2[n] = '{0:0b}'.format(i)
        n += 1
    print("Total Hits:     {} ({}%)".format(hits2, (float(hits2)/float(total_branches2) * 100)))
    print("Total Misses:   {} ({}%)".format(misses2, (float(misses2)/float(total_branches2) * 100)))
    print("Total Branches: {}".format(total_branches2))
    print("16 2-bit Predictor History Table:    {}".format(q2))
    print("Number of buffer misses (Unique Branches):   {}".format(len(branches)))

    f.close()

if __name__=="__main__":
    main()
