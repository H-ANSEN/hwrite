#include <criterion/criterion.h>
#include "../mat.h"

static Mat *matrix = NULL;

static void init_empty_mat(void) {
    matrix = mat_create(4, 4);
    if (!matrix) cr_assert_fail("Failed to allocate memory for matrix");
}

static void init_seq_mat(void) {
    matrix = mat_create(4, 4);
    if (!matrix) cr_assert_fail("Failed to allocate memory for matrix");
    MAT_ELEM(matrix, 0, 0) = 0;  MAT_ELEM(matrix, 0, 1) = 1;
    MAT_ELEM(matrix, 0, 2) = 2;  MAT_ELEM(matrix, 0, 3) = 3;
    MAT_ELEM(matrix, 1, 0) = 4;  MAT_ELEM(matrix, 1, 1) = 5;
    MAT_ELEM(matrix, 1, 2) = 6;  MAT_ELEM(matrix, 1, 3) = 7;
    MAT_ELEM(matrix, 2, 0) = 8;  MAT_ELEM(matrix, 2, 1) = 9;
    MAT_ELEM(matrix, 2, 2) = 10; MAT_ELEM(matrix, 2, 3) = 11;
    MAT_ELEM(matrix, 3, 0) = 12; MAT_ELEM(matrix, 3, 1) = 13;
    MAT_ELEM(matrix, 3, 2) = 14; MAT_ELEM(matrix, 3, 3) = 15;
}

static void init_mat(void) {
    matrix = mat_create(3, 3);
    MAT_ELEM(matrix, 0, 0) = 1; MAT_ELEM(matrix, 0, 1) = 2; MAT_ELEM(matrix, 0, 2) = 3;
    MAT_ELEM(matrix, 1, 0) = 2; MAT_ELEM(matrix, 1, 1) = 4; MAT_ELEM(matrix, 1, 2) = 6;
    MAT_ELEM(matrix, 2, 0) = 3; MAT_ELEM(matrix, 2, 1) = 6; MAT_ELEM(matrix, 2, 2) = 8;
}

static void clean(void) {
    mat_free(matrix);
}

Test(mat_test, zeroed_mat_on_init, .init=init_empty_mat, .fini=clean) {
    for (int i = 0; i < matrix->cols; i++) {
        for (int j = 0; j < matrix->rows; j++) {
            float element = MAT_ELEM(matrix, i, j);
            cr_assert_eq(element, 0);
        }
    }
}

Test(mat_test, mean_mat_calculation, .init=init_seq_mat, .fini=clean) {
    float mean_results[matrix->cols];
    mat_mean(matrix, mean_results);
    cr_assert_eq(mean_results[0], 6.0f);
    cr_assert_eq(mean_results[1], 7.0f);
    cr_assert_eq(mean_results[2], 8.0f);
    cr_assert_eq(mean_results[3], 9.0f);
}

Test(mat_test, centered_mat_calculation, .init=init_seq_mat, .fini=clean) {
    mat_center(matrix);
    cr_assert_eq(MAT_ELEM(matrix, 0, 0), -6.0f);
    cr_assert_eq(MAT_ELEM(matrix, 0, 1), -6.0f);
    cr_assert_eq(MAT_ELEM(matrix, 0, 2), -6.0f);
    cr_assert_eq(MAT_ELEM(matrix, 0, 3), -6.0f);
    cr_assert_eq(MAT_ELEM(matrix, 1, 0), -2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 1, 1), -2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 1, 2), -2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 1, 3), -2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 2, 0), 2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 2, 1), 2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 2, 2), 2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 2, 3), 2.0f);
    cr_assert_eq(MAT_ELEM(matrix, 3, 0), 6.0f);
    cr_assert_eq(MAT_ELEM(matrix, 3, 1), 6.0f);
    cr_assert_eq(MAT_ELEM(matrix, 3, 2), 6.0f);
    cr_assert_eq(MAT_ELEM(matrix, 3, 3), 6.0f);
}

Test(mat_test, covariance_seq_mat_calculation, .init=init_seq_mat, .fini=clean) {
    mat_center(matrix); // covariance must be on a centered matrix
    Mat *cov = mat_cov(matrix);
                        
    // all entries should be the same value since the original matrix 'seq' is
    // linearly dependent
    for (size_t i = 0; i < cov->cols * cov->rows; i++) {
        cr_assert_float_eq(cov->data[i], 26.66, 0.01);
    }

    mat_free(cov);
}

Test(mat_test, covariance_mat_calculation, .init=init_mat, .fini=clean) {
    mat_center(matrix);
    Mat *cov = mat_cov(matrix);

    cr_assert_eq(MAT_ELEM(cov, 0, 0), 1.0f); cr_assert_eq(MAT_ELEM(cov, 0, 1), 2.0f); cr_assert_eq(MAT_ELEM(cov, 0, 2), 2.5f);
    cr_assert_eq(MAT_ELEM(cov, 1, 0), 2.0f); cr_assert_eq(MAT_ELEM(cov, 1, 1), 4.0f); cr_assert_eq(MAT_ELEM(cov, 1, 2), 5.0f);
    cr_assert_eq(MAT_ELEM(cov, 2, 0), 2.5f); cr_assert_eq(MAT_ELEM(cov, 2, 1), 5.0f); cr_assert_float_eq(MAT_ELEM(cov, 2, 2), 6.33f, 0.01);

    mat_free(cov);
}
