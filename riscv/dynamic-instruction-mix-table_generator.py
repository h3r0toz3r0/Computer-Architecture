store =  ['stp', 'str', 'strb', 'strh', 'stur', 'msr', 'stxr', 'stlxr']
load =   ['ldr', 'movk', 'movz', 'ubfm', 'sbfm', 'ldur', 'ldp','ldrb', 'adr', 'ldrh', 'bfm', 'movn', 'ld1', 'ldurb', 'mrs', 'ldaxr', 'ldxr', 'ldrsw']
alu =    ['add', 'sub', 'subs', 'adrp', 'orr', 'adds', 'and', 'svc', 'bic', 'bics', 'rev', 'clz', 'eor', 'madd', 'umaddl', 'lslv', 'umulh', 'cmeq', 'addp', 'lsrv', 'rbit', 'nop', 'orn', 'asrv', 'sys', 'dmb', 'udiv', 'msub', 'sdiv', 'ands', 'xx']
jump =   ['ret']
branch = ['bl', 'bcond', 'b', 'cbz', 'cbnz', 'csel', 'br', 'ccmp', 'blr', 'csinc', 'tbz', 'tbnz', 'csinv']

def find_unique_ops():
        cmds = []
        with open('qsort.log', 'r') as f:
                for line in f:
                        line = line.strip().split(',')[1]
                        if line not in cmds:
                                cmds.append(line)
                                print("Adding: {}".format(line))


def main():
        stores = 0 
        loads = 0 
        alus = 0 
        jumps = 0 
        branches = 0 
        instructions = 0 
        with open('qsort.log', 'r') as f:
                for line in f:
                        line = line.strip().split(',')
			try:
                        	line = line[1]
			except Exception as e:
				continue
                        if line in store:
                                stores += 1
                        elif line in load:
                                loads += 1
                        elif line in alu:
                                alus += 1
                        elif line in jump:
                                jumps += 1
                        elif line in branch:
                                branches += 1
                        else:
                                print("Unrecognized Argument! {}".format(line))
                        instructions += 1
         
        print("Total instructions: {}".format(instructions))
        print("Stores: {} ({}%)".format(stores, (float(stores)/float(instructions)) * 100.0))
        print("Loads: {} ({}%)".format(stores, (float(loads)/float(instructions)) * 100.0))
        print("ALUs: {} ({}%)".format(stores, (float(alus)/float(instructions)) * 100.0))
        print("Branches: {} ({}%)".format(stores, (float(branches)/float(instructions)) * 100.0))
        print("Jumps: {} ({}%)".format(stores, (float(jumps)/float(instructions)) * 100.0))

if __name__ == '__main__':
        # find_unique_ops()
        main()

