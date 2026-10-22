const fs = require('fs');

function getDominantComplexity(candidates) {
    // Ranks from lowest (0) to highest (7)
    const RANK_MAP = {
        "O(1)": 0,
        "O(log N)": 1,
        "O(N)": 2,
        "O(N log N)": 3,
        "O(N^2)": 4,
        "O(N^3)": 5,
        "O(N^4)": 6,
        "O(2^N)": 7
    };

    let highest = { tc: "O(1)", tag: "Constant Time", reason: "(Sequential statements)", rank: 0 };

    for (let c of candidates) {
        let rank = RANK_MAP[c.tc] !== undefined ? RANK_MAP[c.tc] : 2;
        if (c.tc.startsWith("O(N^")) {
            let p = parseInt(c.tc.replace("O(N^", "").replace(")", "")) || 2;
            rank = 3 + p; // e.g. N^2 -> 5
        }
        if (rank > highest.rank) {
            highest = { tc: c.tc, tag: c.tag, reason: c.reason, rank: rank };
        }
    }
    return highest;
}

function analyzeCodeComplexity(code) {
    if (!code || !code.trim()) {
        return { tc: "O(1)", tag: "Constant Time", reason: "(Empty editor)", patterns: ["No executable code detected."], loopDepth: 0, hasRecursion: false };
    }

    const sanitized = code
        .replace(/"(?:[^"\\]|\\.)*"/g, '""')
        .replace(/'(?:[^'\\]|\\.)*'/g, "''")
        .replace(/\/\/.*$/gm, '')
        .replace(/\/\*[\s\S]*?\*\//g, '');

    // 1. Detect actual functions and their bodies for TRUE self-recursion
    const funcRegex = /(?:void|int|double|float|bool|string|auto|long|char|size_t)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*\{/g;
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

    const patterns = [];
    const candidates = [];

    // Check standard library algorithms
    if (/\b(?:std::)?(?:sort|stable_sort)\s*\(/.test(sanitized)) {
        candidates.push({
            tc: "O(N log N)",
            tag: "Linearithmic Time",
            reason: "(std::sort IntroSort / MergeSort dominant algorithm)"
        });
        patterns.push("Standard Library: std::sort -> O(N log N)");
    }
    if (/\b(?:std::)?(?:binary_search|lower_bound|upper_bound)\s*\(/.test(sanitized)) {
        candidates.push({
            tc: "O(log N)",
            tag: "Logarithmic Time",
            reason: "(std::binary_search logarithmic divide-and-conquer)"
        });
        patterns.push("Standard Library: std::binary_search -> O(log N)");
    }

    // Check recursion
    if (recursiveFuncs.length > 0) {
        const topRec = recursiveFuncs[0];
        patterns.push(`Function ${topRec.name}(): Self-recursive calls detected (${topRec.callCount} branch(es))`);
        if (topRec.callCount >= 2) {
            candidates.push({
                tc: "O(2^N)",
                tag: "Exponential Time",
                reason: `(Multiple branching recursive calls in ${topRec.name})`
            });
        } else if (topRec.body.includes("/ 2") || topRec.body.includes(">> 1") || topRec.body.includes("/= 2")) {
            if (topRec.body.match(/\bfor\s*\(/) || topRec.body.match(/\bwhile\s*\(/)) {
                candidates.push({
                    tc: "O(N log N)",
                    tag: "Linearithmic (Divide & Conquer)",
                    reason: `(Divide & conquer recursion with linear merge in ${topRec.name})`
                });
            } else {
                candidates.push({
                    tc: "O(log N)",
                    tag: "Logarithmic Time (Divide & Conquer)",
                    reason: `(Binary recursive reduction in ${topRec.name})`
                });
            }
        } else {
            candidates.push({
                tc: "O(N)",
                tag: "Linear Recursion",
                reason: `(Single recursive step in ${topRec.name})`
            });
        }
    }

    // Loop analysis
    const lines = sanitized.split('\n');
    let maxLoopDepth = 0;
    let currentLoopDepth = 0;
    const activeLoopStack = [];
    let bestChain = { depth: 0, logCount: 0, linearCount: 0 };

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
            isLog = false;
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

    if (bestChain.depth >= 3) {
        if (bestChain.logCount > 0) {
            const exp = bestChain.linearCount > 0 ? (bestChain.linearCount > 1 ? `N^${bestChain.linearCount} ` : `N `) : "";
            candidates.push({
                tc: `O(${exp}log N)`,
                tag: "Polynomial-Logarithmic Time",
                reason: `(Nested loop chain with depth ${bestChain.depth} and logarithmic stepping)`
            });
        } else {
            candidates.push({
                tc: `O(N^${bestChain.depth})`,
                tag: `Polynomial Time (N^${bestChain.depth})`,
                reason: `(Nested loop depth of ${bestChain.depth})`
            });
        }
    } else if (bestChain.depth === 2) {
        if (bestChain.logCount >= 2) {
            candidates.push({
                tc: "O(log^2 N)",
                tag: "Polylogarithmic Time",
                reason: "(Two nested logarithmic loops)"
            });
        } else if (bestChain.logCount === 1) {
            candidates.push({
                tc: "O(N log N)",
                tag: "Linearithmic Time",
                reason: "(Nested loop: Outer O(N) loop × Inner O(log N) loop)"
            });
        } else {
            candidates.push({
                tc: "O(N^2)",
                tag: "Quadratic Time",
                reason: "(Nested loop depth of 2)"
            });
        }
    } else if (bestChain.depth === 1) {
        if (bestChain.logCount >= 1) {
            candidates.push({
                tc: "O(log N)",
                tag: "Logarithmic Time",
                reason: "(Single loop with step doubling or halving)"
            });
        } else {
            candidates.push({
                tc: "O(N)",
                tag: "Linear Time",
                reason: "(Single linear loop)"
            });
        }
    }

    if (candidates.length === 0) {
        candidates.push({
            tc: "O(1)",
            tag: "Constant Time",
            reason: "(Sequential statements)"
        });
    }

    const dominant = getDominantComplexity(candidates);

    return {
        tc: dominant.tc,
        tag: dominant.tag,
        reason: dominant.reason,
        patterns: patterns.length > 0 ? patterns : ["Sequential instructions without iteration or recursion."],
        loopDepth: bestChain.depth,
        hasRecursion: recursiveFuncs.length > 0
    };
}

// Test countUnique
const countUniqueCode = `#include <vector>
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

const res = analyzeCodeComplexity(countUniqueCode);
console.log("countUnique result:", res.tc, "| Tag:", res.tag, "| Reason:", res.reason);

if (res.tc === "O(N log N)") {
    console.log("SUCCESS: Dominant term correctly resolved to O(N log N)!");
} else {
    console.log("FAILED: Expected O(N log N), got", res.tc);
    process.exit(1);
}
