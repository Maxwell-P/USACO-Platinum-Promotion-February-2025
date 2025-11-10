#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int nodes, edges;
    cin >> nodes >> edges;
    int fullSet = 1 << nodes;

    vector<vector<int>> adjMatrix(nodes, vector<int>(nodes, 0));
    vector<int> bitAdj(nodes, 0);
    for (int i = 0; i < edges; i++){
        int u, v;
        cin >> u >> v;
        u--; v--;
        adjMatrix[u][v] = adjMatrix[v][u] = 1;
        bitAdj[u] |= (1 << v);
        bitAdj[v] |= (1 << u);
    }

    int allNodes = (1 << nodes) - 1;
    vector<int> missingEdges(nodes, 0);
    for (int i = 0; i < nodes; i++){
        int complement = allNodes ^ (1 << i);
        missingEdges[i] = complement & ~(bitAdj[i]);
    }

    vector<int> subsetSize(fullSet, 0);
    for (int subset = 0; subset < fullSet; subset++){
        subsetSize[subset] = __builtin_popcount(subset);
    }

    vector<int> edgeCounts(fullSet, 0);
    for (int subset = 0; subset < fullSet; subset++){
        int count = 0;
        for (int i = 0; i < nodes; i++){
            if(subset & (1 << i)){
                for (int j = i + 1; j < nodes; j++){
                    if(subset & (1 << j)){
                        count += adjMatrix[i][j];
                    }
                }
            }
        }
        edgeCounts[subset] = count;
    }

    vector<int> partitionCost(fullSet, 0);
    for (int subset = 0; subset < fullSet; subset++){
        if(subsetSize[subset] <= 1) {
            partitionCost[subset] = 0;
        } else {
            int totalPairs = subsetSize[subset] * (subsetSize[subset] - 1) / 2;
            int costA = edgeCounts[subset];
            int costB = totalPairs - edgeCounts[subset];
            partitionCost[subset] = min(costA, costB);
        }
    }

    const int INF = 1e9;
    vector<int> minCost(fullSet, INF);
    minCost[0] = 0;

    for (int subset = 1; subset < fullSet; subset++){
        for (int submask = subset; submask; submask = (submask - 1) & subset){
            int remaining = subset ^ submask;
            int interCost = 0;
            int temp = submask;
            while(temp){
                int bitIdx = __builtin_ctz(temp);
                interCost += __builtin_popcount(missingEdges[bitIdx] & remaining);
                temp &= (temp - 1);
            }
            minCost[subset] = min(minCost[subset], minCost[remaining] + partitionCost[submask] + interCost);
        }
    }

    cout << minCost[fullSet - 1] << "\n";
    return 0;
}