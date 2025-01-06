# Bidirectional Dijkstra with Multi-threading

## Overview

The **Bidirectional Dijkstra algorithm** is an optimization of Dijkstra's shortest path algorithm that performs two simultaneous searches: one from the source node and one from the target node. The algorithm stops when the searches meet, allowing for faster pathfinding, especially in large graphs.

To enhance performance, **multi-threading** is utilized to run the forward and backward searches concurrently, reducing the time taken to find the shortest path.

## Key Concepts

1. **Two Simultaneous Searches**:
   - **Forward Search**: Starts from the source node and explores neighboring nodes towards the target node.
   - **Backward Search**: Starts from the target node and explores neighbors towards the source node.
   
2. **Meeting Point**:
   - The searches meet when they visit a common node. This node is the "meeting point," and the path from the source to the target can be reconstructed through this node.
   
3. **Termination Condition**:
   - The search stops when both searches meet at the same node. The shortest path is then reconstructed by combining the paths from the source to the meeting node and the target to the meeting node.

4. **Multi-threading**:
   - Two threads are created to run the forward and backward searches concurrently, which reduces the search time.
   - A **mutex** is used to synchronize shared data, such as the visited nodes and distance updates.

## Algorithm Overview

### 1. Initialize Data Structures
   - **Graph**: A graph is represented as an adjacency list, where each node has its neighbors and edge weights.
   - **Distance Arrays**: Two arrays (`dist_forward` and `dist_backward`) store the shortest distances found so far from the source and target nodes.
   - **Predecessor Arrays**: Arrays (`forward_pred` and `backward_pred`) store the predecessor of each node for path reconstruction.
   - **Priority Queues**: Two priority queues (`pq_forward` and `pq_backward`) are used to select the next node to explore based on the shortest tentative distance.
   - **Visited Sets**: Two sets (`visited_forward` and `visited_backward`) track visited nodes during the forward and backward searches.

### 2. Start the Searches
   - The forward search starts from the source node, and the backward search starts from the target node. Both searches are initialized with a distance of zero.

### 3. Run the Searches in Parallel
   - **Forward Search**: A thread runs the forward search from the source node, checking if nodes visited by the forward search are also visited by the backward search. If so, the shortest path is updated.
   - **Backward Search**: Another thread runs the backward search from the target node, similarly checking for intersections with the forward search.

### 4. Mutex for Synchronization
   - A **mutex** (`mtx`) is used to protect shared data structures, ensuring that only one thread can modify or access shared data at a time.

### 5. Check for Meeting Points
   - During both searches, the algorithm checks for nodes that have been visited by both searches. If such a node is found, it calculates the total distance from the source to the target through this node and updates the shortest path.

### 6. Reconstruct the Shortest Path
   - After the searches meet, the shortest path is reconstructed by combining the forward path (from the source to the meeting node) and the backward path (from the target to the meeting node).

### 7. Termination
   - The algorithm terminates as soon as the forward and backward searches meet or when all nodes have been processed.

## Thread Functionality

- **Forward Search (Thread 1)**: Explores the graph from the source node, updating distances and checking for intersections with the backward search.
- **Backward Search (Thread 2)**: Explores the graph from the target node, updating distances and checking for intersections with the forward search.

Both threads continue until a meeting point is found or all nodes have been processed.

## Code Example

```cpp
// Threaded Forward Search
void forward_search(...) {
    while (!pq_forward.empty()) {
        // Pop the top element from the priority queue
        // Visit and relax edges
        // Check for intersection with backward search
        // Update best distance and meeting node if a path is found
    }
}

// Threaded Backward Search
void backward_search(...) {
    while (!pq_backward.empty()) {
        // Pop the top element from the priority queue
        // Visit and relax edges
        // Check for intersection with forward search
        // Update best distance and meeting node if a path is found
    }
}

// Main function running both searches in parallel
std::pair<int, std::vector<int>> bidirectional_dijkstra(...) {
    // Initialize data structures
    // Start both forward and backward searches in parallel using threads
    // Wait for both threads to finish
    // Reconstruct and return the shortest path if found
}
```
## Advantages of Multi-threading in Bidirectional Dijkstra

1. **Reduced Search Space**:  
   By performing both searches concurrently, we reduce the search space each search needs to explore, leading to faster computation.

2. **Faster Convergence**:  
   The algorithm meets in the middle, reducing the overall distance that needs to be explored, which is especially beneficial in large graphs.

3. **Efficiency**:  
   Multi-threading takes advantage of multiple CPU cores, speeding up the algorithm for large-scale graphs, leading to significant performance improvements in real-time applications.

4. **Scalability**:  
   Multi-threading allows the algorithm to scale better with increasing graph sizes, ensuring that the performance benefits become more evident as the graph grows.

---

## Challenges

1. **Thread Synchronization**:  
   Proper synchronization is crucial to avoid race conditions when threads access shared resources. Mutexes or other synchronization techniques ensure thread safety for shared data. However, improper synchronization can lead to deadlocks or inconsistent results.

2. **Increased Complexity**:  
   Multi-threading adds complexity to the implementation, requiring careful management of threads, synchronization, and data sharing. Debugging and testing multi-threaded code can also be more difficult compared to single-threaded solutions.

3. **Overhead**:  
   While multi-threading can improve performance, there is an overhead involved in managing threads. For smaller graphs, the overhead of creating and synchronizing threads might outweigh the performance benefits.

4. **Resource Contention**:  
   When multiple threads try to access shared resources (like the priority queues or visited nodes), it can lead to contention. Proper thread management and resource allocation are necessary to avoid bottlenecks.

5. **Deadlocks**:  
   If not carefully managed, multi-threaded programs can encounter deadlocks where threads wait indefinitely for each other to release resources. This can result in performance degradation and unpredictable behavior.
