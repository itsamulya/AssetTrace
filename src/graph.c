#include <stdio.h>
#include <string.h>

#include "graph.h"

/*
    Initialize the graph.

    Every location initially has
    no connection with another location.
*/
void initializeGraph(
    CampusGraph *campus
) {
    if (campus == NULL) {
        return;
    }

    campus->locationCount = 0;

    for (int i = 0; i < MAX_LOCATIONS; i++) {
        for (int j = 0; j < MAX_LOCATIONS; j++) {
            if (i == j) {
                campus->graph[i][j] = 0;
            } else {
                campus->graph[i][j] = INF;
            }
        }
    }
}

/*
    Add a location to the graph.
*/
int addLocation(
    CampusGraph *campus,
    const char *name
) {
    if (campus == NULL || name == NULL) {
        return -1;
    }

    if (campus->locationCount >= MAX_LOCATIONS) {
        return -1;
    }

    int index = campus->locationCount;

    strncpy(campus->locations[index].name, name, sizeof(campus->locations[index].name) - 1);
    campus->locations[index].name[sizeof(campus->locations[index].name) - 1] = '\0';

    campus->locationCount++;

    return index;
}

/*
    Add a weighted connection between
    two locations.
*/
void addConnection(
    CampusGraph *campus,
    int source,
    int destination,
    int distance
) {
    if (campus == NULL) {
        return;
    }

    if (source < 0 || source >= MAX_LOCATIONS ||
        destination < 0 || destination >= MAX_LOCATIONS) {
        return;
    }

    campus->graph[source][destination] = distance;
    campus->graph[destination][source] = distance;
}

/*
    Find the numerical index of
    a location by its name.
*/
int findLocation(
    CampusGraph *campus,
    const char *name
) {
    if (campus == NULL || name == NULL) {
        return -1;
    }

    for (int i = 0; i < campus->locationCount; i++) {
        if (strcmp(campus->locations[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

/*
    Find the unvisited vertex
    with the smallest distance.
*/
int findMinimumDistance(
    int distances[],
    int visited[],
    int count
) {
    int minimum = INF;
    int minimumIndex = -1;

    for (int i = 0; i < count; i++) {
        if (!visited[i] && distances[i] < minimum) {
            minimum = distances[i];
            minimumIndex = i;
        }
    }

    return minimumIndex;
}

/*
    Dijkstra's shortest-path algorithm.
*/
int dijkstra(
    CampusGraph *campus,
    int source,
    int destination
) {
    if (campus == NULL ||
        source < 0 || source >= campus->locationCount ||
        destination < 0 || destination >= campus->locationCount) {
        return INF;
    }

    int distances[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS];

    /*
        Initialize distances.
    */
    for (int i = 0; i < campus->locationCount; i++) {
        distances[i] = INF;
        visited[i] = 0;
    }

    distances[source] = 0;

    /*
        Process every vertex.
    */
    for (int step = 0; step < campus->locationCount; step++) {
        int current = findMinimumDistance(
            distances,
            visited,
            campus->locationCount
        );

        if (current == -1) {
            break;
        }

        visited[current] = 1;

        /*
            Relax neighboring vertices.
        */
        for (int neighbor = 0; neighbor < campus->locationCount; neighbor++) {
            if (campus->graph[current][neighbor] != INF) {
                int newDistance = distances[current] + campus->graph[current][neighbor];

                if (newDistance < distances[neighbor]) {
                    distances[neighbor] = newDistance;
                }
            }
        }
    }

    return distances[destination];
}

/*
    Build standard campus graph topology.
*/
void buildCampusGraph(
    CampusGraph *campus
) {
    if (campus == NULL) {
        return;
    }

    initializeGraph(campus);

    int library = addLocation(campus, "Library");
    int blockA  = addLocation(campus, "Block A");
    int blockB  = addLocation(campus, "Block B");
    int canteen = addLocation(campus, "Canteen");
    int gate    = addLocation(campus, "Main Gate");

    addConnection(campus, library, blockA, 3);
    addConnection(campus, library, canteen, 6);
    addConnection(campus, blockA, blockB, 2);
    addConnection(campus, blockB, canteen, 3);
    addConnection(campus, canteen, gate, 5);
    addConnection(campus, blockB, gate, 8);
}