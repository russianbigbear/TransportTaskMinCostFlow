#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <list>
#include <locale>
#include <vector>

using namespace std;

int N; // ���-�� ������
int M; // ���-�� �����
const int INF = 999; // �������������
int source, outlet; // �������� � ����

int excess[INF]; //���������� ��������(�������) / �����
int hight[INF]; //������

/*
u -���������� ����������� �����
c - ��������� �����
f - ������� �����
*/
struct rib {
	int b, u, c, f;
	size_t back;
};

/*�������� ������������� */
/*�����������, ����� ��������� �������
1) u - �����������
2) ����� u->v ���������� ���������� ����
3) ������� u ���� v, ������ h[u] = h[v] + 1*/
void push(int u, int v, int **graph)
{
	int f = min(excess[u], graph[u][v]); // ����� �������� ��� �������������, ����������� �� ������� � u � ������� ����� ������� ���
	excess[u] -= f; excess[v] += f; // ������� ����� � ������� v � ������� ��� �� ������� u
	graph[u][v] -= f; // ��������� ������� � ������� u
	graph[v][u] += f; // ����������� ������� � ������� v
}

/*�������� �������� */
/*�����������, ����� �����������
1) u - �����������
2) ������ ������� push �� � ����� �������*/
void lift(int u, int **graph)
{
	int min = 3 * N + 1;  // ������� ���������� ������������ �������

						  //������� ������ � ����������� �������
	for (int i = 0; i < N; i++)
		if (graph[u][i] && (hight[i] < min))
			min = hight[i];
	hight[u] = min + 1; // ����������� ����� �������� ������
}

/*������������ �������(������ �������)*/
void discharge(int u, int **graph)
{
	int v = 0;
	while (excess[u] > 0) // ���� ���� ������� ���� �������� push, ���� lift
	{
		if (graph[u][v] && hight[u] == hight[v] + 1) //����� ���� � ���������� ���� � ��� ������ �� 1 ������
		{
			push(u, v, graph);
			v = 0;
			continue;
		}
		v++;
		if (v == N)  // ��������� �������� push ��� ���� �������, �� lift
		{
			lift(u, graph);
			v = 0;
		}
	}
}

/*������������� ������������ ����*/
void init(int **graph)
{
	for (int i = 0; i < N; i++)
	{
		if (i == source) continue; // ���������� �����
		excess[i] = graph[source][i]; //�����
		graph[i][source] += graph[source][i];
	}
	hight[source] = N; // ������ ������ ����� ���������� ������
}


/*���� ����� �� �����, ��������� ��� ������ ������� ���������� ����������� � ������� ���������*/
void input_matrix(ifstream &in, int **graph, int** cost)
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			in >> graph[i][j];

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			in >> cost[i][j];
}

/* ������ ����� c ����� ����(����� � graphic.png) */
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

/*����������� �������*/
void copy_matrix(int** graph, int** copy) {
	for (int i = 0; i < N; i++) {
		copy[i] = new int[N];
		for (int j = 0; j < N; j++) 
			copy[i][j] = graph[i][j];
	}
}

/*������ ���������� ����*/
int count_rib(int **graph) {
	int count = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (graph[i][j] != 0 && i < j)
				count++;
	return count;
}

/*������� �� ������� � ������ �����*/
void matrix_to_vector_rib(int** graph, int** cost, vector<vector<rib>>& graph_vector) {
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++)
			if (graph[i][j] > 0) {
				graph_vector[i].push_back({ j, graph[i][j] , cost[i][j], 0, graph_vector[j].size() });
				graph_vector[j].push_back({ i, 0, (-1)*cost[i][j], 0, graph_vector[i].size() });
			}
}

/*------------------------------------------------------------------------------------------------------------------------------*/

/*������� ������*/
int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "rus");
	ifstream ifs; ofstream ofs;

	int ans;
	do {
		cout << "������������� ���� �����? (1-��, 0 -���)." << endl;
		cin >> ans;
		if (ans == 1) system("input.txt");
	} while (ans != 1 && ans != 0);

	ifs.open("input.txt");
	ifs >> N >> source >> outlet; //����� �� ����� ���������� ������, ������ � �����
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
		cout << "������� ����? (1-��, 0 -���)." << endl;
		cin >> ans;
		if (ans == 1) print_graph(transport_network, transport_network_cost);
	} while (ans != 1 && ans != 0);

	ifs.close();

	list<int> l; // ������ ������������� �������
	list<int>::iterator cur;
	int old;

	init(transport_network_copy); // �������������

	/*�������� ������������� � ������(��� ���������� ������� ������)*/
	 //������� ������� �� ����� � ���� � ������
	for (int i = 0; i < N; i++)
		if (i != source && i != outlet)
			l.push_front(i);
	cur = l.begin();  //����� ��������� �������

	while (cur != l.end())
	{
		old = hight[*cur]; //��������� ������ ������� ����� �������������
		discharge(*cur, transport_network_copy); // ����������� �������
		if (hight[*cur] != old) { // ���� ������ ���� ������( ������ �����������)
			l.push_front(*cur);
			l.erase(cur);
			cur = l.begin(); // ���������� ��� ������� � ������ ������
		}
		cur++;
	}

	int MaxFlow = excess[outlet]; // ������������ �����

	l.clear(); // ������� ������

	/*������� ��������� � �������� ������*/
	M = count_rib(transport_network_cost);

	vector<vector<rib>> transport_network_vector(N);
	matrix_to_vector_rib(transport_network, transport_network_cost, transport_network_vector);

	/*�������� ��������-������(�������� ��������������� �����)*/
	int our_flow;
	int flow = 0, cost = 0;

	while (1) {
		cout << "������� ����� ������: "; cin >> our_flow;
		if (our_flow <= MaxFlow) {
			while (flow < our_flow) { // ��������� �������� ���� �������� ������ �� ���������� �������� �����
				vector<int> id(N, 0);
				vector<int> distance(N, INF);
				vector<int> queue(N);
				vector<int> path(N);
				vector<size_t> path_rib(N);
				int qh = 0, qt = 0;

				/*����� ���� � ����������� �������� ���������� (�� ������� ��������� ������)*/
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
				int add_flow = our_flow - flow; //������� �������� ������ 

				for (int v = outlet; v != source; v = path[v]) { //���������� �� ������� ����������� ���� � ����� �� ������ 
					 /* ���������� r = min{r(e): ��� � - �������, ���� � ����.�����.}
					r(e) �� ������ ����� r(e)= c(e) - f(e)(���������� ����������� ����� �������� �����), � �� �������� r(e) = f(e)*/
					int pv = path[v];  
					size_t pr = path_rib[v];
					add_flow = min(add_flow, transport_network_vector[pv][pr].u - transport_network_vector[pv][pr].f);
				}

				for (int v = outlet; v != source; v = path[v]) { // ���������� �� ������� ����������� ���� � ����� �� ������
					int pv = path[v];
					size_t pr = path_rib[v];
					size_t r = transport_network_vector[pv][pr].back;
					transport_network_vector[pv][pr].f += add_flow; // �� ������ ����� ����������� ����� �� r
					transport_network_vector[v][r].f -= add_flow; // �� �������� ����� ��������� ����� �� r
					cost += transport_network_vector[pv][pr].c * add_flow; //����������� �������� ������
				}
				flow += add_flow; //����������� ���������� ����� �� addflow
			}

			ofs.open("output.txt");
			ofs << "��������� �������� ������ " << flow << " �����: " << cost << ".";
			ofs.close();
			break;
		}
		else {
			cout << "�������� ����� ������ �������������! �������� ����!" << endl;
			continue;
		}
	}

	/*�������� ����������*/
	do {
		cout << "������� ���������? (1-��, �������, 0 - �� ��������)." << endl;
		cin >> ans;
		if (ans == 1) system("output.txt");

	} while (ans != 1 && ans != 0);

	return 0;
}