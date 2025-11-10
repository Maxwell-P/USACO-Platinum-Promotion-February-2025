#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const ll INF_LIMIT = (1LL << 60);

int totalNodes;
vector<int> nextNode;
vector<ll> nodeCostValue;
vector<int> visitStatus;
vector<bool> isCycleNode;
vector<int> componentId;
vector<vector<int>> cycleComponents;
vector<vector<int>> treeChildren;
vector<vector<ll>> dpValues;
vector<bool> hasProcessed;

ll getNodeCost(int index) {
    return (nextNode[index] == index ? 0LL : nodeCostValue[index]);
}

void detectCycles() {
    int componentIndex = 0;
    visitStatus.assign(totalNodes + 1, 0);
    isCycleNode.assign(totalNodes + 1, false);
    componentId.assign(totalNodes + 1, -1);

    for (int i = 1; i <= totalNodes; i++) {
        if (visitStatus[i] != 0) continue;
        int current = i;
        vector<int> traversalPath;
        while (true) {
            if (visitStatus[current] == 0) {
                visitStatus[current] = 1;
                traversalPath.push_back(current);
                current = nextNode[current];
            } else if (visitStatus[current] == 1) {
                vector<int> cycle;
                int startIdx = 0;
                for (int j = 0; j < (int)traversalPath.size(); j++) {
                    if (traversalPath[j] == current) { startIdx = j; break; }
                }
                for (int j = startIdx; j < (int)traversalPath.size(); j++) {
                    cycle.push_back(traversalPath[j]);
                    isCycleNode[traversalPath[j]] = true;
                    componentId[traversalPath[j]] = componentIndex;
                }
                cycleComponents.push_back(cycle);
                componentIndex++;
                for (int node : traversalPath)
                    visitStatus[node] = 2;
                break;
            } else {
                for (int node : traversalPath)
                    visitStatus[node] = 2;
                break;
            }
        }
    }
}

void computeTreeDP(int node) {
    hasProcessed[node] = true;
    dpValues[node][0] = 0;
    dpValues[node][1] = getNodeCost(node);
    for (int child : treeChildren[node]) {
        if (isCycleNode[child]) continue;
        if (!hasProcessed[child]) computeTreeDP(child);
        dpValues[node][0] += dpValues[child][1];
        dpValues[node][1] += min(dpValues[child][0], dpValues[child][1]);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> totalNodes;
    nextNode.resize(totalNodes + 1);
    nodeCostValue.resize(totalNodes + 1);

    for (int i = 1; i <= totalNodes; i++) {
        cin >> nextNode[i];
    }
    for (int i = 1; i <= totalNodes; i++) {
        cin >> nodeCostValue[i];
    }

    detectCycles();

    treeChildren.assign(totalNodes + 1, vector<int>());
    for (int i = 1; i <= totalNodes; i++) {
        treeChildren[nextNode[i]].push_back(i);
    }

    dpValues.assign(totalNodes + 1, vector<ll>(2, 0));
    hasProcessed.assign(totalNodes + 1, false);
    for (int i = 1; i <= totalNodes; i++) {
        if (!isCycleNode[i] && !hasProcessed[i])
            computeTreeDP(i);
    }

    vector<ll> extraCost(totalNodes + 1, 0), baseCost(totalNodes + 1, 0);
    for (int i = 1; i <= totalNodes; i++){
        if (!isCycleNode[i]) continue;
        ll extra = 0, base = 0;
        for (int child : treeChildren[i]){
            if (isCycleNode[child]) continue;
            extra += min(dpValues[child][0], dpValues[child][1]);
            base += dpValues[child][1];
        }
        extraCost[i] = extra;
        baseCost[i] = base;
    }

    ll totalMinCost = 0;
    for (auto &cycle : cycleComponents) {
        int cycleSize = cycle.size();
        if (cycleSize == 0) continue;
        vector<ll> selectCost(cycleSize), deselectCost(cycleSize);
        for (int i = 0; i < cycleSize; i++) {
            int node = cycle[i];
            selectCost[i] = getNodeCost(node) + extraCost[node];
            deselectCost[i] = baseCost[node];
        }

        ll minCycleCost = INF_LIMIT;
        vector<vector<ll>> dpCycle(cycleSize, vector<ll>(2, INF_LIMIT));
        dpCycle[0][1] = selectCost[0];
        for (int i = 1; i < cycleSize; i++) {
            dpCycle[i][0] = dpCycle[i-1][1] + deselectCost[i];
            dpCycle[i][1] = min(dpCycle[i-1][0], dpCycle[i-1][1]) + selectCost[i];
        }
        minCycleCost = min(dpCycle[cycleSize-1][0], dpCycle[cycleSize-1][1]);

        if (cycleSize > 1) {
            vector<vector<ll>> altDpCycle(cycleSize, vector<ll>(2, INF_LIMIT));
            altDpCycle[0][0] = deselectCost[0];
            altDpCycle[1][1] = altDpCycle[0][0] + selectCost[1];
            for (int i = 2; i < cycleSize; i++) {
                altDpCycle[i][0] = altDpCycle[i-1][1] + deselectCost[i];
                altDpCycle[i][1] = min(altDpCycle[i-1][0], altDpCycle[i-1][1]) + selectCost[i];
            }
            minCycleCost = min(minCycleCost, altDpCycle[cycleSize-1][1]);
        }
        totalMinCost += minCycleCost;
    }

    cout << totalMinCost << "\n";
    return 0;
}