#ifndef MISSION_PLANNING_H
#define MISSION_PLANNING_H

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <atomic>

// using namespace std;

struct Edge
{
  int to;
  int weight;
};

// Class for Mission Planning
class MissionPlanning
{
  using PQElement = std::pair<int, int>;                           // Pair to store distance and node
  using Graph = std::vector<std::vector<Edge>>;                    // Graph represented as an adjacency list
  std::mutex mtx;                                                  // Mutex for thread synchronization
  std::atomic<int> best_distance{std::numeric_limits<int>::max()}; // Atomic best distance
  std::atomic<int> meeting_node{-1};                               // Atomic node where the paths meet
  std::atomic<bool> found_path{false};                             // Atomic flag to indicate path found

  Graph graph, reverse_graph;

  // Function to reconstruct the path from source to target via meeting node
  std::vector<int> reconstruct_path(const std::vector<int> &forward_pred, const std::vector<int> &backward_pred, int meeting_node)
  {
    std::vector<int> path;

    // Reconstruct the forward part of the path
    for (int u = meeting_node; u != -1; u = forward_pred[u])
    {
      path.push_back(u);
    }
    reverse(path.begin(), path.end()); // Reverse to get the correct order

    // Reconstruct the backward part of the path
    for (int u = backward_pred[meeting_node]; u != -1; u = backward_pred[u])
    {
      path.push_back(u);
    }

    return path;
  }

  // Function for forward search in bidirectional Dijkstra
  void forward_search(const Graph &graph, std::vector<int> &dist_forward, std::vector<int> &forward_pred,
                      std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> &pq_forward,
                      std::set<int> &visited_forward, std::set<int> &visited_backward)
  {
    while (!pq_forward.empty())
    {
      auto top = pq_forward.top();
      auto dist = top.first;
      auto u = top.second;
      pq_forward.pop();

      {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety
        if (visited_forward.count(u))
          continue;
        visited_forward.insert(u);

        // Check if the node is visited in the backward search
        if (visited_backward.count(u))
        {
          int total_distance = dist_forward[u] + dist;
          if (total_distance < best_distance.load())
          {
            best_distance.store(total_distance);
            meeting_node.store(u);
            found_path.store(true); // Mark that a path is found
          }
        }
      }

      // Relax edges for forward search
      for (const auto &edge : graph[u])
      {
        int v = edge.to, weight = edge.weight;
        if (dist_forward[v] > dist + weight)
        {
          dist_forward[v] = dist + weight;
          forward_pred[v] = u;
          pq_forward.emplace(dist_forward[v], v);
        }
      }
    }
  }

  // Thread function for backward search in bidirectional Dijkstra
  void backward_search(const Graph &reverse_graph, std::vector<int> &dist_backward, std::vector<int> &backward_pred,
                       std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> &pq_backward,
                       std::set<int> &visited_forward, std::set<int> &visited_backward)
  {
    while (!pq_backward.empty())
    {
      auto top = pq_backward.top();
      auto dist = top.first;
      auto u = top.second;
      pq_backward.pop();

      {
        std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety
        if (visited_backward.count(u))
          continue;
        visited_backward.insert(u);

        // Check if the node is visited in the forward search
        if (visited_forward.count(u))
        {
          int total_distance = dist_backward[u] + dist;
          if (total_distance < best_distance.load())
          {
            best_distance.store(total_distance);
            meeting_node.store(u);
            found_path.store(true); // Mark that a path is found
          }
        }
      }

      // Relax edges for backward search
      for (const auto &edge : reverse_graph[u])
      {
        int v = edge.to, weight = edge.weight;
        if (dist_backward[v] > dist + weight)
        {
          dist_backward[v] = dist + weight;
          backward_pred[v] = u;
          pq_backward.emplace(dist_backward[v], v);
        }
      }
    }
  }

  // Main function for bidirectional Dijkstra's algorithm
  std::pair<int, std::vector<int>> bidirectional_dijkstra(const Graph &graph, const Graph &reverse_graph, int source, int target)
  {
    int n = graph.size();
    std::vector<int> dist_forward(n, std::numeric_limits<int>::max());
    std::vector<int> dist_backward(n, std::numeric_limits<int>::max());
    std::vector<int> forward_pred(n, -1);
    std::vector<int> backward_pred(n, -1);
    std::set<int> visited_forward, visited_backward;

    // Priority queues for forward and backward searches
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq_forward;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq_backward;

    dist_forward[source] = 0;
    dist_backward[target] = 0;
    pq_forward.emplace(0, source);
    pq_backward.emplace(0, target);

    // Launch threads for forward and backward searches
    std::thread forward_thread(&MissionPlanning::forward_search, this, ref(graph), ref(dist_forward), ref(forward_pred),
                               ref(pq_forward), ref(visited_forward), ref(visited_backward));
    std::thread backward_thread(&MissionPlanning::backward_search, this, ref(reverse_graph), ref(dist_backward), ref(backward_pred),
                                ref(pq_backward), ref(visited_forward), ref(visited_backward));

    // Wait for threads to complete
    forward_thread.join();
    backward_thread.join();

    // If no path is found, return -1 and an empty path
    if (!found_path.load())
      return {-1, {}};

    // Reconstruct the path and return the result
    std::vector<int> path = reconstruct_path(forward_pred, backward_pred, meeting_node.load());
    return {best_distance.load(), path};
  }

public:
  // Constructor to initialize the graph with random weights
  MissionPlanning()
  {
    int nodes = 100; // Number of nodes
    graph.resize(nodes);
    reverse_graph.resize(nodes);

    // Initialize the graph with random weights
    for (int i = 0; i < nodes; i++)
    {
      for (int j = i + 1; j < nodes; j++)
      {
        int weight = rand() % 100 + 1;
        graph[i].push_back({j, weight});
        graph[j].push_back({i, weight});
      }
    }

    // Create the reverse graph
    for (int i = 0; i < nodes; i++)
    {
      for (const auto &edge : graph[i])
      {
        reverse_graph[edge.to].push_back({i, edge.weight});
      }
    }
  }

  std::vector<int> getShortestPath(int source, int target)
  {
    if (source < 0 || target < 0 || source >= 100 || target >= 100)
    {
      std::cout << "Invalid Nodes" << std::endl;
      return {};
    }

    auto top = bidirectional_dijkstra(graph, reverse_graph, source, target);
    auto shortest_distance_bi = top.first;
    auto path_bi = top.second;
    std::cout << "Shortest path distance: " << shortest_distance_bi << std::endl;

    return path_bi;
  }
};

#endif // MISSION_PLANNING_H
