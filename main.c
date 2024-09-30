#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_N 10
#define MAX_K 10
#define MAX_VALUE_LEN 20

typedef struct {
    int n;
    int k;
} Keys;

typedef struct {
    int base;
    char value[MAX_VALUE_LEN];
} Point;

typedef struct {
    Keys keys;
    Point points[MAX_N];
} TestCase;

long long decode_value(char* value, int base) {
    long long result = 0;
    int len = strlen(value);
    for (int i = 0; i < len; i++) {
        int digit;
        if (isdigit(value[i])) {
            digit = value[i] - '0';
        } else {
            digit = toupper(value[i]) - 'A' + 10;
        }
        result = result * base + digit;
    }
    return result;
}

void gaussian_elimination(int n, double a[MAX_K][MAX_K], double b[MAX_K], double x[MAX_K]) {
    for (int i = 0; i < n; i++) {
        int max_row = i;
        for (int j = i + 1; j < n; j++) {
            if (fabs(a[j][i]) > fabs(a[max_row][i])) {
                max_row = j;
            }
        }
        for (int j = i; j < n; j++) {
            double temp = a[i][j];
            a[i][j] = a[max_row][j];
            a[max_row][j] = temp;
        }
        double temp = b[i];
        b[i] = b[max_row];
        b[max_row] = temp;

        for (int j = i + 1; j < n; j++) {
            double factor = a[j][i] / a[i][i];
            for (int k = i; k < n; k++) {
                a[j][k] -= factor * a[i][k];
            }
            b[j] -= factor * b[i];
        }
    }

    for (int i = n - 1; i >= 0; i--) {
        x[i] = b[i];
        for (int j = i + 1; j < n; j++) {
            x[i] -= a[i][j] * x[j];
        }
        x[i] /= a[i][i];
    }
}

double find_secret(TestCase *tc) {
    int k = tc->keys.k;
    double a[MAX_K][MAX_K] = {0};
    double b[MAX_K] = {0};
    double x[MAX_K] = {0};

    for (int i = 0; i < k; i++) {
        double xi = i + 1;
        double yi = decode_value(tc->points[i].value, tc->points[i].base);

        for (int j = 0; j < k; j++) {
            a[i][j] = pow(xi, k - 1 - j);
        }
        b[i] = yi;
    }

    gaussian_elimination(k, a, b, x);

    return x[k - 1];
}

int is_point_on_curve(TestCase *tc, int index, double secret) {
    int k = tc->keys.k;
    double x = index + 1;
    double y = decode_value(tc->points[index].value, tc->points[index].base);
    double calculated_y = 0;

    for (int i = 0; i < k - 1; i++) {
        calculated_y += pow(x, k - 1 - i) * (y - secret) / pow(x, k - 1);
    }
    calculated_y += secret;

    return fabs(y - calculated_y) < 1e-6;
}

void solve_test_case(TestCase *tc) {
    double secret = find_secret(tc);
    printf("Secret: %.0f\n", secret);

    if (tc->keys.n > tc->keys.k) {
        printf("Wrong points: ");
        int wrong_count = 0;
        for (int i = 0; i < tc->keys.n; i++) {
            if (!is_point_on_curve(tc, i, secret)) {
                printf("%d ", i + 1);
                wrong_count++;
            }
        }
        if (wrong_count == 0) {
            printf("None");
        }
        printf("\n");
    }
}

int main() {
    TestCase tc1 = {
        .keys = {4, 3},
        .points = {
            {10, "4"},
            {2, "111"},
            {10, "12"},
            {4, "213"}
        }
    };

    TestCase tc2 = {
        .keys = {9, 6},
        .points = {
            {10, "28735619723837"},
            {16, "1A228867F0CA"},
            {12, "32811A4AA0B7B"},
            {11, "917978721331A"},
            {16, "1A22886782E1"},
            {10, "28735619654702"},
            {14, "71AB5070CC4B"},
            {9, "122662581541670"},
            {8, "642121030037605"}
        }
    };

    printf("Test Case 1:\n");
    solve_test_case(&tc1);

    printf("\nTest Case 2:\n");
    solve_test_case(&tc2);

    return 0;
}
