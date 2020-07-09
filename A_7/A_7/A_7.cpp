#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <list>
#include <locale>
#include <vector>

using namespace std;

int N; // кол-во вершин
int M; // кол-во ребер
const int INF = 999; // бесконечность
int source, outlet; // источник и сток

int excess[INF]; //избыточное вещество(еденицы) / поток
int hight[INF]; //высота

/*
u -пропускная способность ребра
c - стоимость ребра
f - текущий поток
*/
struct rib {
	int b, u, c, f;
	size_t back;
};

/*операция проталкивания */
/*выполняется, когда выполнены условия
1) u - переполнена
2) ребро u->v приналжеит остаточной сети
3) вершина u выше v, тоесть h[u] = h[v] + 1*/
void push(int u, int v, int **graph)
{
	int f = min(excess[u], graph[u][v]); // число вещества для проталкивания, минимальное из избытка в u и сколько можно пустить ещё
	excess[u] -= f; excess[v] += f; // добавим поток в вершину v и отнимем его из вершины u
	graph[u][v] -= f; // уменьшаем излишек в вершине u
	graph[v][u] += f; // увеличиваем излишек в вершине v
}

/*операция поднятия */
/*выполняется, когда выполняются
1) u - переполнена
2) нельзя сделать push ни в какую вершину*/
void lift(int u, int **graph)
{
	int min = 3 * N + 1;  // задание начального минимального вершины

						  //находим соседа с минимальной высотой
	for (int i = 0; i < N; i++)
		if (graph[u][i] && (hight[i] < min))
			min = hight[i];
	hight[u] = min + 1; // присваиваем новое значение высоты
}

/*обслуживание вершины(снятие излишка)*/
void discharge(int u, int **graph)
{
	int v = 0;
	while (excess[u] > 0) // пока есть излишек либо операция push, либо lift
	{
		if (graph[u][v] && hight[u] == hight[v] + 1) //ребро есть в остаточной сети и его высота на 1 меньше
		{
			push(u, v, graph);
			v = 0;
			continue;
		}
		v++;
		if (v == N)  // выполнили операцию push для всех соседей, то lift
		{
			lift(u, graph);
			v = 0;
		}
	}
}

/*инициализация транспортной сети*/
void init(int **graph)
{
	for (int i = 0; i < N; i++)
	{
		if (i == source) continue; // пропускаем исток
		excess[i] = graph[source][i]; //поток
		graph[i][source] += graph[source][i];
	}
	hight[source] = N; // высота истока равна количеству вершин
}


/*ввод графа из файла, заданного при помощи матрицы пропускной способности и матрицы стомостей*/
void input_matrix(ifstream &in, int **graph, int** cost)
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			in >> graph[i][j];

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			in >> cost[i][j];
}

/* печать графа c весом рёбер(вывод в graphic.png) */
void print_graph(int **graph, int **cost) {
	ofstream out;
	out.open("graphic.dot");
	out << "digraph G{" << "\n";
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (graph[i][j] != 0 && i < j)
				out << i + 1 << " ->" << j + 1 << "[label = \" " << graph[i][j] << " | " << cost[i][j] << "\"];\n";
	out << "}";
	out.close();
	system("dot -Tpng graphic.dot -o graphic.png ");
	system("graphic.png");
}

/*копирование матрицы*/
void copy_matrix(int** graph, int** copy) {
	for (int i = 0; i < N; i++) {
		copy[i] = new int[N];
		for (int j = 0; j < N; j++) 
			copy[i][j] = graph[i][j];
	}
}

/*Посчет количества рёбер*/
int count_rib(int **graph) {
	int count = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (graph[i][j] != 0 && i < j)
				count++;
	return count;
}

/*перевод из массива в список ребер*/
void matrix_to_vector_rib(int** graph, int** cost, vector<vector<rib>>& graph_vector) {
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++)
			if (graph[i][j] > 0) {
				graph_vector[i].push_back({ j, graph[i][j] , cost[i][j], 0, graph_vector[j].size() });
				graph_vector[j].push_back({ i, 0, (-1)*cost[i][j], 0, graph_vector[i].size() });
			}
}

/*------------------------------------------------------------------------------------------------------------------------------*/

/*Главная функия*/
int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "rus");
	ifstream ifs; ofstream ofs;

	int ans;
	do {
		cout << "Редактировать файл ввода? (1-да, 0 -нет)." << endl;
		cin >> ans;
		if (ans == 1) system("input.txt");
	} while (ans != 1 && ans != 0);

	ifs.open("input.txt");
	ifs >> N >> source >> outlet; //вывод из файла количества вершин, истока и стока
	source -= 1; outlet -= 1;

	int **transport_network = new int*[N];
	int **transport_network_cost = new int*[N];
	int **transport_network_copy = new int*[N];

	for (int i = 0; i < N; i++) {
		transport_network[i] = new int[N];
		transport_network_cost[i] = new int[N];
		for (int j = 0; j < N; j++) {
			transport_network[i][j] = 0;
			transport_network_cost[i][j] = 0;
		}
	}
	input_matrix(ifs, transport_network, transport_network_cost);
	copy_matrix(transport_network, transport_network_copy);


	do {
		cout << "Вывести граф? (1-да, 0 -нет)." << endl;
		cin >> ans;
		if (ans == 1) print_graph(transport_network, transport_network_cost);
	} while (ans != 1 && ans != 0);

	ifs.close();

	list<int> l; // список потенциальных соседей
	list<int>::iterator cur;
	int old;

	init(transport_network_copy); // инициализация

	/*Алгоритм проталкивания в начало(для нахождения границы потока)*/
	 //заносим вершины не исток и сток в список
	for (int i = 0; i < N; i++)
		if (i != source && i != outlet)
			l.push_front(i);
	cur = l.begin();  //задаём начальную вершину

	while (cur != l.end())
	{
		old = hight[*cur]; //сохраняем высоту вершины перед осблуживанием
		discharge(*cur, transport_network_copy); // обслуживаем вершину
		if (hight[*cur] != old) { // если высота была подята( высота увеличилась)
			l.push_front(*cur);
			l.erase(cur);
			cur = l.begin(); // отправляем эту вершину в начало списка
		}
		cur++;
	}

	int MaxFlow = excess[outlet]; // максимальный поток

	l.clear(); // очистка списка

	/*матрица смежности в заданный вектор*/
	M = count_rib(transport_network_cost);

	vector<vector<rib>> transport_network_vector(N);
	matrix_to_vector_rib(transport_network, transport_network_cost, transport_network_vector);

	/*алгоритм Бусакера-Гоуэна(алгоритм увеличивающихся путей)*/
	int our_flow;
	int flow = 0, cost = 0;

	while (1) {
		cout << "Введите объём потока: "; cin >> our_flow;
		if (our_flow <= MaxFlow) {
			while (flow < our_flow) { // повторяем алгоритм пока мощность потока не превзойдет заданный объём
				vector<int> id(N, 0);
				vector<int> distance(N, INF);
				vector<int> queue(N);
				vector<int> path(N);
				vector<size_t> path_rib(N);
				int qh = 0, qt = 0;

				/*поиск пути с минимальной удельной стоимостью (на осонове алгоритма Левита)*/
				queue[qt++] = source;
				distance[source] = 0;

				while (qh != qt) {
					int edge = queue[qh++];
					id[edge] = 2;

					if (qh == N) qh = 0;

					for (size_t i = 0; i < transport_network_vector[edge].size(); ++i) {
						rib & r = transport_network_vector[edge][i];

						if (r.f < r.u && distance[edge] + r.c < distance[r.b]) {
							distance[r.b] = distance[edge] + r.c;

							if (id[r.b] == 0) {
								queue[qt++] = r.b;

								if (qt == N) qt = 0;
							}
							else
								if (id[r.b] == 2) {
									if (--qh == -1)
										qh = N - 1;
									queue[qh] = r.b;
								}

							id[r.b] = 1;
							path[r.b] = edge;
							path_rib[r.b] = i;
						}
					}
				}

				if (distance[outlet] == INF)  break;
				int add_flow = our_flow - flow; //считаем мощность потока 

				for (int v = outlet; v != source; v = path[v]) { //проходимся по вектору кратчайшего пути с конца до начала 
					 /* вычисление r = min{r(e): где е - вершина, пути с удел.стоим.}
					r(e) на прямых дугах r(e)= c(e) - f(e)(пропускная способность минус пущенный поток), а на обратных r(e) = f(e)*/
					int pv = path[v];  
					size_t pr = path_rib[v];
					add_flow = min(add_flow, transport_network_vector[pv][pr].u - transport_network_vector[pv][pr].f);
				}

				for (int v = outlet; v != source; v = path[v]) { // проходимся по вектору кратчайшего пути с конца до начала
					int pv = path[v];
					size_t pr = path_rib[v];
					size_t r = transport_network_vector[pv][pr].back;
					transport_network_vector[pv][pr].f += add_flow; // на прямых дугах увеличиваем поток на r
					transport_network_vector[v][r].f -= add_flow; // на обратных дугах уменьшаем поток на r
					cost += transport_network_vector[pv][pr].c * add_flow; //увеличиваем мощность потока
				}
				flow += add_flow; //увеличиваем пропускной поток на addflow
			}

			ofs.open("output.txt");
			ofs << "Стоимость заданого потока " << flow << " равна: " << cost << ".";
			ofs.close();
			break;
		}
		else {
			cout << "Заданный поток больше максимального! Поторите ввод!" << endl;
			continue;
		}
	}

	/*открытие результата*/
	do {
		cout << "Вывести результат? (1-да, вывести, 0 - не выводить)." << endl;
		cin >> ans;
		if (ans == 1) system("output.txt");

	} while (ans != 1 && ans != 0);

	return 0;
}