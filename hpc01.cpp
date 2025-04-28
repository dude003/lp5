
#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>
using namespace std;

void bfs(int start, const vector<vector<int>>& adj, vector<bool>& visited) {
    queue<int> q;
    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int node = q.front(); q.pop();
        cout << "Visited (BFS) Node: " << node << endl;

        vector<int> neighbors;

        #pragma omp parallel for
        for (int i = 0; i < (int)adj[node].size(); ++i) {
            int neighbor = adj[node][i];
            if (!visited[neighbor]) {
                #pragma omp critical
                {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        neighbors.push_back(neighbor);
                        cout << "Thread " << omp_get_thread_num()
                             << " processing neighbor (BFS): " << neighbor << endl;
                    }
                }
            }
        }

        for (int v : neighbors) q.push(v);
    }
}

void dfs(int node, const vector<vector<int>>& adj, vector<bool>& visited) {
    int tid = omp_get_thread_num();
    #pragma omp critical
    {
        visited[node] = true;
        cout << "Thread " << tid << " visited (DFS) Node: " << node << endl;
    }

    for (int neighbor : adj[node]) {
        bool go = false;
        #pragma omp critical
        {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                go = true;
                cout << "Thread " << tid << " preparing DFS for neighbor: "
                     << neighbor << endl;
            }
        }
        if (go) {
            #pragma omp task
            dfs(neighbor, adj, visited);
        }
    }
    #pragma omp taskwait
}

int main() {
    // Uncomment to hard-set thread count in code:
    // omp_set_num_threads(8);

    int n, m;
    cout << "Enter number of nodes and edges: ";
    cin >> n >> m;

    vector<vector<int>> adj(n);
    cout << "Enter edges (u v):\n";
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<bool> visited(n, false);

    cout << "\nBFS starting from node 0:\n";
    double t0 = omp_get_wtime();
    bfs(0, adj, visited);
    cout << "Time taken for BFS: " << (omp_get_wtime() - t0) << " sec\n";

    fill(visited.begin(), visited.end(), false);
    cout << "\nDFS starting from node 0:\n";
    t0 = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        dfs(0, adj, visited);
    }
    cout << "Time taken for DFS: " << (omp_get_wtime() - t0) << " sec\n";
    return 0;
}
