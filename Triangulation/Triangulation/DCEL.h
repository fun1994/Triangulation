#pragma once
#include <vector>
#include <string>

class Vertex {
public:
	double x;
	double y;
	int incidentEdge;
	std::string type;
	bool left;
	bool right;
};

class HalfEdge {
public:
	int origin;
	int twin;
	int prev;
	int next;
	int incidentFace;
	int helper;
};

class Face {
public:
	int outerComponent;
	std::vector<int> innerComponents;
};

class DCEL {
public:
	std::vector<Vertex> V;
	std::vector<HalfEdge> E;
	std::vector<Face> F;
};
