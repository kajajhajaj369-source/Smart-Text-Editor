const fs = require('fs');

const code = `#include <vector>
#include <algorithm>
int countUnique(std::vector<int> arr) {
    if (arr.empty()) return 0;
    std::sort(arr.begin(), arr.end());
    int count = 1;
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i] != arr[i - 1]) count++;
    }
    return count;
}`;

// Current logic test
function testCurrent(code) {
    const sanitized = code
        .replace(/"(?:[^"\\]|\\.)*"/g, '""')
        .replace(/'(?:[^'\\]|\\.)*'/g, "''")
        .replace(/\/\/.*$/gm, '')
        .replace(/\/\*[\s\S]*?\*\//g, '');

    let stdAlgoComplexity = null;
    let stdAlgoReason = "";
    if (/\b(?:std::)?(?:sort|stable_sort)\s*\(/.test(sanitized)) {
        stdAlgoComplexity = "O(N log N)";
        stdAlgoReason = "(std::sort IntroSort / MergeSort algorithm)";
    }

    const forStandardRegex = /\bfor\s*\(([^;]*);([^;]*);([^)]*)\)/;
    const lines = sanitized.split('\n');
    let bestChain = { depth: 0, logCount: 0, linearCount: 0 };
    for (let l of lines) {
        if (forStandardRegex.test(l)) {
            bestChain = { depth: 1, logCount: 0, linearCount: 1 };
        }
    }

    console.log("bestChain depth:", bestChain.depth);
    console.log("stdAlgoComplexity:", stdAlgoComplexity);

    // Current if/else ladder:
    let tc = "O(1)";
    if (bestChain.depth === 1) {
        tc = "O(N)";
    } else if (stdAlgoComplexity) {
        tc = stdAlgoComplexity;
    }
    console.log("Output was:", tc);
}

testCurrent(code);
