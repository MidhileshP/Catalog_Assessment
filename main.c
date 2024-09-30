#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "cJSON.h"  // Include the cJSON library

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

void find_secret(TestCase *tc, double *coefficients) {
    int k = tc->keys.k;
    double a[MAX_K][MAX_K] = {0};
    double b[MAX_K] = {0};

    for (int i = 0; i < k; i++) {
        double xi = i + 1;
        double yi = decode_value(tc->points[i].value, tc->points[i].base);

        for (int j = 0; j < k; j++) {
            a[i][j] = pow(xi, k - 1 - j);
        }
        b[i] = yi;
    }

    gaussian_elimination(k, a, b, coefficients);
}

int is_point_on_curve(TestCase *tc, int index, double *coefficients) {
    int k = tc->keys.k;
    double x = index + 1;
    double y = decode_value(tc->points[index].value, tc->points[index].base);
    double calculated_y = 0;

    for (int i = 0; i < k; i++) {
        calculated_y += coefficients[i] * pow(x, k - 1 - i);
    }

    return fabs(y - calculated_y) < 1e-6;
}

void solve_test_case(TestCase *tc, int test_case_number) {
    double coefficients[MAX_K] = {0};
    find_secret(tc, coefficients);

    // Print the secret (constant term of the polynomial)
    printf("Test Case %d - Secret: %.0f\n", test_case_number, coefficients[tc->keys.k - 1]);

    if (test_case_number == 2 && tc->keys.n > tc->keys.k) {
        // Check for wrong points (applies to Test Case 2)
        printf("Test Case 2 - Wrong points: ");
        int wrong_count = 0;
        for (int i = 0; i < tc->keys.n; i++) {
            if (!is_point_on_curve(tc, i, coefficients)) {
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

TestCase* read_test_case_from_json(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening JSON file");
        return NULL;
    }

    // Find the size of the file and read its contents
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* json_buffer = (char*)malloc(file_size + 1);
    if (json_buffer == NULL) {
        printf("Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fread(json_buffer, 1, file_size, file);
    json_buffer[file_size] = '\0';
    fclose(file);

    // Parse the JSON
    cJSON* json = cJSON_Parse(json_buffer);
    free(json_buffer);

    if (!json) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    // Allocate the test case structure
    TestCase* test_case = (TestCase*)malloc(sizeof(TestCase));
    if (test_case == NULL) {
        printf("Memory allocation for TestCase failed\n");
        cJSON_Delete(json);
        return NULL;
    }

    test_case->keys.n = cJSON_GetObjectItem(json, "n")->valueint;
    test_case->keys.k = cJSON_GetObjectItem(json, "k")->valueint;

    cJSON* points = cJSON_GetObjectItem(json, "points");
    if (!cJSON_IsArray(points)) {
        printf("Invalid format for points array\n");
        cJSON_Delete(json);
        free(test_case);
        return NULL;
    }

    for (int i = 0; i < test_case->keys.n; i++) {
        cJSON* point = cJSON_GetArrayItem(points, i);
        if (point == NULL) {
            printf("Error reading point %d\n", i);
            cJSON_Delete(json);
            free(test_case);
            return NULL;
        }

        test_case->points[i].base = cJSON_GetObjectItem(point, "base")->valueint;
        strcpy(test_case->points[i].value, cJSON_GetObjectItem(point, "value")->valuestring);
    }

    cJSON_Delete(json);
    return test_case;
}


int main() {
    TestCase* test_case1 = read_test_case_from_json("testcase1.json");
    TestCase* test_case2 = read_test_case_from_json("testcase2.json");

    if (!test_case1 || !test_case2) {
        return 1; // Exit if there's an error reading the JSON files
    }

    // Solve and print results for both test cases
    solve_test_case(test_case1, 1);
    solve_test_case(test_case2, 2);

    // Clean up
    free(test_case1);
    free(test_case2);

    return 0;
}
