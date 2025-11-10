#include <bits/stdc++.h>

using namespace std;

const long long MODULO = 1e9 + 7;

long long powerMod(long long base, long long exponent) {
    long long result = 1;
    while (exponent > 0) {
        if (exponent & 1) {
            result = (result * base) % MODULO;
        }
        base = (base * base) % MODULO;
        exponent >>= 1;
    }
    return result;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    long long numElements, numSegments, numQueries;
    cin >> numElements >> numSegments >> numQueries;

    vector<long long> breakpoints = {1, numElements + 1};
    vector<pair<long long, long long>> segmentRanges(numSegments);

    for (int i = 0; i < numSegments; i++) {
        long long left, right;
        cin >> left >> right;
        breakpoints.push_back(left);
        breakpoints.push_back(right + 1);
        segmentRanges[i] = {left, right + 1};
    }

    sort(breakpoints.begin(), breakpoints.end());
    breakpoints.erase(unique(breakpoints.begin(), breakpoints.end()), breakpoints.end());

    vector<long long> toggle(breakpoints.size(), 0);

    for (int i = 0; i < numSegments; i++) {
        int leftIndex = lower_bound(breakpoints.begin(), breakpoints.end(), segmentRanges[i].first) - breakpoints.begin();
        int rightIndex = lower_bound(breakpoints.begin(), breakpoints.end(), segmentRanges[i].second) - breakpoints.begin();

        toggle[leftIndex] ^= 1;
        toggle[rightIndex] ^= 1;
    }

    for (int i = 0; i + 1 < breakpoints.size(); i++) {
        toggle[i + 1] ^= toggle[i];
    }

    for (int i = breakpoints.size() - 1; i >= 0; i--) {
        toggle[i] = toggle[i - 1];
    }

    vector<long long> toggleSum(breakpoints.size(), 0), powerAccum(breakpoints.size(), 0);

    for (int i = 1; i < breakpoints.size(); i++) {
        powerAccum[i] = (powerAccum[i - 1] * powerMod(2, breakpoints[i] - breakpoints[i - 1])) % MODULO;
        powerAccum[i] += toggle[i] * (powerMod(2, breakpoints[i] - breakpoints[i - 1]) - 1);
        powerAccum[i] %= MODULO;

        toggleSum[i] = toggleSum[i - 1] + toggle[i] * (breakpoints[i] - breakpoints[i - 1]);
    }

    auto countToggles = [&](int l, int r) -> long long {
        if (l >= r) return 0;

        int leftIndex = lower_bound(breakpoints.begin(), breakpoints.end(), l + 1) - breakpoints.begin() - 1;
        int rightIndex = lower_bound(breakpoints.begin(), breakpoints.end(), r) - breakpoints.begin();

        if (leftIndex + 1 == rightIndex) {
            return toggle[leftIndex + 1] * (r - l);
        }

        long long result = toggleSum[rightIndex - 1] - toggleSum[leftIndex + 1];
        result += toggle[leftIndex + 1] * (breakpoints[leftIndex + 1] - l);
        result += toggle[rightIndex] * (r - breakpoints[rightIndex - 1]);
        return result;
    };

    auto computeFinal = [&](int l, int r) -> long long {
        if (l >= r) return 0;

        int leftIndex = lower_bound(breakpoints.begin(), breakpoints.end(), l + 1) - breakpoints.begin() - 1;
        int rightIndex = lower_bound(breakpoints.begin(), breakpoints.end(), r) - breakpoints.begin();

        if (leftIndex + 1 == rightIndex) {
            return toggle[leftIndex + 1] * (powerMod(2, r - l) - 1);
        }

        long long midResult = (powerAccum[rightIndex - 1] - powerAccum[leftIndex + 1] * powerMod(2, breakpoints[rightIndex - 1] - breakpoints[leftIndex + 1]) % MODULO) % MODULO;
        midResult = (midResult * powerMod(2, r - breakpoints[rightIndex - 1])) % MODULO;

        long long startResult = (toggle[leftIndex + 1] * (powerMod(2, breakpoints[leftIndex + 1] - l) - 1) * powerMod(2, r - breakpoints[leftIndex + 1])) % MODULO;
        long long endResult = toggle[rightIndex] * (powerMod(2, r - breakpoints[rightIndex - 1]) - 1);

        return ((startResult + midResult + endResult) % MODULO + MODULO) % MODULO;
    };

    for (int i = 0; i < numQueries; i++) {
        int left, right, threshold;
        cin >> left >> right >> threshold;
        right += 1;

        if (countToggles(left, right) >= threshold) {
            long long result = powerMod(2, threshold) - 1;
            if (result < 0) result += MODULO;
            cout << result << "\n";
            continue;
        }

        int lower = left, upper = right;
        while (lower < upper) {
            int mid = (lower + upper) / 2 + 1;
            if (countToggles(left, mid) + right - mid >= threshold) {
                lower = mid;
            } else {
                upper = mid - 1;
            }
        }

        long long toggles = countToggles(left, lower);
        long long result = (powerMod(2, toggles) - 1) * powerMod(2, right - lower) % MODULO;
        result += computeFinal(lower, right);
        result = (result % MODULO + MODULO) % MODULO;

        cout << result << "\n";
    }
}