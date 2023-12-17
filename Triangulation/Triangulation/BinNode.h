#pragma once
#include <iostream>
#include "macro.h"

template <typename T>
class BinNode {
public:
	T data;
	BinNode<T>* parent;
	BinNode<T>* lc;
	BinNode<T>* rc;
	int height;
	RBColor color;
	BinNode() :parent(NULL), lc(NULL), rc(NULL), height(0), color(RB_RED) {}
	BinNode(T e, BinNode<T>* p = NULL, BinNode<T>* lc = NULL, BinNode<T>* rc = NULL, int h = 0, RBColor c = RB_RED) :data(e), parent(p), lc(lc), rc(rc), height(h), color(c) {}
	BinNode<T>* succ() {
		BinNode<T>* s = this;
		if (rc) {
			s = rc;
			while (HasLChild(*s)) {
				s = s->lc;
			}
		}
		else {
			while (IsRChild(*s)) {
				s = s->parent;
			}
			s = s->parent;
		}
		return s;
	}
};
