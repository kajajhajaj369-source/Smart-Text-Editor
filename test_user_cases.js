const fs = require('fs');

const testCases = [
    {
        num: 1,
        name: "Constant Time: O(1)",
        code: `#include <vector>
int getFirstElement(const std::vector<int>& arr) {
    if (arr.empty()) return -1;
    return arr[0];
}`,
        expected: "O(1)"
    },
    {
        num: 2,
        name: "Logarithmic Time: O(log n)",
        code: `#include <vector>
int binarySearch(const std::vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}`,
        expected: "O(log N)"
    },
    {
        num: 3,
        name: "Linear Time: O(n)",
        code: `#include <vector>
bool linearSearch(const std::vector<int>& arr, int target) {
    for (int num : arr) {
        if (num == target) return true;
    }
    return false;
}`,
        expected: "O(N)"
    },
    {
        num: 4,
        name: "Linearithmic Time: O(n log n)",
        code: `#include <vector>
#include <algorithm>
void sortArray(std::vector<int>& arr) {
    std::sort(arr.begin(), arr.end());
}`,
        expected: "O(N log N)"
    },
    {
        num: 5,
        name: "Quadratic Time: O(n^2)",
        code: `#include <vector>
bool hasDuplicates(const std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] == arr[j]) return true;
        }
    }
    return false;
}`,
        expected: "O(N^2)"
    },
    {
        num: 6,
        name: "Exponential Time: O(2^n)",
        code: `int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}`,
        expected: "O(2^N)"
    }
];

function analyzeCodeComplexity(code) {
    if (!code || !code.trim()) {
        return { tc: "O(1)", tag: "Constant Time", reason: "(Empty editor)", patterns: ["No executable code detected."], loopDepth: 0, hasRecursion: false };
    }

    const sanitized = code
        .replace(/"(?:[^"\\]|\\.)*"/g, '""')
        .replace(/'(?:[^'\\]|\\.)*'/g, "''")
        .replace(/\/\/.*$/gm, '')
        .replace(/\/\*[\s\S]*?\*\//g, '');

    // 1. Detect actual functions and their bodies
    const funcRegex = /(?:void|int|double|float|bool|string|auto|long|char)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*\{/g;
    let m;
    let functions = [];
    while ((m = funcRegex.exec(sanitized)) !== null) {
        const name = m[1];
        if (name === "if" || name === "for" || name === "while" || name === "switch") continue;
        const startIndex = m.index + m[0].length;
        let depth = 1;
        let endIndex = -1;
        for (let i = startIndex; i < sanitized.length; i++) {
            if (sanitized[i] === '{') depth++;
            else if (sanitized[i] === '}') {
                depth--;
                if (depth === 0) {
                    endIndex = i;
                    break;
                }
            }
        }
        if (endIndex !== -1) {
            const body = sanitized.substring(startIndex, endIndex);
            functions.push({ name, body });
        }
    }

    let recursiveFuncs = [];
    for (let fn of functions) {
        if (fn.name === "main") continue;
        const callRegex = new RegExp('\\b' + fn.name + '\\s*\\(', 'g');
        const internalCalls = (fn.body.match(callRegex) || []).length;
        if (internalCalls >= 1) {
            recursiveFuncs.push({ name: fn.name, callCount: internalCalls, body: fn.body });
        }
    }

    // 2. Detect well-known standard library algorithms / calls
    let stdAlgoComplexity = null;
    let stdAlgoReason = "";
    if (/\b(?:std::)?(?:sort|stable_sort)\s*\(/.test(sanitized)) {
        stdAlgoComplexity = "O(N log N)";
        stdAlgoReason = "(std::sort IntroSort / MergeSort algorithm)";
    } else if (/\b(?:std::)?(?:binary_search|lower_bound|upper_bound)\s*\(/.test(sanitized)) {
        stdAlgoComplexity = "O(log N)";
        stdAlgoReason = "(std::binary_search logarithmic divide-and-conquer)";
    }

    // 3. Loop detection & nesting hierarchy
    const lines = sanitized.split('\n');
    let maxLoopDepth = 0;
    let currentLoopDepth = 0;
    const patterns = [];
    
    const activeLoopStack = [];
    let bestChain = { depth: 0, logCount: 0, linearCount: 0 };

    // Supports:
    // for (init; cond; step)
    // for (auto x : container) -> Range-based for loop
    const forStandardRegex = /\bfor\s*\(([^;]*);([^;]*);([^)]*)\)/;
    const forRangeRegex = /\bfor\s*\([^:]+:[^)]+\)/;
    const whileRegex = /\bwhile\s*\(([^)]*)\)/;
    const logStepRegex = /(\/=|>>=|\*=|<<=|\/\s*2|\*\s*2|\bmid\b|\blow\s*\+|\bhigh\s*-|\b(j|i|k|n|step)\s*[\*\/])/;

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();
        if (!line) continue;

        let isLoopStart = false;
        let loopType = "";
        let isLog = false;

        const forMatch = line.match(forStandardRegex);
        const forRangeMatch = line.match(forRangeRegex);
        if (forMatch) {
            isLoopStart = true;
            loopType = "for loop";
            const stepExpr = forMatch[3] || "";
            if (logStepRegex.test(stepExpr) || logStepRegex.test(forMatch[2] || "")) {
                isLog = true;
            }
        } else if (forRangeMatch) {
            isLoopStart = true;
            loopType = "range-for loop";
            isLog = false; // Iterating through containers is O(N)
        } else {
            const whileMatch = line.match(whileRegex);
            if (whileMatch) {
                isLoopStart = true;
                loopType = "while loop";
                if (logStepRegex.test(whileMatch[1] || "")) {
                    isLog = true;
                }
            }
        }

        if (isLoopStart) {
            currentLoopDepth++;
            activeLoopStack.push({ isLog, line: i + 1, depth: currentLoopDepth });
            if (currentLoopDepth > maxLoopDepth) maxLoopDepth = currentLoopDepth;

            let currentLogs = activeLoopStack.filter(l => l.isLog).length;
            let currentLinears = activeLoopStack.length - currentLogs;
            if (activeLoopStack.length > bestChain.depth || 
               (activeLoopStack.length === bestChain.depth && currentLogs > bestChain.logCount)) {
                bestChain = { depth: activeLoopStack.length, logCount: currentLogs, linearCount: currentLinears };
            }

            patterns.push(`Line ${i + 1}: Detected ${loopType} [Depth ${currentLoopDepth}] -> ${isLog ? 'O(log N) step' : 'O(N) step'}`);
        }

        // Check for logarithmic operations inside loop body (e.g. j /= 2 or mid = left + (right-left)/2)
        if (activeLoopStack.length > 0 && !activeLoopStack[activeLoopStack.length - 1].isLog) {
            if (logStepRegex.test(line)) {
                activeLoopStack[activeLoopStack.length - 1].isLog = true;
                let currentLogs = activeLoopStack.filter(l => l.isLog).length;
                let currentLinears = activeLoopStack.length - currentLogs;
                if (activeLoopStack.length >= bestChain.depth) {
                    bestChain = { depth: activeLoopStack.length, logCount: currentLogs, linearCount: currentLinears };
                }
            }
        }

        // Count block close braces
        const openBraces = (line.match(/{/g) || []).length;
        const closeBraces = (line.match(/}/g) || []).length;
        const diff = closeBraces - openBraces;
        if (diff > 0 && activeLoopStack.length > 0) {
            for (let d = 0; d < diff && activeLoopStack.length > 0; d++) {
                activeLoopStack.pop();
            }
            currentLoopDepth = activeLoopStack.length;
        }
    }

    if (recursiveFuncs.length > 0) {
        for (let rf of recursiveFuncs) {
            patterns.push(`Function ${rf.name}(): Self-recursive calls detected (${rf.callCount} branch(es))`);
        }
    }

    if (stdAlgoComplexity) {
        patterns.push(`Standard Library Algorithm: ${stdAlgoReason}`);
    }

    // 4. Final Big-O Complexity Determination
    let tc = "O(1)";
    let tag = "Constant Time";
    let reason = "(Sequential statements)";

    if (recursiveFuncs.length > 0) {
        const topRec = recursiveFuncs[0];
        if (topRec.callCount >= 2) {
            tc = "O(2^N)";
            tag = "Exponential Time";
            reason = `(Multiple branching recursive calls in ${topRec.name})`;
        } else if (topRec.body.includes("/ 2") || topRec.body.includes(">> 1") || topRec.body.includes("/= 2")) {
            if (topRec.body.match(/\bfor\s*\(/) || topRec.body.match(/\bwhile\s*\(/)) {
                tc = "O(N log N)";
                tag = "Linearithmic (Divide & Conquer)";
                reason = `(Divide & conquer recursion with linear merge in ${topRec.name})`;
            } else {
                tc = "O(log N)";
                tag = "Logarithmic Time (Divide & Conquer)";
                reason = `(Binary recursive reduction in ${topRec.name})`;
            }
        } else {
            tc = "O(N)";
            tag = "Linear Recursion";
            reason = `(Single recursive step in ${topRec.name})`;
        }
    } else if (bestChain.depth >= 3) {
        if (bestChain.logCount > 0) {
            const exp = bestChain.linearCount > 0 ? (bestChain.linearCount > 1 ? `N^${bestChain.linearCount} ` : `N `) : "";
            tc = `O(${exp}log N)`;
            tag = `Polynomial-Logarithmic Time`;
            reason = `(Nested loop chain with depth ${bestChain.depth} and logarithmic stepping)`;
        } else {
            tc = `O(N^${bestChain.depth})`;
            tag = `Polynomial Time (N^${bestChain.depth})`;
            reason = `(Nested loop depth of ${bestChain.depth})`;
        }
    } else if (bestChain.depth === 2) {
        if (bestChain.logCount >= 2) {
            tc = "O(log^2 N)";
            tag = "Polylogarithmic Time";
            reason = "(Two nested logarithmic loops)";
        } else if (bestChain.logCount === 1) {
            tc = "O(N log N)";
            tag = "Linearithmic Time";
            reason = "(Nested loop: Outer O(N) loop × Inner O(log N) loop)";
        } else {
            tc = "O(N^2)";
            tag = "Quadratic Time";
            reason = "(Nested loop depth of 2)";
        }
    } else if (bestChain.depth === 1) {
        if (bestChain.logCount >= 1) {
            tc = "O(log N)";
            tag = "Logarithmic Time";
            reason = "(Single loop with step doubling or halving)";
        } else {
            tc = "O(N)";
            tag = "Linear Time";
            reason = "(Single linear loop)";
        }
    } else if (stdAlgoComplexity) {
        tc = stdAlgoComplexity;
        tag = stdAlgoComplexity === "O(N log N)" ? "Linearithmic Time" : "Logarithmic Time";
        reason = stdAlgoReason;
    }

    return {
        tc,
        tag,
        reason,
        patterns: patterns.length > 0 ? patterns : ["Sequential instructions without iteration or recursion."],
        loopDepth: bestChain.depth,
        hasRecursion: recursiveFuncs.length > 0
    };
}

console.log("=== RUNNING USER'S EXACT 6 PROGRAM TEST CASES ===\n");
let allPass = true;
for (let tc of testCases) {
    const res = analyzeCodeComplexity(tc.code);
    const pass = res.tc === tc.expected;
    console.log(`Test ${tc.num}: ${tc.name}`);
    console.log(`  -> Detected TC: ${res.tc} | Tag: ${res.tag}`);
    console.log(`  -> Reason: ${res.reason}`);
    console.log(`  -> Status: ${pass ? 'PASSED ✓' : 'FAILED ✗'}\n`);
    if (!pass) allPass = false;
}

if (allPass) {
    console.log(">>> ALL 6 USER TEST CASES PASSED PERFECTLY! <<<");
} else {
    process.exit(1);
}
