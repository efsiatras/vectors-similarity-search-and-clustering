CC=gcc
CFLAGS=-g

LSH_OBJS=hashtable/hashnode.o hashtable/hashtable.o hashtable/htArray.o images/image.o images/imageArray.o images/imageNode.o utils/functions.o utils/timer.o main_lsh.o 
CUBE_OBJS=hypercube/fArray.o hypercube/vertex.o hypercube/hypercube.o images/image.o images/imageArray.o images/imageNode.o utils/functions.o utils/timer.o main_cube.o 
CLUSTER_OBJS=kmedians/kmedians.o images/image.o images/imageArray.o images/imageNode.o hypercube/fArray.o hypercube/vertex.o hypercube/hypercube.o utils/functions.o utils/timer.o main_cluster.o hashtable/hashnode.o hashtable/htArray.o hashtable/hashtable.o
TOTAL_OBJS=hashtable/hashnode.o hashtable/hashtable.o hashtable/htArray.o hypercube/fArray.o hypercube/vertex.o hypercube/hypercube.o images/image.o images/imageArray.o images/imageNode.o utils/functions.o utils/timer.o kmedians/kmedians.o main_lsh.o main_cube.o main_cluster.o

all: lsh cube cluster

lsh: $(LSH_OBJS)
	$(CC) $(LSH_OBJS) -o lsh $(CFLAGS)

cube: $(CUBE_OBJS)
	$(CC) $(CUBE_OBJS) -o cube $(CFLAGS)

cluster: $(CLUSTER_OBJS)
	$(CC) $(CLUSTER_OBJS) -o cluster $(CFLAGS)

.PHONY: clean

clean:
	rm -f lsh cube cluster $(TOTAL_OBJS)