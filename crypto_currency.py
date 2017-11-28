import math, sys

def main():
    # read the input and build up the graph
    graph = {}; edges = {}; rates = {}; vertex_set = set()
    for line in sys.stdin:
        try:
            l_arr = line.strip().split(' ')
            src, dest, rate = l_arr[0], l_arr[1], float(l_arr[2])
        except:
            print('Incorrect input format'); exit(1)

        if src not in graph: graph[src] = set()
        graph[src].add(dest)
        edges[(src, dest)] = math.log(1 / rate)
        rates[(src, dest)] = rate
        vertex_set.add(src); vertex_set.add(dest)

    vertices = sorted(list(vertex_set))
    print(len(vertices))

    # floyd-warshall, detect if there's any negative cycle
    dist = {}; nexti = {}
    for i in vertices:
        for j in vertices:
            dist[(i, j)] = 0 if i == j else sys.maxsize
            if (i, j) in edges: dist[(i, j)] = edges[(i, j)]
            nexti[(i, j)] = ''
            if (i, j) in edges: nexti[(i, j)] = j

    for k in vertices:
        for i in vertices:
            for j in vertices:
                if dist[(i, k)] + dist[(k, j)] < dist[(i, j)]:
                    dist[(i, j)] = dist[(i, k)] + dist[(k, j)]
                    nexti[(i, j)] = nexti[(i, k)]
        for i in vertices:
            if dist[(i, i)] < 0:
                print(i, dist[(i, i)])
                u = v = i
                print(u, end=', ')
                print(rates[u, nexti[(u, v)]])
                u = nexti[(u, v)]
                while u != v:
                    print(u, end=', ')
                    print(rates[u, nexti[(u, v)]])
                    u = nexti[(u, v)]
                print(v)
                exit(0)

if __name__ == '__main__': main()

# BTC-BITF BCC-BITF 0.9980079880679485
# BTC-BTRX BCC-BTRX 6.233595800524935
