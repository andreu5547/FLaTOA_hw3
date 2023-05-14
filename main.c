#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define OUTPUT_FILE "graph.dot" // Файл выходных данных для GraphViz'a
#define COEFF_UP_MEMORY 2 // Коэффициент увеличения выделенной памяти
#define BASE_CAPACITY 10

#define swap_Edge(a, b) Edge temp = a; \
                        a = b; \
                        b = temp;

typedef struct Edge { // Структура данных для ребра
    uint16_t src;
    uint16_t dest;
    uint16_t weight;
} Edge;

typedef struct Graph { // Структура данных графа
    uint16_t num_vertices; // Число вершин
    uint16_t num_edges; // Число ребер
    uint16_t capacity; // Вместимость
    bool directed; // Ориентированность
    bool weighted; // Взвешенность
    Edge *edges; // Ребра
} Graph;

Graph *create_graph(uint16_t capacity) { // Функция создания графа
    Graph *graph = (Graph *) malloc(sizeof(Graph)); // Выделяем память под граф
    if (!graph) { // Проверяем выделилась ли память
        puts("Allocation error!");
        exit(1);
    }
    // Задаем базовые значения и выделяем память для ребер + проверка выделилась ли
    graph->capacity = capacity;
    graph->num_vertices = 0;
    graph->num_edges = 0;
    graph->directed = false;
    graph->weighted = false;
    graph->edges = (Edge *) malloc(capacity * sizeof(Edge)); // Выделяем память под ребра
    if (!graph->edges) { // Проверяем выделилась ли память
        puts("Allocation error!");
        exit(1);
    }
    return graph;
}

void add_memory_graph(Graph *graph) { // Функция расширения вместимости графа
    if (graph->capacity < 10000) // Выбираем коэффициент в зависимости от вместимости графа
        graph->capacity = graph->capacity * COEFF_UP_MEMORY;
    else graph->capacity = (uint16_t) graph->capacity * COEFF_UP_MEMORY / 2; // Устанавливаем новое значение вместимости
    graph->edges = (Edge *) realloc(graph->edges, graph->capacity * sizeof(Edge)); // Выделяем новое количество памяти
    if (!graph->edges) { // Проверяем выделилась ли память
        puts("Allocation error!");
        exit(1);
    }
}

void add_edge(Graph *graph, uint16_t *edge) { // Функция для добавления ребер в граф
    graph->edges[graph->num_edges].src = edge[0];
    graph->edges[graph->num_edges].dest = edge[1];
    graph->edges[graph->num_edges].weight = edge[2];
}

Graph *read_graph_from_file(char *filename, bool directed, bool weighted) { // Функция считывания графа из файла в формате "исходная_вершина конечная_вершина"
    FILE *fp = fopen(filename, "r"); // Открываем файл
    if (fp == NULL) { // Проверяем открылся ли файл
        printf("Error opening file %s!", filename);
        return NULL;
    }

    Graph *graph = create_graph(BASE_CAPACITY); // Создание графа
    // создание буферов: для считываемого символа, числа, сохранённого первого числа и флага для проверки корректности колва чисел в строке
    char buff_c = '0';
    uint8_t i = 0;
    uint16_t buff_n = 0, buff_src[3] = {0, 0, 0};

    while (buff_c != EOF) { // Читаем посимвольно файл
        buff_c = getc(fp);
        if (buff_c != '\n' && buff_c != EOF) { // Если не символ не \n или EOF входим в if
            if ('0' <= buff_c && buff_c <= '9') { // Проверяем является ли символ цифрой
                buff_n = buff_n * 10 + buff_c - '0';
            } else if (buff_c == ' ') { // Если символ пробел -> ввод цифры окончен
                if ((i > 2 && graph->weighted) || (i > 1 && !graph->weighted)) { // Проверяем максимальное кол-во символов, введённых до, в зависимости от типа графа
                    puts("Error input file data!");
                    return NULL;
                }
                if (graph->num_vertices < buff_n) // поиск максимального номера вершины
                    graph->num_vertices = buff_n;
                buff_src[i++] = buff_n; // Записываем число
                buff_n = 0; // Очищаем буфер
            } else { // Если есть посторонние символы - ошибка
                puts("Error input file data!");
                return NULL;
            }
        } else {
            buff_src[i++] = buff_n; // Записываем число
            if (graph->capacity == graph->num_edges) // Выделение дополнительной памяти, при её нехватке
                add_memory_graph(graph);
            if (graph->num_vertices < buff_n) // поиск максимального номера вершины
                graph->num_vertices = buff_n;
            if ((i == 3 && graph->weighted) || (i == 2 && !graph->weighted)) { // Проверяем, что введены все необходимые числа
                add_edge(graph, buff_src); // Добавляем ребро
                graph->num_edges++; // Увеличиваем количество ребер на 1
            }
            // Обнуляем временные переменные
            buff_n = 0;
            i = 0;
        }
        graph->directed = directed;
        graph->weighted = weighted;
    }

    fclose(fp); // Закрываем файл
    return graph; // Возвращаем граф
}

void print_graph_info(Graph *graph) { // Функция вывода информации о графе
    printf("Number of vertices: %d\nNumber of edges: %d\nDirected: %d\nWeighted: %d\n", graph->num_vertices + 1, graph->num_edges, graph->directed, graph->weighted);
}

void write_graph_to_dot_file(Graph *graph, char *filename) { // Функция записи графа в файл, для GraphViz'a
    FILE *fp = fopen(filename, "w"); // Открываем файл для записи
    if (fp == NULL) { // Проверяем корректно ли открылся файл
        printf("Error opening file\n");
        return;
    }

    if (graph->directed) { // Выбор режима записи (ориентированный / неориентированный граф) и его запись
        fputs("digraph G {\n", fp);
        if (graph->weighted) // Взвешенный или нет
            for (uint16_t i = 0; i < graph->num_edges; i++)
                fprintf(fp, "\t%d -> %d [label=\"%d\"];\n", graph->edges[i].src, graph->edges[i].dest, graph->edges[i].weight);
        else for (uint16_t i = 0; i < graph->num_edges; i++)
                fprintf(fp, "\t%d -> %d;\n", graph->edges[i].src, graph->edges[i].dest);
    } else {
        fputs("graph {\n", fp);
        if (graph->weighted) // Взвешенный или нет
            for (uint16_t i = 0; i < graph->num_edges; i++)
                fprintf(fp, "\t%d -- %d [label=\"%d\"];\n", graph->edges[i].src, graph->edges[i].dest, graph->edges[i].weight);
        else for (uint16_t i = 0; i < graph->num_edges; i++)
                fprintf(fp, "\t%d -- %d;\n", graph->edges[i].src, graph->edges[i].dest);
    }
    fputs("}", fp);

    fclose(fp); // Закрываем файл

    char command[100]; // Создание и отправка команды для вызова GraphViz'a
    sprintf(command, "dot -Tpng %s -o %s.png", filename, filename);
    if (system(command)) { // Проверяем выделилась ли память
        puts("Error use GraphViz!\nCheck your configuration of GraphViz.");
    }
}

void free_graph(Graph *graph) { // Функция очистки памяти, выделенной для графа
    free(graph->edges);
    free(graph);
}

Graph * sort_edge(Graph *graph) { // Функция сортировки графа
    // сортировка ребер пузырьком по второму элементу
    for (uint16_t i = 0; i < graph->num_edges - 1; i++)
        for (uint16_t j = 0; j < graph->num_edges - i - 1; j++)
            if (graph->edges[j].dest > graph->edges[j + 1].dest) {
                swap_Edge(graph->edges[j], graph->edges[j + 1])
            }
    // сортировка ребер пузырьком по первому элементу
    for (uint16_t i = 0; i < graph->num_edges - 1; i++)
        for (uint16_t j = 0; j < graph->num_edges - i - 1; j++)
            if (graph->edges[j].src > graph->edges[j + 1].src) {
                swap_Edge(graph->edges[j], graph->edges[j + 1])
            }

    return graph; // Возвращаем отсортированный граф
}

Graph * delete_sorting_repeating_edge(Graph *graph) { // Функция удаления повторяющихся ребер из графа
    for (uint16_t i = 0; i < graph->num_edges; ++i) { // перебираем все элементы множества
        bool fl = false;
        for (uint16_t j = i + 1; j < graph->num_edges; ++j) // ищем дубликат текущего элемента
            if (graph->edges[i].src == graph->edges[j].src && graph->edges[i].dest == graph->edges[j].dest && graph->edges[i].weight == graph->edges[j].weight)
                fl = true;
        if (fl) { //если элемент нашёлся, то удаляем его и сдвигаем массив на один элемент влево и уменьшаем размер на 1
            for (uint16_t j = i; j < graph->num_edges; ++j)
                graph->edges[j] = graph->edges[j + 1 ];
            i--;
            graph->num_edges--;
        }
    }
    return graph;
}

bool compare_string(const char *s1, const char *s2) { // Функция сравнения строк
    for (int i = 0; s1[i] != '\0' && s2[i] != '\0'; ++i) {
        if (s1[i] != s2[i]) return false;
        if (s1[i] == '\0' && s2[i] != '\0') return false;
        if (s2[i] == '\0' && s1[i] != '\0') return false;
    }
    return true;
}

void print_help() { // Вывод помощи
    puts("Usage: FLaTOA_hw2 <input filename> <directed> <weighted> <del_repeat>\n");
    puts("Arguments:");
    puts("<input filename>: Name of the file with the graph // string");
    puts("<directed>: Directed or undirected graph // true or false (1, 0)");
    puts("<weighted>: Weighted or unweighted graph // true or false (1, 0)");
    puts("<del_repeat>: Delete duplicate edges in a graph // true or false (1, 0)");
    puts("\nExample: FLaTOA_hw2 graph.txt true false true");
    puts("\nTo see this message again usage: FLaTOA_hw2 -help");
}

bool check_arg(int argc, char *argv[], bool *directed, bool *weighted, bool *del_repeat) { // Функция обработки аргументов
    if (compare_string(argv[1], "h") || compare_string(argv[1], "-h") || compare_string(argv[1], "help") || compare_string(argv[1], "-help")) {
        print_help();
        return false;
    }

    if (argc < 5) { // Проверяем, что переданы все необходимые аргументы
        printf("Error: not enough arguments provided\n");
        print_help();
        return false;
    }

    if (compare_string(argv[2], "1") || compare_string(argv[2], "true")) { // Проверяем bool'евы ли значения переданы
        *directed = true;
    } else if (compare_string(argv[2], "0") || compare_string(argv[2], "false")) {
        *directed = false;
    } else {
        printf("Error: check directed value\n");
        print_help();
        return false;
    }

    if (compare_string(argv[3], "1") || compare_string(argv[3], "true")) { // Проверяем bool'евы ли значения переданы
        *weighted = true;
    } else if (compare_string(argv[3], "0") || compare_string(argv[3], "false")) {
        *weighted = false;
    } else {
        printf("Error: check weighted value\n");
        print_help();
        return false;
    }

    if (compare_string(argv[4], "1") || compare_string(argv[4], "true")) { // Проверяем bool'евы ли значения переданы
        *del_repeat = true;
    } else if (compare_string(argv[4], "0") || compare_string(argv[4], "false")) {
        *del_repeat = false;
    } else {
        printf("Error: check del_repeat value\n");
        print_help();
        return false;
    }

    return true;
}

void DFS(int vertex, Edge *edges, bool* visited, int vertices) {
    visited[vertex] = true;
    for (int i = 0; i < vertices; ++i) {
        if (edges[i].src == vertex && !visited[edges[i].dest]) {
            DFS(edges[i].dest, edges, visited, vertices);
        }
    }
}

int isGraphConnected(Graph *graph) {
    bool* visited = (bool*)calloc(graph->num_vertices, sizeof(bool));;

    DFS(0, graph->edges, visited, graph->num_edges);
    for (int i = 0; i < graph->num_vertices; ++i) {
        if (!visited[i]) {
            free(visited);
            return 0;
        }
    }
    free(visited);
    return 1;
}

int main(int argc, char *argv[]) {
    bool directed, weighted, del_repeat;

    if (!check_arg(argc, argv, &directed, &weighted, &del_repeat)) return 0;

    Graph *graph;
    graph = read_graph_from_file(argv[1], directed, weighted);

    if (graph != NULL) {
        puts("Input graph info:");
        print_graph_info(graph);
        if (del_repeat) {
            sort_edge(graph);
            delete_sorting_repeating_edge(graph);
            puts("After removing duplicate edges graph info:");
            print_graph_info(graph);
        }

        write_graph_to_dot_file(graph, OUTPUT_FILE);

        printf("Graph is %s!", isGraphConnected(graph) ? "connected" : "not connected");

        free_graph(graph);
    }
    return 0;
}