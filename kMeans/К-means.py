import numpy as np
import matplotlib.pyplot as plt
import math
import random
import sys


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return f'({self.x}, {self.y})'

    def __repr__(self):
        return f'({self.x}, {self.y})'


class Cluster:
    def __init__(self, centroid):
        self.centroid = centroid
        self.points = []

    def __str__(self):
        return f'Center: {self.centroid}'

    def __repr__(self):
        return f'Center: {self.centroid}'

    def add_point(self, point):
        self.points.append(point)

    def remove_point(self, point):
        self.points.remove(point)

    def distance_to_centroid(self, point):
        return math.sqrt(
            ((self.centroid.x - point.x) ** 2) +
            ((self.centroid.y - point.y) ** 2)
        )

    def update_centroid(self):
        allX = sum([point.x for point in self.points])
        allY = sum([point.y for point in self.points])
        self.centroid = Point(allX / len(self.points), allY / len(self.points))


def kmeans(clustersCount, all_points):
    # Pick random points from our data as the initial centroids
    centroids = random.choices(all_points, k=clustersCount)

    clusters = []
    for i in range(clustersCount):
        clusters.append(Cluster(centroids[i]))

    # Initialization
    for point in all_points:
        min_distance = float("inf")

        for cluster in clusters:
            current_distance = cluster.distance_to_centroid(point)
            if (current_distance < min_distance):
                min_distance = current_distance
                closest_cluster = cluster

        closest_cluster.add_point(point)

    for cluster in clusters:
        cluster.update_centroid()

    # Update Centroid
    should_continue = True
    while (should_continue):
        should_continue = False
        for cluster in clusters:
            for point in cluster.points:
                min_distance = float("inf")
                for c in clusters:
                    current_distance = c.distance_to_centroid(point)
                    if (current_distance < min_distance):
                        min_distance = current_distance
                        closest_cluster = c
                if (cluster != closest_cluster):
                    should_continue = True
                    cluster.remove_point(point)
                    closest_cluster.add_point(point)

        for cluster in clusters:
            cluster.update_centroid()

    return clusters


def printPoints(clusters):
    for c in clusters:
        print(c, len(c.points))


def kmeans_with_random_restart(clustersCount, all_points, times):
    # Minimize within-cluster point scatter:
    min_within_point_scatter = float("inf")
    for i in range(times):
        clusters = kmeans(clustersCount, all_points)
        within_point_scatter = 0
        for cluster in clusters:
            sum_of_distance = 0
            for point in cluster.points:
                sum_of_distance += cluster.distance_to_centroid(point)
            within_point_scatter += len(cluster.points) * sum_of_distance
        if (within_point_scatter < min_within_point_scatter):
            min_within_point_scatter = within_point_scatter
            min_clusters = clusters

    return min_clusters


def read_data_from_file(file_name):
    coordinates = []
    with open(file_name, "r") as f:
        allLines = f.readlines()
        for line in allLines:
            coordinates.append([float(n)
                                for n in line.rstrip('\n').split('\t')])
    return coordinates


if __name__ == "__main__":
    import pathlib
    # file_name = str(pathlib.Path(__file__).parent.absolute()) + "\\normal.txt"
    file_name = str(pathlib.Path(__file__).parent.absolute()) + "\\unbalanced.txt"

    k = 8  # clustersCount
    all_points = [Point(x, y) for x, y in read_data_from_file(file_name)]
    # clusters = kmeans(k, all_points)
    times = 10
    clusters = kmeans_with_random_restart(k, all_points, times)

    for cluster in clusters:
        x = [point.x for point in cluster.points]
        y = [point.y for point in cluster.points]
        plt.scatter(
            x, y, c=[(random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1))])
    plt.grid(True)
    plt.show()
