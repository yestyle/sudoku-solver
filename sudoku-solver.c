#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct cell {
	char row;
	char col;
	char box;
	char val;
	char sel[9];
	char sel_count;
};

struct sudoku {
	struct cell cell[9][9];
	char empty;
	char empty_row[9];
	char empty_col[9];
	char empty_box[9];
};

typedef struct trynode {
	struct sudoku  *sudoku;
	char row;
	char col;
	char val;
	struct trynode *next;
	struct trynode *prev;
} TRYNODE;

void print_sudoku(struct sudoku *sudoku)
{
	char value = 0;
	printf("    1 2 3   4 5 6   7 8 9\n");
	for (char i = 0; i < 9; i++) {
		if (i % 3 == 0) {
			printf("  -------------------------\n");
		}
		printf("%d ", i + 1);;
		for (char j = 0; j < 9; j++) {
			if (j % 3 == 0) {
				printf("| ");
			}
			value = sudoku->cell[i][j].val;
			if (value) {
				printf("%d ", value);
			} else {
				printf("  ");
			}
		}
		printf("|\n");
	}
	printf("  -------------------------\n");
	printf("Total empty cells: %d\n", sudoku->empty);
	printf("Empty cells of each row: ");
	for (char i = 0; i < 9; i++) {
		printf("%d ", sudoku->empty_row[i]);
	}
	printf("\n");
	printf("Empty cells of each col: ");
	for (char i = 0; i < 9; i++) {
		printf("%d ", sudoku->empty_col[i]);
	}
	printf("\n");
	printf("Empty cells of each box: ");
	for (char i = 0; i < 9; i++) {
		printf("%d ", sudoku->empty_box[i]);
	}
	printf("\n");
}

void print_selection(struct sudoku *sudoku)
{
	struct cell *pcell = NULL;
	for (char i = 0; i < 9; i++) {
		for (char j = 0; j < 9; j++) {
			pcell = &sudoku->cell[i][j];
			printf("[%d,%d]: ", i + 1, j + 1);
			for (char k = 0; k < 9; k++) {
				char sel = pcell->sel[k];
				if (sel) {
					printf("%d ", sel);
				} else {
					printf("  ");
				}
			}
			char count = pcell->sel_count;
			if (count == 1) {
				printf("[%d]\n", pcell->val);
			} else {
				printf(" %d\n", count);
			}
		}
	}
}

void update_cell_selection(struct sudoku *sudoku, char i, char j)
{
	struct cell *pcell = NULL;
	char value = sudoku->cell[i][j].val;
	if (value) {
		// row & col of cell in the top-left corner of this box
		char r = i / 3 * 3;
		char c = j / 3 * 3;
		for (char k = 0; k < 9; k++) {
			// all the cells on the same row
			pcell = &sudoku->cell[i][k];
			if ((k != j) && pcell->sel[value-1]) {
				pcell->sel[value-1] = 0;
				pcell->sel_count--;
			}
			// all the cells on the same column
			pcell = &sudoku->cell[k][j];
			if ((k != i) && pcell->sel[value-1]) {
				pcell->sel[value-1] = 0;
				pcell->sel_count--;
			}
			// all the cells in the same box
			pcell = &sudoku->cell[r+k/3][c+k%3];
			if ((pcell->row != i) && (pcell->col != j) && pcell->sel[value-1]) {
				pcell->sel[value-1] = 0;
				pcell->sel_count--;
			}
		}
	}
	return;
}

void update_all_selection(struct sudoku *sudoku)
{
	for (char i = 0; i < 9; i++) {
		for (char j = 0; j < 9; j++) {
			update_cell_selection(sudoku, i, j);
		}
	}

	return;
}

int check_selection(struct sudoku *sudoku)
{
	for (char i = 0; i < 9; i++) {
		for (char j = 0; j < 9; j++) {
			if (sudoku->cell[i][j].sel_count <= 0) {
				return 0;
			}
		}
	}
	return 1;
}

int check_sudoku(struct sudoku *sudoku)
{
	char target = (1 + 9) * 9 / 2;
	char sum = 0;
	// Check each row
	for (char i = 0; i < 9; i++) {
		sum = 0;
		for (char j = 0; j < 9; j++) {
			sum += sudoku->cell[i][j].val;
		}
		if (sum != target) {
			return 0;
		}
	}
	// Check each col
	for (char j = 0; j < 9; j++) {
		sum = 0;
		for (char i = 0; i < 9; i++) {
			sum += sudoku->cell[i][j].val;
		}
		if (sum != target) {
			return 0;
		}
	}
	// Check each box
	for (char i = 0; i < 9; i++) {
		sum = 0;
		char r = i / 3 * 3;
		char c = i % 3 * 3;
		for (char k = 0; k < 9; k++) {
			sum += sudoku->cell[r+k/3][c+k%3].val;
		}
		if (sum != target) {
			return 0;
		}
	}
	return 1;
}

void solution(char *puzzle)
{
	struct sudoku sudoku;
	char init[] = {1,2,3,4,5,6,7,8,9};
	struct cell *pcell = NULL;
	clock_t start, end;

	start = clock();

	// Init empty stats
	sudoku.empty = 9 * 9;
	memset(sudoku.empty_row, 9, sizeof(sudoku.empty_row));
	memset(sudoku.empty_col, 9, sizeof(sudoku.empty_col));
	memset(sudoku.empty_box, 9, sizeof(sudoku.empty_box));

	// Init every cell
	for (char i = 0; i < 9; i++) {
		for (char j = 0; j < 9; j++) {
			char value = puzzle[i * 9 + j];
			pcell = &sudoku.cell[i][j];
			pcell->val = value;
			pcell->row = i;
			pcell->col = j;
			pcell->box = i / 3 * 3 + j / 3;
			if (value) {
				memset(pcell->sel, 0, sizeof(pcell->sel));
				pcell->sel[value-1] = value;
				pcell->sel_count = 1;
				sudoku.empty--;
				sudoku.empty_row[i]--;
				sudoku.empty_col[j]--;
				sudoku.empty_box[pcell->box]--;
			} else {
				memcpy(pcell->sel, init, sizeof(pcell->sel));
				pcell->sel_count = 9;
			}
		}
	}

	printf("Initial puzzle:\n");
	print_sudoku(&sudoku);

	// Init selection
	update_all_selection(&sudoku);

	char prev_empty_loop = 0;
	char prev_empty_update = 0;
	do {
		prev_empty_loop = sudoku.empty;
		// Check if any single cell has only one selection
		do {
			prev_empty_update = sudoku.empty;
			for (char i = 0; i < 9; i++) {
				for (char j = 0; j < 9; j++) {
					pcell = &sudoku.cell[i][j];
					if (pcell->val) {
						continue;
					}
					if (pcell->sel_count == 1) {
						for (char k = 0; k < 9; k++) {
							pcell->val += pcell->sel[k];
						}
						sudoku.empty--;
						sudoku.empty_row[i]--;
						sudoku.empty_col[j]--;
						sudoku.empty_box[pcell->box]--;
						update_cell_selection(&sudoku, i, j);
					}
				}
			}
		} while (sudoku.empty < prev_empty_update);
		print_selection(&sudoku);
		print_sudoku(&sudoku);

		if (sudoku.empty == 0) {
			break;
		}

		// Check if any cell in a 3*3 box has exclusive selection that other cells don't have
		do {
			prev_empty_update = sudoku.empty;
			for (char i = 0; i < 9; i++) {
				char r = i / 3 * 3;
				char c = i % 3 * 3;
				for (char k = 0; k < 9; k++) {
					char count = 0;
					char value = 0;
					char m, n;
					for (char j = 0; j < 9; j++) {
						char sel = sudoku.cell[r + j/3][c + j%3].sel[k];
						if (sel) {
							count++;
							value = sel;
							m = r + j / 3;
							n = c + j % 3;
							if (count >= 2) {
								break;
							}
						}
					}
					pcell = &sudoku.cell[m][n];
					if ((count == 1) && (pcell->val == 0)) {
						memset(pcell->sel, 0, sizeof(pcell->sel));
						pcell->sel[value-1] = value;
						pcell->sel_count = 1;
						pcell->val = value;
						sudoku.empty--;
						sudoku.empty_row[m]--;
						sudoku.empty_col[n]--;
						sudoku.empty_box[pcell->box]--;
						update_cell_selection(&sudoku, m, n);
					}
				}
			}
		} while (sudoku.empty < prev_empty_update);
		print_selection(&sudoku);
		print_sudoku(&sudoku);

		if (sudoku.empty == 0) {
			break;
		}

		// Check if any cell in a one column has exclusive selection that other cells don't have
		do {
			prev_empty_update = sudoku.empty;
			for (char i = 0; i < 9; i++) {
				for (char k = 0; k < 9; k++) {
					char count = 0;
					char value = 0;
					char m, n;
					for (char j = 0; j < 9; j++) {
						char sel = sudoku.cell[j][i].sel[k];
						if (sel) {
							count++;
							value = sel;
							m = j;
							n = i;
							if (count >= 2) {
								break;
							}
						}
					}
					pcell = &sudoku.cell[m][n];
					if ((count == 1) && (pcell->val == 0)) {
						memset(pcell->sel, 0, sizeof(pcell->sel));
						pcell->sel[value-1] = value;
						pcell->sel_count = 1;
						pcell->val = value;
						sudoku.empty--;
						sudoku.empty_row[m]--;
						sudoku.empty_col[n]--;
						sudoku.empty_box[pcell->box]--;
						update_cell_selection(&sudoku, m, n);
					}
				}
			}
		} while (sudoku.empty < prev_empty_update);
		print_selection(&sudoku);
		print_sudoku(&sudoku);

		if (sudoku.empty == 0) {
			break;
		}

		// Check if any cell in a one row has exclusive selection that other cells don't have
		do {
			prev_empty_update = sudoku.empty;
			for (char i = 0; i < 9; i++) {
				for (char k = 0; k < 9; k++) {
					char count = 0;
					char value = 0;
					char m, n;
					for (char j = 0; j < 9; j++) {
						char sel = sudoku.cell[i][j].sel[k];
						if (sel) {
							count++;
							value = sel;
							m = i;
							n = j;
							if (count >= 2) {
								break;
							}
						}
					}
					pcell = &sudoku.cell[m][n];
					if ((count == 1) && (pcell->val == 0)) {
						memset(pcell->sel, 0, sizeof(pcell->sel));
						pcell->sel[value-1] = value;
						pcell->sel_count = 1;
						pcell->val = value;
						sudoku.empty--;
						sudoku.empty_row[m]--;
						sudoku.empty_col[n]--;
						sudoku.empty_box[pcell->box]--;
						update_cell_selection(&sudoku, m, n);
					}
				}
			}
		} while (sudoku.empty < prev_empty_update);
		print_selection(&sudoku);
		print_sudoku(&sudoku);

		if (sudoku.empty == 0) {
			break;
		}

		// Adjust some selections
		for (char i = 0; i < 9; i++) {
			if (sudoku.empty_box[i] == 2) {
				char count = 0;
				char r = i / 3 * 3;
				char c = i % 3 * 3;
				struct cell *pcell[2] = {NULL, NULL};
				for (char k = 0; k < 9; k++) {
					if (sudoku.cell[r+k/3][c+k%3].sel_count == 2) {
						count++;
						if (count == 1) {
							pcell[0] = &sudoku.cell[r+k/3][c+k%3];
						} else if (count == 2) {
							pcell[1] = &sudoku.cell[r+k/3][c+k%3];
							break;
						}
					}
				}
				if (pcell[0]->row == pcell[1]->row) {
					char row = pcell[0]->row;
					for (char j = 0; j < 9; j++) {
						if ((j == pcell[0]->col) || (j == pcell[1]->col)) {
							continue;
						}
						for (char k = 0; k < 9; k++) {
							if ((pcell[0]->sel[k]) && (sudoku.cell[row][j].sel[k] == pcell[0]->sel[k])) {
								sudoku.cell[row][j].sel[k] = 0;
								sudoku.cell[row][j].sel_count--;
							}
						}
					}
				} else if (pcell[0]->col == pcell[1]->col) {
					char col = pcell[0]->col;
					for (char j = 0; j < 9; j++) {
						if ((j == pcell[0]->row) || (j == pcell[1]->row)) {
							continue;
						}
						for (char k = 0; k < 9; k++) {
							if ((pcell[0]->sel[k]) && (sudoku.cell[j][col].sel[k] == pcell[0]->sel[k])) {
								sudoku.cell[j][col].sel[k] = 0;
								sudoku.cell[j][col].sel_count--;
							}
						}
					}
				}
			}
		}

	} while (sudoku.empty < prev_empty_loop);

	if (sudoku.empty) {
		printf("Try to brute force...\n");
		TRYNODE tryhead = {
			.sudoku = &sudoku,
			.next   = NULL,
			.prev   = NULL,
		};
		TRYNODE *ptry = &tryhead;

		TRYNODE *ptrynext = NULL;
		char pos = 0;
		while (pos < 9*9) {
			char i = pos / 9;
			char j = pos % 9;
			if (ptry->sudoku->cell[i][j].val) {
				pos++;
				continue;
			}
			printf("[i=%d,j=%d,pos=%d", i + 1, j + 1, pos);
			pcell = &ptry->sudoku->cell[i][j];
			char k;
			if (ptry->val) {
				k = ptry->val;
			} else {
				k = 0;
			}
			printf(",k=%d]\n", k);
			for (; k < 9; k++) {
				if (pcell->sel[k] == 0) {
					continue;
				}
				ptrynext = (TRYNODE*)calloc(sizeof(TRYNODE), 1);
				ptrynext->prev = ptry;
				ptry->next = ptrynext;

				ptry->row = pcell->row;
				ptry->col = pcell->col;
				ptry->val = pcell->sel[k];
				ptrynext->sudoku = (struct sudoku*)calloc(sizeof(*(ptry->sudoku)), 1);
				memcpy(ptrynext->sudoku, ptry->sudoku, sizeof(*(ptry->sudoku)));
				ptry = ptrynext;

				printf("Try [%d,%d]=%d(k=%d)\n", pcell->row + 1, pcell->col + 1, pcell->sel[k], k);
				ptry->sudoku->cell[pcell->row][pcell->col].val = pcell->sel[k];
				memset(ptry->sudoku->cell[pcell->row][pcell->col].sel, 0, sizeof(ptry->sudoku->cell[pcell->row][pcell->col].sel));
				ptry->sudoku->cell[pcell->row][pcell->col].sel[k] = k + 1;
				ptry->sudoku->cell[pcell->row][pcell->col].sel_count = 1;
				ptry->sudoku->empty--;
				ptry->sudoku->empty_row[pcell->row]--;
				ptry->sudoku->empty_col[pcell->col]--;
				ptry->sudoku->empty_box[pcell->box]--;
				update_cell_selection(ptry->sudoku, pcell->row, pcell->col);
				print_selection(ptry->sudoku);
				print_sudoku(ptry->sudoku);
				if (check_selection(ptry->sudoku)) {
					break;
				}
				ptrynext = ptry;
				ptry = ptry->prev;
				ptry->row = 0;
				ptry->col = 0;
				ptry->val = 0;
				ptry->next = NULL;
				free(ptrynext->sudoku);
				free(ptrynext);
			}

			if (k == 9) {
				ptrynext = ptry;
				ptry = ptry->prev;
				ptry->next = NULL;
				free(ptrynext->sudoku);
				free(ptrynext);

				// Try again from next pos
				pos = ptry->row * 9 + ptry->col;

				continue;
			}

			pos++;
		}

		memcpy(tryhead.sudoku, ptry->sudoku, sizeof(*(ptry->sudoku)));

		TRYNODE *pprev = NULL;
		while (ptry != &tryhead) {
			pprev = ptry->prev;
			free(ptry->sudoku);
			free(ptry);
			ptry = pprev;
		}
	}

	printf("Finished:\n");
	print_selection(&sudoku);
	print_sudoku(&sudoku);
	if (check_sudoku(&sudoku)) {
		printf("Correct!\n");
	} else {
		printf("Wrong!\n");
	}

	end = clock();
	printf("Clocks used: %ld\n", end - start);

	return;
}

int main(int argc, char* argv[])
{
	#if 0
	char puzzle1_hard[] = {
		/*     |        |        | */
		0, 0, 0, 3, 0, 1, 0, 0, 2,
		0, 0, 0, 0, 0, 4, 0, 7, 5,
		0, 2, 0, 0, 6, 0, 0, 0, 0,
		8, 0, 2, 0, 0, 9, 0, 0, 3,
		4, 9, 0, 0, 0, 0, 0, 1, 7,
		1, 0, 0, 4, 0, 0, 6, 0, 8,
		0, 0, 0, 0, 1, 0, 0, 4, 0,
		5, 8, 0, 6, 0, 0, 0, 0, 0,
		2, 0, 0, 9, 0, 5, 0, 0, 0
	};

	char puzzle2_medium[] = {
		/*     |        |        | */
		7, 4, 5, 8, 0, 0, 9, 0, 0,
		8, 0, 0, 0, 0, 4, 2, 0, 6,
		0, 0, 0, 1, 0, 0, 0, 8, 4,
		0, 2, 0, 0, 0, 1, 0, 0, 0,
		1, 0, 4, 0, 0, 0, 8, 0, 5,
		0, 0, 0, 6, 0, 0, 0, 3, 0,
		0, 9, 0, 0, 0, 8, 0, 0, 0,
		4, 0, 3, 2, 0, 0, 0, 0, 7,
		0, 0, 1, 0, 0, 9, 6, 4, 8
	};

	char puzzle3_easy[] = {
		/*     |        |        | */
		4, 0, 3, 0, 0, 0, 7, 1, 0,
		0, 1, 0, 5, 0, 0, 0, 0, 0,
		9, 0, 6, 0, 0, 3, 8, 0, 0,
		0, 6, 0, 0, 1, 5, 4, 0, 0,
		0, 7, 9, 0, 4, 0, 2, 3, 0,
		0, 0, 1, 9, 2, 0, 0, 8, 0,
		0, 0, 5, 4, 0, 0, 9, 0, 3,
		0, 0, 0, 0, 0, 2, 0, 7, 0,
		0, 3, 2, 0, 0, 0, 6, 0, 8
	};

	char puzzle4_medium[] = {
		/*     |        |        | */
		0, 3, 0, 0, 8, 5, 2, 0, 0,
		0, 0, 0, 0, 4, 0, 0, 0, 5,
		0, 9, 0, 2, 0, 0, 0, 4, 0,
		3, 0, 2, 0, 0, 0, 5, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 3,
		0, 0, 5, 0, 0, 0, 1, 0, 4,
		0, 8, 0, 0, 0, 7, 0, 6, 0,
		4, 0, 0, 0, 9, 0, 0, 0, 0,
		0, 0, 9, 3, 6, 0, 0, 8, 0
	};

	char puzzle5_hard[] = {
		/*     |        |        | */
		8, 0, 0, 0, 3, 0, 0, 7, 0,
		0, 0, 2, 0, 9, 1, 8, 0, 0,
		0, 0, 1, 0, 0, 0, 0, 3, 5,
		0, 0, 0, 6, 0, 0, 0, 0, 0,
		2, 0, 4, 0, 0, 0, 3, 0, 8,
		0, 0, 0, 0, 0, 5, 0, 0, 0,
		9, 2, 0, 0, 0, 0, 5, 0, 0,
		0, 0, 6, 1, 5, 0, 4, 0, 0,
		0, 4, 0, 0, 2, 0, 0, 0, 7
	};

	char puzzle6_easy[] = {
		/*     |        |        | */
		0, 3, 0, 0, 4, 0, 0, 7, 5,
		0, 0, 1, 0, 6, 7, 0, 2, 3,
		0, 6, 0, 0, 5, 3, 0, 0, 0,
		0, 0, 0, 4, 0, 0, 7, 3, 1,
		3, 0, 0, 0, 0, 0, 0, 0, 9,
		1, 7, 8, 0, 0, 6, 0, 0, 0,
		0, 0, 0, 7, 1, 0, 0, 4, 0,
		4, 5, 0, 2, 3, 0, 1, 0, 0,
		2, 1, 0, 0, 8, 0, 0, 9, 0
	};

	char puzzle7_medium[] = {
		/*     |        |        | */
		0, 0, 3, 4, 5, 0, 9, 2, 1,
		0, 0, 0, 0, 0, 0, 6, 0, 0,
		2, 4, 0, 9, 1, 6, 7, 0, 0,
		0, 5, 0, 0, 0, 7, 2, 1, 0,
		0, 3, 0, 0, 8, 0, 5, 4, 7,
		0, 0, 0, 0, 0, 0, 0, 8, 6,
		0, 8, 4, 0, 6, 0, 0, 0, 0,
		9, 6, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 3
	};

	char puzzle8_hard[] = {
		/*     |        |        | */
		3, 0, 0, 0, 0, 0, 0, 0, 7,
		0, 4, 0, 7, 6, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 6, 9, 0,
		6, 2, 0, 9, 0, 1, 0, 0, 0,
		0, 0, 7, 0, 0, 0, 0, 0, 8,
		0, 8, 0, 0, 0, 3, 9, 0, 4,
		0, 0, 0, 1, 5, 0, 4, 7, 0,
		0, 0, 0, 0, 0, 8, 0, 5, 0,
		0, 1, 0, 4, 0, 0, 0, 2, 0
	};

	char puzzle9_expert[] = {
		/*     |        |        | */
		5, 0, 0, 7, 0, 0, 0, 0, 0,
		0, 7, 0, 0, 0, 0, 0, 0, 8,
		0, 0, 0, 0, 4, 3, 1, 0, 0,
		4, 1, 0, 0, 5, 0, 7, 8, 3,
		3, 0, 6, 0, 0, 0, 0, 5, 1,
		0, 0, 0, 0, 0, 0, 2, 0, 0,
		0, 0, 0, 0, 2, 0, 4, 0, 7,
		0, 3, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 3, 8, 0, 0, 0
	};

	//solution(puzzle1_hard);
	//solution(puzzle2_medium);
	//solution(puzzle3_easy);
	//solution(puzzle4_medium);
	solution(puzzle5_hard);
	//solution(puzzle6_easy);
	//solution(puzzle7_medium);
	//solution(puzzle8_hard);
	//solution(puzzle9_expert);

	#else

	char puzzle[9*9] = {0};
	int input;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			scanf("%d", &input);
			puzzle[i * 9 + j] = (char)input;
		}
	}
	solution(puzzle);

	#endif

	return 0;
}
