const fs = require('fs');

function analyzeCodeComplexity(code) {
    if (!code || !code.trim()) {
        return { tc: "O(1)", tag: "Constant Time", reason: "(Empty editor)", patterns: ["No executable code detected."], loopDepth: 0, hasRecursion: false };
    }

    // Strip string literals, char literals, and comments
    const sanitized = code
        .replace(/"(?:[^"\\]|\\.)*"/g, '""')
        .replace(/'(?:[^'\\]|\\.)*'/g, "''")
        .replace(/\/\/.*$/gm, '')
        .replace(/\/\*[\s\S]*?\*\//g, '');

    // 1. Detect actual functions and their bodies to check for TRUE self-recursion
    // Matches: return_type func_name(args) { body }
    const funcRegex = /(?:void|int|double|float|bool|string|auto|long|char)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*\{/g;
    let m;
    let functions = [];
    while ((m = funcRegex.exec(sanitized)) !== null) {
        const name = m[1];
        if (name === "if" || name === "for" || name === "while" || name === "switch") continue;
        const startIndex = m.index + m[0].length;
        // Find matching close brace for this function body
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

    // 2. Loop detection & nesting hierarchy
    const lines = sanitized.split('\n');
    let maxLoopDepth = 0;
    let currentLoopDepth = 0;
    const patterns = [];
    
    // Track loop at each active depth: { isLog: boolean, line: number }
    const activeLoopStack = [];
    // Record maximal nested chain properties
    let bestChain = { depth: 0, logCount: 0, linearCount: 0 };

    const forRegex = /\bfor\s*\(([^;]*);([^;]*);([^)]*)\)/;
    const whileRegex = /\bwhile\s*\(([^)]*)\)/;
    const logStepRegex = /(\/=|>>=|\*=|<<=|\/\s*2|\*\s*2|\bmid\b|\blow\s*\+|\bhigh\s*-|\b(j|i|k|n|step)\s*[\*\/])/;

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();
        if (!line) continue;

        let isLoopStart = false;
        let loopType = "";
        let isLog = false;

        const forMatch = line.match(forRegex);
        if (forMatch) {
            isLoopStart = true;
            loopType = "for loop";
            // Check step (3rd expression) or condition/body
            const stepExpr = forMatch[3] || "";
            if (logStepRegex.test(stepExpr) || logStepRegex.test(forMatch[2] || "")) {
                isLog = true;
            }
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

            // Check if current chain exceeds bestChain
            let currentLogs = activeLoopStack.filter(l => l.isLog).length;
            let currentLinears = activeLoopStack.length - currentLogs;
            if (activeLoopStack.length > bestChain.depth || 
               (activeLoopStack.length === bestChain.depth && currentLogs > bestChain.logCount)) {
                bestChain = { depth: activeLoopStack.length, logCount: currentLogs, linearCount: currentLinears };
            }

            patterns.push(`Line ${i + 1}: Detected ${loopType} [Depth ${currentLoopDepth}] -> ${isLog ? 'O(log N) step' : 'O(N) step'}`);
        }

        // Check for logarithmic operations inside loop body (e.g. j /= 2; inside while or for body)
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
            patterns.push(`Line Function ${rf.name}(): Self-recursive calls detected (${rf.callCount} branch(es))`);
        }
    }

    // 3. Final Big-O Complexity Determination
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
            // Check if there is an O(N) loop inside the recursive function (like MergeSort)
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

// TEST SUITE WITH EVERY COMPLEXITY
const testCases = [
    {
        name: "Constant O(1)",
        code: `int main() { int a = 5; int b = 10; return a + b; }`,
        expected: "O(1)"
    },
    {
        name: "Linear O(N)",
        code: `void func(int n) { for (int i = 0; i < n; i++) { std::cout << i; } }`,
        expected: "O(N)"
    },
    {
        name: "Logarithmic O(log N) - for loop",
        code: `void binarySearch(int n) { for (int i = 1; i < n; i *= 2) { } }`,
        expected: "O(log N)"
    },
    {
        name: "Logarithmic O(log N) - while loop",
        code: `void divLoop(int n) { while (n > 0) { n /= 2; } }`,
        expected: "O(log N)"
    },
    {
        name: "Linearithmic O(N log N) - USER CASE",
        code: `#include <iostream>
void linearithmicFunction(int n) {
    int totalIterations = 0;
    // Outer loop runs 'n' times -> O(n)
    for (int i = 0; i < n; i++) {
        // Inner loop cuts 'j' in half each step -> O(log n)
        for (int j = n; j > 0; j /= 2) {
            totalIterations++;
        }
    }
    std::cout << totalIterations;
}
int main() {
    int n = 8;
    linearithmicFunction(n);
    return 0;
}`,
        expected: "O(N log N)"
    },
    {
        name: "Quadratic O(N^2)",
        code: `void bubbleSort(int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            swap(a[i], a[j]);
        }
    }
}`,
        expected: "O(N^2)"
    },
    {
        name: "Cubic O(N^3)",
        code: `void matrixMul(int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}`,
        expected: "O(N^3)"
    },
    {
        name: "Linear Recursion O(N)",
        code: `void countDown(int n) {
    if (n <= 0) return;
    std::cout << n;
    countDown(n - 1);
}`,
        expected: "O(N)"
    },
    {
        name: "Exponential Recursion O(2^N)",
        code: `int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}`,
        expected: "O(2^N)"
    }
];

let allPassed = true;
for (let t of testCases) {
    const res = analyzeCodeComplexity(t.code);
    const pass = res.tc === t.expected;
    console.log(`[${pass ? 'PASS' : 'FAIL'}] ${t.name}: Expected ${t.expected}, Got ${res.tc} (Reason: ${res.reason})`);
    if (!pass) allPassed = false;
}

if (!allPassed) {
    process.exit(1);
} else {
    console.log("\n>>> ALL TEST CASES PASSED ACCURATELY! <<<");
}
