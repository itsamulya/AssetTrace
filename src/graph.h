#ifndef GRAPH_H
#define GRAPH_H

#define MAX_LOCATIONS 20
#define INF 999999

typedef struct {
    char name[50];
} Location;

typedef struct {
    int graph[MAX_LOCATIONS][MAX_LOCATIONS];
    Location locations[MAX_LOCATIONS];
    int locationCount;
} CampusGraph;

void initializeGraph(CampusGraph *campus);

int addLocation(
    CampusGraph *campus,
    const char *name
);

void addConnection(
    CampusGraph *campus,
    int source,
    int destination,
    int distance
);

int findLocation(
    CampusGraph *campus,
    const char *name
);

int dijkstra(
    CampusGraph *campus,
    int source,
    int destination
);

void buildCampusGraph(
    CampusGraph *campus
);

#endif /* GRAPH_H */