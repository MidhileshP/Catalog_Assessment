const fs = require('fs');

// Function to decode value based on its base
function decodeValue(value, base) {
    let result = 0;
    const len = value.length;
    for (let i = 0; i < len; i++) {
        let digit;
        if (/\d/.test(value[i])) {
            digit = value[i].charCodeAt(0) - '0'.charCodeAt(0);
        } else {
            digit = value[i].toUpperCase().charCodeAt(0) - 'A'.charCodeAt(0) + 10;
        }
        result = result * base + digit;
    }
    return result;
}

// Gaussian elimination function
function gaussianElimination(n, a, b) {
    const x = new Array(n).fill(0);

    // Forward elimination
    for (let i = 0; i < n; i++) {
        let maxRow = i;
        for (let j = i + 1; j < n; j++) {
            if (Math.abs(a[j][i]) > Math.abs(a[maxRow][i])) {
                maxRow = j;
            }
        }

        // Swap rows in matrix 'a' and vector 'b'
        if (maxRow !== i) {
            [a[i], a[maxRow]] = [a[maxRow], a[i]];
            [b[i], b[maxRow]] = [b[maxRow], b[i]];
        }

        // Check if the pivot element is close to zero
        if (Math.abs(a[i][i]) < 1e-9) {
            throw new Error("Matrix is singular or nearly singular");
        }

        // Eliminate column i
        for (let j = i + 1; j < n; j++) {
            const factor = a[j][i] / a[i][i];
            for (let k = i; k < n; k++) {
                a[j][k] -= factor * a[i][k];
            }
            b[j] -= factor * b[i];
        }
    }

    // Back substitution
    for (let i = n - 1; i >= 0; i--) {
        x[i] = b[i];
        for (let j = i + 1; j < n; j++) {
            x[i] -= a[i][j] * x[j];
        }
        x[i] /= a[i][i];
    }

    return x;
}

// Function to find the secret
function findSecret(tc) {
    const k = tc.keys.k;
    const a = Array.from({ length: k }, () => Array(k).fill(0));
    const b = Array(k).fill(0);

    for (let i = 0; i < k; i++) {
        const xi = i + 1;
        const yi = decodeValue(tc.points[i].value, tc.points[i].base);

        for (let j = 0; j < k; j++) {
            a[i][j] = Math.pow(xi, k - 1 - j);
        }
        b[i] = yi;
    }

    console.log("Matrix A before Gaussian elimination:", a);
    console.log("Vector B before Gaussian elimination:", b);

    const coefficients = gaussianElimination(k, a, b);

    console.log("Coefficients after Gaussian elimination:", coefficients);
    return coefficients;
}

// Function to check if a point lies on the curve
function isPointOnCurve(tc, index, coefficients) {
    const k = tc.keys.k;
    const x = index + 1;
    const y = decodeValue(tc.points[index].value, tc.points[index].base);
    let calculatedY = 0;

    for (let i = 0; i < k; i++) {
        calculatedY += coefficients[i] * Math.pow(x, k - 1 - i);
    }

    return Math.abs(y - calculatedY) < 1e-6;
}

// Function to solve each test case
function solveTestCase(tc) {
    const coefficients = findSecret(tc);
    const secret = coefficients[coefficients.length - 1];

    console.log(`Secret: ${Math.round(secret)}`);

    if (tc.keys.n > tc.keys.k) {
        let wrongPoints = [];
        for (let i = 0; i < tc.keys.n; i++) {
            if (!isPointOnCurve(tc, i, coefficients)) {
                wrongPoints.push(i + 1);
            }
        }
        if (wrongPoints.length > 0) {
            console.log("Wrong points: " + wrongPoints.join(', '));
        } else {
            console.log("Wrong points: None");
        }
    }
}

// Read and parse JSON test cases
function readTestCases(filename) {
    const data = fs.readFileSync(filename, 'utf8');
    return JSON.parse(data);
}

// Main execution
function main() {
    try {
        // Test Case 1
        const testCase1 = readTestCases('testcase1.json');
        console.log("Test Case 1:");
        solveTestCase(testCase1);
        console.log("\n");

        // Test Case 2
        const testCase2 = readTestCases('testcase2.json');
        console.log("Test Case 2:");
        solveTestCase(testCase2);
    } catch (error) {
        console.error("An error occurred:", error.message);
    }
}

main();
