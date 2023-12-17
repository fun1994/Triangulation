#pragma once
#include <stack>
#include "StatusStructure.h"

class Triangulation {
	double area2(Vertex& p, Vertex& q, Vertex& r) {
		return p.x * q.y - p.y * q.x + q.x * r.y - q.y * r.x + r.x * p.y - r.y * p.x;
	}
	bool toLeft(Vertex& p, Vertex& q, Vertex& r) {
		return area2(p, q, r) > 0;
	}
	bool toLeft(DCEL& P, HalfEdge& e, Vertex& v) {
		return toLeft(P.V[e.origin], P.V[P.E[e.twin].origin], v);
	}
	void determineVertexType(DCEL& P) {
		for (int i = 0; i < P.V.size(); i++) {
			if (P.V[P.E[P.E[P.V[i].incidentEdge].prev].origin].y < P.V[i].y && P.V[P.E[P.E[P.V[i].incidentEdge].next].origin].y < P.V[i].y) {
				if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
					if (toLeft(P, P.E[P.V[i].incidentEdge], P.V[P.E[P.E[P.V[i].incidentEdge].prev].origin])) {
						P.V[i].type = "START";
					}
					else {
						P.V[i].type = "SPLIT";
					}
				}
				else {
					if (toLeft(P, P.E[P.V[i].incidentEdge], P.V[P.E[P.E[P.V[i].incidentEdge].prev].origin])) {
						P.V[i].type = "SPLIT";
					}
					else {
						P.V[i].type = "START";
					}
				}
			}
			else if (P.V[i].y < P.V[P.E[P.E[P.V[i].incidentEdge].prev].origin].y && P.V[i].y < P.V[P.E[P.E[P.V[i].incidentEdge].next].origin].y) {
				if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
					if (toLeft(P, P.E[P.V[i].incidentEdge], P.V[P.E[P.E[P.V[i].incidentEdge].prev].origin])) {
						P.V[i].type = "END";
					}
					else {
						P.V[i].type = "MERGE";
					}
				}
				else {
					if (toLeft(P, P.E[P.V[i].incidentEdge], P.V[P.E[P.E[P.V[i].incidentEdge].prev].origin])) {
						P.V[i].type = "MERGE";
					}
					else {
						P.V[i].type = "END";
					}
				}
			}
			else {
				P.V[i].type = "REGULAR";
			}
		}
	}
	int partition(DCEL& P, std::vector<int>& Q, int low, int high) {
		int pivot = Q[low];
		while (low < high) {
			while (low < high && P.V[Q[high]].y <= P.V[pivot].y) {
				high--;
			}
			Q[low] = Q[high];
			while (low < high && P.V[Q[low]].y >= P.V[pivot].y) {
				low++;
			}
			Q[high] = Q[low];
		}
		Q[low] = pivot;
		return low;
	}
	void quickSort(DCEL& P, std::vector<int>& Q, int low, int high) {
		if (low < high) {
			int pivot = partition(P, Q, low, high);
			quickSort(P, Q, low, pivot - 1);
			quickSort(P, Q, pivot + 1, high);
		}
	}
	void initializeEventQueue(DCEL& P, std::vector<int>& Q) {
		Q.resize(P.V.size());
		for (int i = 0; i < Q.size(); i++) {
			Q[i] = i;
		}
		quickSort(P, Q, 0, Q.size() - 1);
	}
	double getAngle(DCEL& P, HalfEdge& e1, HalfEdge& e2) {
		Vertex v1 = P.V[e1.origin];
		Vertex v2 = P.V[e2.origin];
		Vertex v3 = P.V[P.E[e2.twin].origin];
		double innerProduct = (v1.x - v2.x) * (v3.x - v2.x) + (v1.y - v2.y) * (v3.y - v2.y);
		double norm1 = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
		double norm2 = sqrt(pow(v3.x - v2.x, 2) + pow(v3.y - v2.y, 2));
		double angle = innerProduct / (norm1 * norm2);
		return angle;
	}
	int nextEdge(DCEL& P, HalfEdge& e) {
		int j = P.V[P.E[e.twin].origin].incidentEdge;
		int k = j;
		while (true) {
			j = P.E[P.E[j].twin].next;
			if (j == P.V[P.E[e.twin].origin].incidentEdge) {
				return k;
			}
			bool toLeft1 = toLeft(P, e, P.V[P.E[P.E[j].twin].origin]);
			bool toLeft2 = toLeft(P, e, P.V[P.E[P.E[k].twin].origin]);
			double angle1 = getAngle(P, e, P.E[j]);
			double angle2 = getAngle(P, e, P.E[k]);
			if (toLeft1 && toLeft2) {
				if (angle2 < angle1) {
					k = j;
				}
			}
			else if (toLeft1 && !toLeft2) {
				k = j;
			}
			else if (!toLeft1 && !toLeft2) {
				if (angle1 < angle2) {
					k = j;
				}
			}
		}
	}
	void addEdge(DCEL& P, int i, int j) {
		P.E.resize(P.E.size() + 2);
		P.E[P.E.size() - 2].origin = i;
		P.E[P.E.size() - 2].twin = P.E.size() - 1;
		P.E[P.E.size() - 1].origin = j;
		P.E[P.E.size() - 1].twin = P.E.size() - 2;
		int k = nextEdge(P, P.E[P.E.size() - 1]);
		P.E[P.E.size() - 2].prev = P.E[k].prev;
		P.E[P.E[k].prev].next = P.E.size() - 2;
		P.E[P.E.size() - 1].next = k;
		P.E[k].prev = P.E.size() - 1;
		k = nextEdge(P, P.E[P.E.size() - 2]);
		P.E[P.E.size() - 1].prev = P.E[k].prev;
		P.E[P.E[k].prev].next = P.E.size() - 1;
		P.E[P.E.size() - 2].next = k;
		P.E[k].prev = P.E.size() - 2;
	}
	void handleStartVertex(DCEL& P, StatusStructure& T, int i) {
		if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
			T.insert(P, P.V[i].incidentEdge, P.V[i].y);
			P.E[P.V[i].incidentEdge].helper = i;
		}
		else {
			T.insert(P, P.E[P.E[P.V[i].incidentEdge].twin].next, P.V[i].y);
			P.E[P.E[P.E[P.V[i].incidentEdge].twin].next].helper = i;
		}
	}
	void handleEndVertex(DCEL& P, StatusStructure& T, int i) {
		if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
			if (P.V[P.E[P.E[P.V[i].incidentEdge].prev].helper].type == "MERGE") {
				addEdge(P, i, P.E[P.E[P.V[i].incidentEdge].prev].helper);
			}
			T.remove(P, P.E[P.E[P.E[P.V[i].incidentEdge].twin].next].twin, P.V[i].y);
		}
		else {
			if (P.V[P.E[P.E[P.V[i].incidentEdge].twin].helper].type == "MERGE") {
				addEdge(P, i, P.E[P.E[P.V[i].incidentEdge].twin].helper);
			}
			T.remove(P, P.E[P.V[i].incidentEdge].twin, P.V[i].y);
		}
	}
	void handleSplitVertex(DCEL& P, StatusStructure& T, int i) {
		int j = T.search(P, P.V[i]);
		addEdge(P, i, P.E[j].helper);
		P.E[j].helper = i;
		if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
			T.insert(P, P.V[i].incidentEdge, P.V[i].y);
			P.E[P.V[i].incidentEdge].helper = i;
		}
		else {
			T.insert(P, P.E[P.E[P.V[i].incidentEdge].prev].twin, P.V[i].y);
			P.E[P.E[P.E[P.V[i].incidentEdge].prev].twin].helper = i;
		}
	}
	void handleMergeVertex(DCEL& P, StatusStructure& T, int i) {
		if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
			if (P.V[P.E[P.E[P.V[i].incidentEdge].prev].helper].type == "MERGE") {
				addEdge(P, i, P.E[P.E[P.V[i].incidentEdge].prev].helper);
			}
			T.remove(P, P.E[P.E[P.E[P.V[i].incidentEdge].twin].next].twin, P.V[i].y);
		}
		else {
			if (P.V[P.E[P.E[P.V[i].incidentEdge].twin].helper].type == "MERGE") {
				addEdge(P, i, P.E[P.E[P.V[i].incidentEdge].twin].helper);
			}
			T.remove(P, P.E[P.V[i].incidentEdge].twin, P.V[i].y);
		}
		int j = T.search(P, P.V[i]);
		if (P.V[P.E[j].helper].type == "MERGE") {
			addEdge(P, i, P.E[j].helper);
		}
		P.E[j].helper = i;
	}
	void handleRegularVertex(DCEL& P, StatusStructure& T, int i) {
		if (P.V[P.E[P.E[P.V[i].incidentEdge].twin].origin].y < P.V[i].y) {
			if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
				if (P.V[P.E[P.E[P.V[i].incidentEdge].prev].helper].type == "MERGE") {
					addEdge(P, i, P.E[P.E[P.V[i].incidentEdge].prev].helper);
				}
				T.remove(P, P.E[P.E[P.E[P.V[i].incidentEdge].twin].next].twin, P.V[i].y);
				T.insert(P, P.V[i].incidentEdge, P.V[i].y);
				P.E[P.V[i].incidentEdge].helper = i;
			}
			else {
				int j = T.search(P, P.V[i]);
				if (P.V[P.E[j].helper].type == "MERGE") {
					addEdge(P, i, P.E[j].helper);
				}
				P.E[j].helper = i;
			}
		}
		else {
			if (0 <= P.F[P.E[P.V[i].incidentEdge].incidentFace].outerComponent) {
				int j = T.search(P, P.V[i]);
				if (P.V[P.E[j].helper].type == "MERGE") {
					addEdge(P, i, P.E[j].helper);
				}
				P.E[j].helper = i;
			}
			else {
				if (P.V[P.E[P.E[P.V[i].incidentEdge].twin].helper].type == "MERGE") {
					addEdge(P, i, P.E[P.E[P.V[i].incidentEdge].twin].helper);
				}
				T.remove(P, P.E[P.V[i].incidentEdge].twin, P.V[i].y);
				T.insert(P, P.E[P.E[P.V[i].incidentEdge].prev].twin, P.V[i].y);
				P.E[P.E[P.E[P.V[i].incidentEdge].prev].twin].helper = i;
			}
		}
	}
	void generateFaces(DCEL& P, int sizeE) {
		int i;
		for (int j = 0; j < P.F.size(); j++) {
			if (0 <= P.F[j].outerComponent && P.F[j].innerComponents.size() == P.F.size() - 2) {
				i = j;
				break;
			}
		}
		if (sizeE == P.E.size()) {
			if (i == 0) {
				int outerComponent = P.F[0].outerComponent;
				P.F.erase(P.F.begin());
				int j = P.F[0].innerComponents[0];
				do {
					P.E[j].incidentFace = 0;
					j = P.E[j].next;
				} while (j != P.F[0].innerComponents[0]);
				P.F.resize(2);
				P.F[1].outerComponent = outerComponent;
				j = P.F[1].outerComponent;
				do {
					P.E[j].incidentFace = 1;
					j = P.E[j].next;
				} while (j != P.F[1].outerComponent);
			}
		}
		else if (sizeE < P.E.size()) {
			P.F.erase(P.F.begin() + i);
			for (int j = i; j < P.F.size(); j++) {
				int index = P.F[j].outerComponent < 0 ? P.F[j].innerComponents[0] : P.F[j].outerComponent;
				int k = index;
				do {
					P.E[k].incidentFace = j;
					k = P.E[k].next;
				} while (k != index);
			}
			for (int j = sizeE; j < P.E.size(); j++) {
				P.E[j].incidentFace = -1;
			}
			for (int j = sizeE; j < P.E.size(); j++) {
				if (P.E[j].incidentFace < 0) {
					P.F.resize(P.F.size() + 1);
					P.F[P.F.size() - 1].outerComponent = j;
					int k = j;
					do {
						P.E[k].incidentFace = P.F.size() - 1;
						k = P.E[k].next;
					} while (k != j);
				}
			}
		}
	}
	int lowest(DCEL& P, std::vector<int>& Q) {
		int left = 0;
		int right = Q.size() - 1;
		if (P.V[Q[0]].y < P.V[Q[Q.size() - 1]].y) {
			while (left < right) {
				int mid = (left + right) / 2;
				if (P.V[Q[mid]].y < P.V[Q[mid + 1]].y) {
					right = mid;
				}
				else {
					if (P.V[Q[mid]].y > P.V[Q[0]].y) {
						right = mid - 1;
					}
					else {
						left = mid + 1;
					}
				}
			}
		}
		else {
			while (left < right) {
				int mid = (left + right) / 2;
				if (P.V[Q[mid]].y > P.V[Q[mid + 1]].y) {
					left = mid + 1;
				}
				else {
					if (P.V[Q[mid]].y > P.V[Q[Q.size() - 1]].y) {
						left = mid + 1;
					}
					else {
						right = mid;
					}
				}
			}
		}
		return left;
	}
	int highest(DCEL& P, std::vector<int>& Q) {
		int left = 0;
		int right = Q.size() - 1;
		if (P.V[Q[0]].y < P.V[Q[Q.size() - 1]].y) {
			while (left < right) {
				int mid = (left + right) / 2;
				if (P.V[Q[mid]].y < P.V[Q[mid + 1]].y) {
					left = mid + 1;
				}
				else {
					if (P.V[Q[mid]].y < P.V[Q[Q.size() - 1]].y) {
						left = mid + 1;
					}
					else {
						right = mid;
					}
				}
			}
		}
		else {
			while (left < right) {
				int mid = (left + right) / 2;
				if (P.V[Q[mid]].y > P.V[Q[mid + 1]].y) {
					right = mid;
				}
				else {
					if (P.V[Q[mid]].y < P.V[Q[0]].y) {
						right = mid - 1;
					}
					else {
						left = mid + 1;
					}
				}
			}
		}
		return left;
	}
	void monotonePartitioning(DCEL& P, std::vector<int>& L, std::vector<int>& R, Face& f) {
		std::vector<int> Q;
		int i = f.outerComponent;
		Q.push_back(P.E[i].origin);
		while (true) {
			i = P.E[i].next;
			if (i == f.outerComponent) {
				break;
			}
			Q.push_back(P.E[i].origin);
		}
		int low = lowest(P, Q);
		int high = highest(P, Q);
		if (low < high) {
			L.insert(L.end(), Q.begin() + high, Q.end());
			L.insert(L.end(), Q.begin(), Q.begin() + (low + 1));
			R.insert(R.end(), Q.begin() + low, Q.begin() + (high + 1));
		}
		else {
			L.insert(L.end(), Q.begin() + high, Q.begin() + (low + 1));
			R.insert(R.end(), Q.begin() + low, Q.end());
			R.insert(R.end(), Q.begin(), Q.begin() + (high + 1));
		}
		for (int i = 0; i < L.size(); i++) {
			P.V[L[i]].right = false;
		}
		for (int i = 0; i < R.size(); i++) {
			P.V[R[i]].left = false;
		}
		for (int i = 0; i < L.size(); i++) {
			P.V[L[i]].left = true;
		}
		for (int i = 0; i < R.size(); i++) {
			P.V[R[i]].right = true;
		}
	}
	void mergeMonotoneChains(DCEL& P, std::vector<int>& Q, std::vector<int>& L, std::vector<int>& R) {
		int left = 0;
		int right = R.size() - 1;
		Q.push_back(L[left]);
		left++;
		right--;
		while (true) {
			if (left < L.size() - 1 && 0 < right) {
				if (P.V[L[left]].y > P.V[R[right]].y) {
					Q.push_back(L[left]);
					left++;
				}
				else {
					Q.push_back(R[right]);
					right--;
				}
			}
			else if (left < L.size() - 1 && right == 0) {
				Q.push_back(L[left]);
				left++;
			}
			else if (left == L.size() - 1 && 0 < right) {
				Q.push_back(R[right]);
				right--;
			}
			else {
				break;
			}
		}
		Q.push_back(L[left]);
	}
	void initializeEventQueue(DCEL& P, std::vector<int>& Q, Face& f) {
		std::vector<int> L, R;
		monotonePartitioning(P, L, R, f);
		mergeMonotoneChains(P, Q, L, R);
	}
	void generateFaces(DCEL& P, int sizeE, int sizeF) {
		if (sizeE < P.E.size()) {
			P.F.erase(P.F.begin() + (sizeF - 1), P.F.end());
			for (int i = sizeE; i < P.E.size(); i++) {
				P.E[i].incidentFace = -1;
			}
			for (int i = sizeE; i < P.E.size(); i++) {
				if (P.E[i].incidentFace < 0) {
					P.F.resize(P.F.size() + 1);
					P.F[P.F.size() - 1].outerComponent = i;
					int j = i;
					do {
						P.E[j].incidentFace = P.F.size() - 1;
						j = P.E[j].next;
					} while (j != i);
				}
			}
		}
	}
	void monotoneDecomposition(DCEL& P) {
		int sizeE = P.E.size();
		determineVertexType(P);
		std::vector<int> Q;
		initializeEventQueue(P, Q);
		StatusStructure T;
		for (int i = 0; i < Q.size(); i++) {
			if (P.V[Q[i]].type == "START") {
				handleStartVertex(P, T, Q[i]);
			}
			else if (P.V[Q[i]].type == "END") {
				handleEndVertex(P, T, Q[i]);
			}
			else if (P.V[Q[i]].type == "SPLIT") {
				handleSplitVertex(P, T, Q[i]);
			}
			else if (P.V[Q[i]].type == "MERGE") {
				handleMergeVertex(P, T, Q[i]);
			}
			else if (P.V[Q[i]].type == "REGULAR") {
				handleRegularVertex(P, T, Q[i]);
			}
		}
		generateFaces(P, sizeE);
	}
	void monotonePolygonTriangulation(DCEL& P, Face& f) {
		std::vector<int> Q;
		initializeEventQueue(P, Q, f);
		std::stack<int> S;
		int c = Q[0];
		S.push(c);
		int s = S.top();
		c = Q[1];
		S.push(c);
		int t = S.top();
		for (int i = 2; i < Q.size(); i++) {
			c = Q[i];
			if (P.V[c].left == P.V[t].left && P.V[c].right == P.V[t].right) {
				while (P.V[t].left ? toLeft(P.V[s], P.V[t], P.V[c]) : !toLeft(P.V[s], P.V[t], P.V[c])) {
					addEdge(P, c, s);
					S.pop();
					if (S.size() == 1) {
						break;
					}
					t = S.top();
					S.pop();
					s = S.top();
					S.push(t);
				}
				s = S.top();
				S.push(c);
				t = S.top();
			}
			else if (P.V[c].left != P.V[t].left && P.V[c].right != P.V[t].right) {
				int top = t;
				while (S.size() > 1) {
					addEdge(P, c, t);
					S.pop();
					t = S.top();
				}
				S.pop();
				S.push(top);
				s = S.top();
				S.push(c);
				t = S.top();
			}
			else if (P.V[c].left && P.V[c].right) {
				S.pop();
				t = S.top();
				while (S.size() > 1) {
					addEdge(P, c, t);
					S.pop();
					t = S.top();
				}
			}
		}
	}
public:
	void triangulation(DCEL& P) {
		int sizeE = P.E.size();
		int sizeF = P.F.size();
		monotoneDecomposition(P);
		for (int i = sizeF - 1; i < P.F.size(); i++) {
			monotonePolygonTriangulation(P, P.F[i]);
		}
		generateFaces(P, sizeE, sizeF);
	}
};
