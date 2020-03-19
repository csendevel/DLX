//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STR_SIZE 30
#define FIG_SIZE 20 //size of array for pentomino comparing
#define POL_SIZE 5 //polyomino size
#define INF 100000000
//#define TGEN

//C++ - C
//new - malloc(sizeof(type))
//delete - free()

int max_col_num, max_row_num;
bool used[60][60]; //array for chaking component
char board[60][60];
char tmp[60];
int comp_count;
int coordinates[60][2]; //board point number -> (x,y)
int figures[63][5][2]; //all figures
int numbers[60][60]; //(x,y) -> board point number

typedef struct TNode {
	struct TNode* left, * right, * up, * down;
	struct TColumn* Column;
} Node;

typedef struct TColumn {
	struct TColumn* left, * right;
	struct TNode* header;
	int length, name;
}Col;

Col** cols;
Node** ans;

//select column with minimum nodes
Col* select_column(Col* head) {
	int s = head->length;
	Col* ptr = head->right;
	Col* ans = NULL;
	while (ptr != head) {
		if (ptr->length < s) {
			s = ptr->length;
			ans = ptr;
		}
		ptr = ptr->right;
	}
	return ans;
}

void cover_column(Col* col) {
	col->right->left = col->left;
	col->left->right = col->right;

	Node* col_ptr = col->header->down;
	Node* row_ptr = col_ptr->right;

	while (col_ptr != col->header) {
		row_ptr = col_ptr->right;
		while (row_ptr != col_ptr) {
			row_ptr->down->up = row_ptr->up;
			row_ptr->up->down = row_ptr->down;
			row_ptr->Column->length--;
			row_ptr = row_ptr->right;
		}
		col_ptr = col_ptr->down;
	}
}

void uncover_column(Col* col) {
	Node* col_ptr = col->header->up;
	Node* row_ptr = col_ptr->left;

	while (col_ptr != col->header) {
		row_ptr = col_ptr->left;
		while (row_ptr != col_ptr) {
			row_ptr->Column->length++;
			row_ptr->down->up = row_ptr;
			row_ptr->up->down = row_ptr;
			row_ptr = row_ptr->left;
		}
		col_ptr = col_ptr->up;
	}

	col->left->right = col;
	col->right->left = col;
}

Col** create_columns(int number) {
	Col** columns = malloc(sizeof(Col) * (number + 1));

	columns[0] = malloc(sizeof(Col));
	columns[0]->header = NULL;
	columns[0]->length = INF;
	columns[0]->name = 0;
	for (int i = 1; i <= number; i++) {
		columns[i] = malloc(sizeof(Col));
		columns[i]->left = columns[i - 1];
		columns[i - 1]->right = columns[i];
		columns[i]->length = 0;
		columns[i]->name = i;
		columns[i]->header = malloc(sizeof(Node));
		columns[i]->header->up = columns[i]->header;
		columns[i]->header->down = columns[i]->header;
	}
	columns[0]->left = columns[number];
	columns[number]->right = columns[0];

	return columns;
}

void insert_in_column(Col* col, Node* node) {
	col->header->up->down = node;
	node->up = col->header->up;
	col->header->up = node;
	node->down = col->header;
	node->Column = col;
	col->length++;
}

void add_figure_to_DLX(int fig_size, int** figure, int fig_num) {
	Node** nodes = malloc(sizeof(Node) * (fig_size + 1));

	nodes[0] = malloc(sizeof(Node));
	insert_in_column(cols[fig_num], nodes[0]);
	for (int i = 1; i <= fig_size; i++) {
		nodes[i] = malloc(sizeof(Node));
		nodes[i]->left = nodes[i - 1];
		nodes[i - 1]->right = nodes[i];
		insert_in_column(cols[*figure[i - 1] + 13], nodes[i]);
	}

	nodes[0]->left = nodes[fig_size];
	nodes[fig_size]->right = nodes[0];
}

bool DLX(int depth) {
	if (cols[0]->right == cols[0]) {
		Node* ptr = NULL;

		int visual[40][40] = { 0 };

		for (int i = 0; i < depth; i++) {
			ptr = ans[i];
			do {
				if (ptr->Column->name > 12)
					visual[coordinates[ptr->Column->name - 13][0]][coordinates[ptr->Column->name - 13][1]] = i + 1;
				ptr = ptr->right;
			} while (ptr != ans[i]);
		}

		for (int i = 0; i < max_row_num; i++) {
			for (int j = 0; j < max_col_num; j++) {
				printf("%2d ", visual[i][j]);
			}
			printf("\n");
		}
		exit(0);
	}

	Col* smallest_column = select_column(cols[0]);

	cover_column(smallest_column);

	Node* col_ptr = smallest_column->header->down;
	Node* row_ptr = col_ptr->right;

	while (col_ptr != smallest_column->header) {

		ans[depth] = col_ptr;

		row_ptr = col_ptr->right;
		while (row_ptr != col_ptr) {
			cover_column(row_ptr->Column);
			row_ptr = row_ptr->right;
		}

		DLX(depth + 1);

		col_ptr = ans[depth];
		smallest_column = col_ptr->Column;

		row_ptr = col_ptr->left;
		while (row_ptr != col_ptr) {
			uncover_column(row_ptr->Column);
			row_ptr = row_ptr->left;
		}

		col_ptr = col_ptr->down;
	}

	uncover_column(smallest_column);
	return false;
}

void add_figure(int fig_num, int sq_num) {
	for (int i = 0; i < 5; i++) {
		used[coordinates[sq_num][0] + figures[fig_num][i][0]][coordinates[sq_num][1] + figures[fig_num][i][1]] = true;
	}
}

void load_figures() {
	FILE* load_file_stream;
	load_file_stream = fopen("figures.txt", "r");
	int c = 0;
	while (fscanf(load_file_stream, "%d", &figures[c / 10][(c / 2) % 5][c % 2]) == 1) c++;
	fclose(load_file_stream);
}

//depth first search
void dfs(int x, int y) {
	used[x][y] = true;
	comp_count++;
	if (x < 59)
		if (!used[x + 1][y])
			dfs(x + 1, y);
	if (y < 59)
		if (!used[x][y + 1])
			dfs(x, y + 1);
	if (x > 0)
		if (!used[x - 1][y])
			dfs(x - 1, y);
	if (y > 0)
		if (!used[x][y - 1])
			dfs(x, y - 1);
}

void used_update() {
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < 60; j++) {
			if (board[i][j] == '#')
				used[i][j] = false;
			else
				used[i][j] = true;
		}
	}
}

//checking possible positions of figures 
//for all component number of squares should be divided by 5 
bool component_check() {
	for (int i = 0; i < 60; i++) {
		if (!used[coordinates[i][0]][coordinates[i][1]]) {
			comp_count = 0;
			dfs(coordinates[i][0], coordinates[i][1]);
			if (comp_count % 5 != 0)
				return false;
		}
	}
	return true;
}

//cheking figures with rotations an inversions
bool is_unique(int fig_one, int fig_two) {
	int tmp[FIG_SIZE][FIG_SIZE] = { 0 };
	int x_new = 0, y_new = 0;
	int fig[5][2] = { 0 };	//second figure
	int r = 5;
	int y = 6;
	bool f = false;

	for (int i = 0; i < 5; i++) {
		fig[i][0] = figures[fig_two][i][0];
		fig[i][1] = figures[fig_two][i][1];
	}

	for (int i = 0; i < 5; i++)
		tmp[y + figures[fig_one][i][1]][r + figures[fig_one][i][0]] = 1;

	for (int t = 0; t < 2; t++) {
		//inversion y
		for (int k = 0; k < 5; k++) fig[k][1] = -fig[k][1];

		for (int rot = 0; rot < 4; rot++) {
			int temp = 0;
			//rotation on 90 degree
			for (int k = 0; k < 5; k++) {
				temp = fig[k][0];
				fig[k][0] = fig[k][1];
				fig[k][1] = -temp;
			}

			for (int i = 0; i < 5; i++) {
				f = true;
				x_new = r + figures[fig_one][i][0];
				y_new = y + figures[fig_one][i][1];
				//comparing
				for (int j = 0; j < 5; j++)
					if (tmp[y_new + fig[j][1]][x_new + fig[j][0]] == 0) f = false;

				if (f) return false;
			}
		}
	}
	return true;
}

void DLX_filling() {
	int x_new = 0, y_new = 0;
	bool f = true;
	int fig_num = 1;
	int** figure = malloc(sizeof(int) * POL_SIZE);

	for (int i = 0; i < 63; i++) {
		if (i > 0)
			if (is_unique(i, i - 1))
				fig_num++;

		for (int j = 0; j < 60; j++) {
			f = true;
			for (int k = 0; k < POL_SIZE; k++) {
				x_new = coordinates[j][0] + figures[i][k][0];
				y_new = coordinates[j][1] + figures[i][k][1];

				if (used[x_new][y_new] || x_new >= 60 || x_new < 0 || y_new >= 60 || y_new < 0)
					f = false;
			}
			if (f) {
				add_figure(i, j);
				if (component_check()) {
					for (int r = 0; r < POL_SIZE; r++) {
						figure[r] = malloc(sizeof(int));
						x_new = coordinates[j][0] + figures[i][r][0];
						y_new = coordinates[j][1] + figures[i][r][1];
						figure[r] = &numbers[x_new][y_new];
					}
					add_figure_to_DLX(POL_SIZE, figure, fig_num);
				}
				used_update();
			}
		}
	}
}

void figures_check() {
	int tmp[FIG_SIZE][FIG_SIZE] = { 0 };

	int r = 5;
	int y = 6;

	for (int k = 0; k < 63; k++) {
		memset(tmp, 0, sizeof(tmp));

		for (int i = 0; i < 5; i++)
			tmp[y + figures[k][i][1]][r + figures[k][i][0]] = 1;

		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				printf("%d", tmp[i][j]);
			}
			printf("\n");
		}
		if (k > 0)
			printf("%d\n", is_unique(k, k - 1));

		_getchar_nolock();
		for (int g = 0; g < 20; g++) printf("\n");
	}
}

int main() {
	time_t t;
	srand((unsigned)time(&t));

	FILE* file_stream;
#ifdef TGEN 
	file_stream = fopen("test.txt", "w");

	//test generation
	int numofpoints = 0;
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < 60; j++) {
			if (rand() % 2 == 0 && numofpoints <= 60) {
				tmp[j] = '#';
				numofpoints++;
			}
			else
				tmp[j] = '.';
		}
		fprintf(file_stream, "%s\n", tmp);
		memset(tmp, '.', sizeof(tmp));
	}

	freopen("test.txt", "r", file_stream);
#else
	file_stream = fopen("test2.txt", "r");
#endif // TGEN

	int c = 0;

	char sym;
	int ptr = 0;
	while ((sym = fgetc(file_stream)) != EOF) {
		if (sym == '\n') {
			c++;
			if (ptr > max_col_num) max_col_num = ptr;
			ptr = 0;
			continue;
		}
		board[c][ptr] = sym; ptr++;
	}
	fclose(file_stream);
	max_row_num = c + 1;

	c = 0;
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < 60; j++) {
			if (board[i][j] == '#') {
				coordinates[c][0] = i;
				coordinates[c][1] = j;
				numbers[i][j] = c; c++;
				used[i][j] = false;
			}
			else
				used[i][j] = true;
		}
	}

	load_figures();

	//figures_check();

	cols = create_columns(72);
	DLX_filling();

	ans = malloc(sizeof(Node) * 12);
	for (int i = 0; i < 12; i++)
		ans[i] = malloc(sizeof(Node));

	DLX(0);
	printf("There are no soultions\n");
}
