
import java.util.*;

public class DijkstraShortestPath {

    private static final int INFINITY = Integer.MAX_VALUE;
    private int[][] adjacencyMatrix;
    private int numVertices;

    public DijkstraShortestPath(int numVertices) {
        this.numVertices = numVertices;
        adjacencyMatrix = new int[numVertices][numVertices];
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                if (i == j) {
                    adjacencyMatrix[i][j] = 0;
                } else {
                    adjacencyMatrix[i][j] = INFINITY;
                }
            }
        }
    }

    public void addEdge(int from, int to, int weight) {
        adjacencyMatrix[from][to] = weight;
        adjacencyMatrix[to][from] = weight;
    }

    public int[] dijkstra(int startVertex) {
        boolean[] visited = new boolean[numVertices];
        int[] distances = new int[numVertices];
        Arrays.fill(distances, INFINITY);
        distances[startVertex] = 0;

        for (int i = 0; i < numVertices - 1; i++) {
            int nearestVertex = findNearestVertex(distances, visited);
            visited[nearestVertex] = true;

            for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
                if (!visited[vertexIndex] &&
                        adjacencyMatrix[nearestVertex][vertexIndex] != INFINITY &&
                        distances[nearestVertex]
                                + adjacencyMatrix[nearestVertex][vertexIndex] < distances[vertexIndex]) {

                    distances[vertexIndex] = distances[nearestVertex] + adjacencyMatrix[nearestVertex][vertexIndex];
                }
            }
        }

        return distances;
    }

    private int findNearestVertex(int[] distances, boolean[] visited) {
        int minDistance = INFINITY;
        int nearestVertex = -1;
        for (int i = 0; i < numVertices; i++) {
            if (!visited[i] && distances[i] < minDistance) {
                minDistance = distances[i];
                nearestVertex = i;
            }
        }
        return nearestVertex;
    }

    public static void main(String[] args) {
        DijkstraShortestPath graph = new DijkstraShortestPath(5);

        graph.addEdge(0, 1, 4);
        graph.addEdge(0, 2, 2);
        graph.addEdge(1, 2, 5);
        graph.addEdge(1, 3, 10);
        graph.addEdge(2, 4, 3);
        graph.addEdge(3, 4, 7);

        int[] distances = graph.dijkstra(0);

        for (int i = 0; i < distances.length; i++) {
            System.out.println("Distance from 0 to vertex " + i + " is: " + distances[i]);
        }
    }
}
